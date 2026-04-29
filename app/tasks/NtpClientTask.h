#ifndef TASKS_NTPCLIENTTASK_H_
#define TASKS_NTPCLIENTTASK_H_

#include "Task.hpp"

namespace RTRTClock::Tasks {

class NtpClientTask : public StaticTask<configMINIMAL_STACK_SIZE + 256> {
  private:
    const std::string m_ntp_server;
    const uint32_t m_update_interval;

    virtual void taskFunc() override;

  public:
    NtpClientTask(UBaseType_t priority, std::string ntp_server,
                  uint32_t update_interval)
        : StaticTask{"NTP Client Task", priority},
          m_ntp_server(std::move(ntp_server)),
          m_update_interval(update_interval) {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_NTPCLIENTTASK_H_