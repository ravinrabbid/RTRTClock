#ifndef TASKS_LEDBLINKTASK_H_
#define TASKS_LEDBLINKTASK_H_

#include "Task.hpp"

namespace RTRTClock::Tasks {

class LedBlinkTask : public StaticTask<configMINIMAL_STACK_SIZE> {
  private:
    uint m_frequency;

    virtual void taskFunc() override;

  public:
    LedBlinkTask(UBaseType_t priority, uint frequency)
        : StaticTask{"Led Task", priority}, m_frequency{frequency} {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_LEDBLINKTASK_H_