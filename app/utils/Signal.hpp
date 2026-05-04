#ifndef UTILS_SIGNAL_HPP_
#define UTILS_SIGNAL_HPP_

#include "FreeRTOS.h"
#include "queue.h"

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <variant>

namespace RTRTClock::Utils {

template <typename... Ts> class SignalSet;

template <typename T> class Signal {
  public:
    using ptr_t = std::shared_ptr<Signal>;

  private:
    template <typename... Ts> friend class SignalSet;

    QueueHandle_t m_queue_handle{nullptr};

    std::array<uint8_t, sizeof(T)> m_queue_storage;
    StaticQueue_t m_queue_buffer;

  public:
    Signal() {
        m_queue_handle = xQueueCreateStatic(
            1, sizeof(T), m_queue_storage.data(), &m_queue_buffer);
    };

    virtual ~Signal() { vQueueDelete(m_queue_handle); };

    Signal(const Signal &) = delete;
    Signal(Signal &&) = delete;
    Signal &operator=(const Signal &) = delete;
    Signal &operator=(Signal &&) = delete;

    void signal(const T &value) { xQueueOverwrite(m_queue_handle, &value); };
    void signal_from_isr(const T &value) {
        xQueueOverwriteFromISR(m_queue_handle, &value, nullptr);
    };

    T take() {
        T value;
        xQueueReceive(m_queue_handle, &value, portMAX_DELAY);
        return value;
    };

    std::optional<T> try_take() {
        T value;
        if (xQueueReceive(m_queue_handle, &value, 0) != pdPASS) {
            return std::nullopt;
        }
        return value;
    };

    void reset() { try_take(); };

    std::optional<T> peek() {
        T value;
        if (xQueuePeek(m_queue_handle, &value, 0) != pdPASS) {
            return std::nullopt;
        }

        return value;
    };
};

template <typename... Ts> class SignalSet {
  private:
    static constexpr size_t SIGNAL_COUNT = sizeof...(Ts);
    static constexpr size_t MAX_ELEMENT_SIZE = std::max({sizeof(Ts)...});

    std::tuple<std::shared_ptr<Signal<Ts>>...> m_signals;

    QueueSetHandle_t m_queue_set_handle{nullptr};

    std::array<uint8_t, MAX_ELEMENT_SIZE> m_queue_set_storage;
    StaticQueue_t m_queue_set_buffer;

  public:
    SignalSet(std::shared_ptr<Signal<Ts>>... signals)
        : m_signals(std::move(signals)...) {

        m_queue_set_handle = xQueueCreateSetStatic(
            SIGNAL_COUNT, m_queue_set_storage.data(), &m_queue_set_buffer);

        std::apply(
            [&](auto &&...sigs) {
                (..., xQueueAddToSet(sigs->m_queue_handle, m_queue_set_handle));
            },
            m_signals);
    };

    virtual ~SignalSet() { vQueueDelete(m_queue_set_handle); };

    SignalSet(const SignalSet &) = delete;
    SignalSet(SignalSet &&) = delete;
    SignalSet &operator=(const SignalSet &) = delete;
    SignalSet &operator=(SignalSet &&) = delete;

    std::variant<Ts...> take() {
        while (true) {
            const auto activated_handle =
                xQueueSelectFromSet(m_queue_set_handle, portMAX_DELAY);

            std::optional<std::variant<Ts...>> result =
                [&]<size_t... Is>(std::index_sequence<Is...>)
                -> std::optional<std::variant<Ts...>> {
                std::optional<std::variant<Ts...>> activated_value;
                (void)(... || (std::get<Is>(m_signals)->m_queue_handle ==
                                   activated_handle &&
                               [&] {
                                   auto value =
                                       std::get<Is>(m_signals)->try_take();

                                   if (value) {
                                       activated_value.emplace(
                                           std::in_place_index<Is>, *value);
                                   }

                                   return activated_value.has_value();
                               }()));
                return activated_value;
            }(std::make_index_sequence<
                    std::tuple_size_v<decltype(m_signals)>>{});

            if (result) {
                return *result;
            }
        }
    };
};

} // namespace RTRTClock::Utils

#endif // UTILS_SIGNAL_HPP_