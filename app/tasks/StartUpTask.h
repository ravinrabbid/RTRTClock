#ifndef TASKS_STARTUPTASK_H_
#define TASKS_STARTUPTASK_H_

#include "Task.hpp"

#include <functional>
#include <span>

namespace RTRTClock::Tasks {

class StartUpTask : public StaticTask<configMINIMAL_STACK_SIZE + 128> {
  private:
    const std::span<const std::reference_wrapper<Task>> m_tasks;

    virtual void taskFunc() override;

  public:
    StartUpTask(UBaseType_t priority,
                std::span<std::reference_wrapper<Task>> tasks)
        : StaticTask{"Start Up Task", priority}, m_tasks{tasks} {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_STARTUPTASK_H_