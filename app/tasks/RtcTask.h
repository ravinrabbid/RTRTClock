#ifndef TASKS_RTCTASK_H_
#define TASKS_RTCTASK_H_

#include "Task.hpp"

#include "utils/Signal.hpp"

#include <time.h>

namespace RTRTClock::Tasks {

class RtcTask : public StaticTask<configMINIMAL_STACK_SIZE + 64> {
  public:
    using datetime_signal_t = Utils::Signal<datetime_t>;
    using time_signal_t = Utils::Signal<time_t>;

  private:
    const time_signal_t::ptr_t m_ntp_update_signal;
    const datetime_signal_t::ptr_t m_minute_signal;

    const std::string m_tz;

    virtual void taskFunc() override;

  public:
    RtcTask(UBaseType_t priority, std::string_view tz)
        : StaticTask{"RTC Task", priority},
          m_ntp_update_signal{std::make_shared<time_signal_t>()},
          m_minute_signal{std::make_shared<datetime_signal_t>()}, m_tz{tz} {};

    time_signal_t::ptr_t get_ntp_update_signal() const;
    datetime_signal_t::ptr_t get_minute_signal() const;
};

} // namespace RTRTClock::Tasks

#endif // TASKS_RTCTASK_H_