#ifndef UTILS_SIGNAL_HPP_
#define UTILS_SIGNAL_HPP_

#include "FreeRTOS.h"
#include "queue.h"

#include <array>
#include <memory>
#include <optional>

namespace RTRTClock::Utils {

template <typename T> class Signal {
  public:
    using ptr_t = std::shared_ptr<Signal>;

  private:
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

} // namespace RTRTClock::Utils

#endif // UTILS_SIGNAL_HPP_