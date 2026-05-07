#ifndef TASKS_DISPLAYTASK_H_
#define TASKS_DISPLAYTASK_H_

#include "RtcTask.h"
#include "Task.hpp"
#include "TemperatureTask.h"
#include "pico_u8g2/I2cHal.hpp"
#include "utils/Signal.hpp"

#include <optional>

namespace RTRTClock::Tasks {

class DisplayTask : public StaticTask<configMINIMAL_STACK_SIZE + 128> {
  public:
    enum class Message : uint8_t {
        WIFI_CONNECTING,
        WIFI_CONNECTED,
        WIFI_RETRYING,
        WAIT_SYNC,
        NONE,
    };

    enum class Command : uint8_t {
        CYCLE_STATUS_BAR,
        CYCLE_CLOCK_MODE,
        CYCLE_TEMPERATURE_UNIT,
    };

    enum class StatusBarMode : uint8_t {
        DATE,
        TEMPERATURE,
    };

    enum class ClockMode : uint8_t {
        CLOCK24H,
        CLOCK12H,
    };

    enum class TemperatureUnit : uint8_t {
        CELSIUS,
        FAHRENHEIT,
    };

    using Config_t = PicoU8g2::I2cHal::Config;
    using MessageSignal_t = Utils::Signal<Message>;
    using CommandSignal_t = Utils::Signal<Command>;

  private:
    using SignalSet_t = Utils::SignalSet<datetime_t, float, Message, Command>;

    void taskFunc() override;

    PicoU8g2::I2cHal m_display;
    std::optional<datetime_t> m_datetime;
    Message m_message{Message::NONE};
    float m_temperature{};

    StatusBarMode m_statusbar_mode{StatusBarMode::DATE};
    ClockMode m_clock_mode;
    TemperatureUnit m_temperature_unit;

    MessageSignal_t::ptr_t m_message_signal;
    CommandSignal_t::ptr_t m_command_signal;

    SignalSet_t m_signals;

  public:
    DisplayTask(UBaseType_t priority, const Config_t &display_config,
                ClockMode clock_mode, TemperatureUnit temperature_unit,
                RtcTask::DatetimeSignal_t::ptr_t minute_signal,
                TemperatureTask::TemperatureSignal_t::ptr_t temperature_signal)
        : StaticTask{"Display Task", priority},
          m_display{display_config, u8g2_Setup_ssd1306_i2c_128x64_noname_f},
          m_clock_mode{clock_mode}, m_temperature_unit{temperature_unit},
          m_message_signal(std::make_shared<MessageSignal_t>()),
          m_command_signal(std::make_shared<CommandSignal_t>()),
          m_signals(std::move(minute_signal), std::move(temperature_signal),
                    m_message_signal, m_command_signal) {};

    [[nodiscard]] MessageSignal_t::ptr_t getMessageSignal() const;
    [[nodiscard]] CommandSignal_t::ptr_t getCommandSignal() const;

    void clearMessage();

    void cycleStatusBarMode();
    void cycleClockMode();
    void cycleTemperatureUnit();
};

} // namespace RTRTClock::Tasks

#endif // TASKS_DISPLAYTASK_H_