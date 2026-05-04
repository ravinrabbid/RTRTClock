#ifndef TASKS_STARTUPTASK_H_
#define TASKS_STARTUPTASK_H_

#include "DisplayTask.h"
#include "Task.hpp"

#include <functional>
#include <span>

namespace RTRTClock::Tasks {

class StartUpTask : public StaticTask<configMINIMAL_STACK_SIZE + 128> {
  private:
    const std::span<const std::reference_wrapper<Task>> m_tasks;

    virtual void taskFunc() override;

    void connectWifi();

    DisplayTask::MessageSignal_t::ptr_t m_message_signal;

  public:
    StartUpTask(UBaseType_t priority,
                std::span<std::reference_wrapper<Task>> tasks,
                DisplayTask::MessageSignal_t::ptr_t msg_sig)
        : StaticTask{"Start Up Task", priority}, m_tasks{tasks},
          m_message_signal{std::move(msg_sig)} {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_STARTUPTASK_H_