#ifndef TASKS_NTPCLIENTTASK_H_
#define TASKS_NTPCLIENTTASK_H_

#include "Task.hpp"

#include "RtcTask.h"

namespace RTRTClock::Tasks {

class NtpClientTask : public StaticTask<configMINIMAL_STACK_SIZE + 256> {
  private:
    const std::string m_ntp_server;
    const uint32_t m_update_interval;

    const RtcTask::signal_t::ptr_t m_rtc_update_signal;

    virtual void taskFunc() override;

  public:
    NtpClientTask(UBaseType_t priority, std::string ntp_server,
                  uint32_t update_interval,
                  RtcTask::signal_t::ptr_t rtc_update_signal)
        : StaticTask{"NTP Client Task", priority},
          m_ntp_server(std::move(ntp_server)),
          m_update_interval(update_interval),
          m_rtc_update_signal(std::move(rtc_update_signal)) {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_NTPCLIENTTASK_H_