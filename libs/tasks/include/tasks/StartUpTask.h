#ifndef TASKS_STARTUPTASK_H_
#define TASKS_STARTUPTASK_H_

#include "DisplayTask.h"
#include "Task.hpp"

#include <functional>
#include <span>
#include <string>

namespace RTRTClock::Tasks {

class StartUpTask : public StaticTask<configMINIMAL_STACK_SIZE + 128> {
  public:
    struct WifiConfig {
        enum class Auth : uint8_t {
            OPEN,
            WPA_TKIP_PSK,
            WPA2_AES_PSK,
            WPA2_MIXED_PSK,
            WPA3_SAE_AES_PSK,
            WPA3_WPA2_AES_PSK,
        };

        std::string ssid;
        std::string password;
        Auth auth_method;
    };

  private:
    const std::span<const std::reference_wrapper<Task>> m_tasks;
    const WifiConfig m_wifi_config;

    DisplayTask::MessageSignal_t::ptr_t m_message_signal;

    void taskFunc() override;

    void connectWifi();

  public:
    StartUpTask(UBaseType_t priority,
                std::span<std::reference_wrapper<Task>> tasks,
                WifiConfig wifi_config,
                DisplayTask::MessageSignal_t::ptr_t msg_sig)
        : StaticTask{"Start Up Task", priority}, m_tasks{tasks},
          m_wifi_config{std::move(wifi_config)},
          m_message_signal{std::move(msg_sig)} {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_STARTUPTASK_H_