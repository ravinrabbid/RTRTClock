#ifndef TASKS_RTCTASK_H_
#define TASKS_RTCTASK_H_

#include "Task.hpp"

#include "utils/Signal.hpp"

#include <ctime>

namespace RTRTClock::Tasks {

class RtcTask : public StaticTask<configMINIMAL_STACK_SIZE + 64> {
  public:
    using DatetimeSignal_t = Utils::Signal<datetime_t>;
    using TimeSignal_t = Utils::Signal<time_t>;

  private:
    const TimeSignal_t::ptr_t m_ntp_update_signal;
    const DatetimeSignal_t::ptr_t m_minute_signal;

    const std::string m_tz;

    virtual void taskFunc() override;

  public:
    RtcTask(UBaseType_t priority, std::string_view tz)
        : StaticTask{"RTC Task", priority},
          m_ntp_update_signal{std::make_shared<TimeSignal_t>()},
          m_minute_signal{std::make_shared<DatetimeSignal_t>()}, m_tz{tz} {};

    TimeSignal_t::ptr_t getNtpUpdateSignal() const;
    DatetimeSignal_t::ptr_t getMinuteSignal() const;
};

} // namespace RTRTClock::Tasks

#endif // TASKS_RTCTASK_H_