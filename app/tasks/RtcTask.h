#ifndef TASKS_RTCTASK_H_
#define TASKS_RTCTASK_H_

#include "Task.hpp"

#include "utils/Signal.hpp"

namespace RTRTClock::Tasks {

class RtcTask : public StaticTask<configMINIMAL_STACK_SIZE + 64> {
  public:
    using signal_t = Utils::Signal<datetime_t>;

  private:
    const signal_t::ptr_t m_ntp_update_signal;
    const signal_t::ptr_t m_minute_signal;

    virtual void taskFunc() override;

  public:
    RtcTask(UBaseType_t priority)
        : StaticTask{"RTC Task", priority},
          m_ntp_update_signal{std::make_shared<signal_t>()},
          m_minute_signal{std::make_shared<signal_t>()} {};

    signal_t::ptr_t get_ntp_update_signal() const;
    signal_t::ptr_t get_minute_signal() const;
};

} // namespace RTRTClock::Tasks

#endif // TASKS_RTCTASK_H_