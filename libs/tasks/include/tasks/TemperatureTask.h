#ifndef TASKS_TEMPERATURETASK_H_
#define TASKS_TEMPERATURETASK_H_

#include "Task.hpp"
#include "utils/Signal.hpp"

namespace RTRTClock::Tasks {

class TemperatureTask : public StaticTask<configMINIMAL_STACK_SIZE> {
  public:
    using TemperatureSignal_t = Utils::Signal<float>;

  private:
    uint m_peroid;
    float m_offset;

    TemperatureSignal_t::ptr_t m_temperature_signal;

    void taskFunc() override;

  public:
    TemperatureTask(UBaseType_t priority, uint peroid, float offset)
        : StaticTask{"Tmprtr Task", priority}, m_peroid{peroid},
          m_offset{offset},
          m_temperature_signal{std::make_shared<TemperatureSignal_t>()} {};

    [[nodiscard]] TemperatureSignal_t::ptr_t getTemperatureSignal() const;
};

} // namespace RTRTClock::Tasks

#endif // TASKS_TEMPERATURETASK_H_