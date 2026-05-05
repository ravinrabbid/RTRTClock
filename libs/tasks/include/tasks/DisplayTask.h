#ifndef TASKS_DISPLAYTASK_H_
#define TASKS_DISPLAYTASK_H_

#include "Task.hpp"

#include "RtcTask.h"

#include "pico_u8g2/I2cHal.hpp"

namespace RTRTClock::Tasks {

class DisplayTask : public StaticTask<configMINIMAL_STACK_SIZE + 128> {
  public:
    enum class Message : uint8_t {
        WIFI_CONNECTING,
        WIFI_CONNECTED,
        WIFI_RETRYING,
    };

    using Config_t = PicoU8g2::I2cHal::Config;
    using MessageSignal_t = Utils::Signal<Message>;

  private:
    using SignalSet_t = Utils::SignalSet<datetime_t, Message>;

    void taskFunc() override;

    PicoU8g2::I2cHal m_display;

    MessageSignal_t::ptr_t m_message_signal;
    SignalSet_t m_signals;

  public:
    DisplayTask(UBaseType_t priority, const Config_t &config,
                RtcTask::DatetimeSignal_t::ptr_t minute_signal)
        : StaticTask{"Display Task", priority},
          m_display{config, u8g2_Setup_ssd1306_i2c_128x64_noname_f},
          m_message_signal(std::make_shared<MessageSignal_t>()),
          m_signals(std::move(minute_signal), m_message_signal) {};

    [[nodiscard]] MessageSignal_t::ptr_t getMessageSignal() const;
};

} // namespace RTRTClock::Tasks

#endif // TASKS_DISPLAYTASK_H_