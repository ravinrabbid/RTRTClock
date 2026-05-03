#ifndef TASKS_DISPLAYTASK_H_
#define TASKS_DISPLAYTASK_H_

#include "Task.hpp"

#include "RtcTask.h"

#include "pico_u8g2/I2cHal.hpp"

namespace RTRTClock::Tasks {

class DisplayTask : public StaticTask<configMINIMAL_STACK_SIZE + 128> {
  public:
    using Config = PicoU8g2::I2cHal::Config;

  private:
    virtual void taskFunc() override;

    PicoU8g2::I2cHal m_display;

    const RtcTask::datetime_signal_t::ptr_t m_minute_signal;

  public:
    DisplayTask(UBaseType_t priority, const Config &config,
                RtcTask::datetime_signal_t::ptr_t minute_signal)
        : StaticTask{"Display Task", priority},
          m_display{config, u8g2_Setup_ssd1306_i2c_128x64_noname_f},
          m_minute_signal(std::move(minute_signal)) {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_DISPLAYTASK_H_