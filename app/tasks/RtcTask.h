#ifndef TASKS_RTCTASK_H_
#define TASKS_RTCTASK_H_

#include "Task.hpp"

#include "utils/Signal.hpp"

namespace RTRTClock::Tasks {

class RtcTask : public StaticTask<configMINIMAL_STACK_SIZE + 512> {
  public:
    using signal_t = Utils::Signal<datetime_t>;

  private:
    signal_t::ptr_t m_signal;

    virtual void taskFunc() override;

  public:
    RtcTask(UBaseType_t priority)
        : StaticTask{"RTC Task", priority},
          m_signal{std::make_shared<signal_t>()} {};

    signal_t::ptr_t get_update_signal();
};

} // namespace RTRTClock::Tasks

#endif // TASKS_RTCTASK_H_