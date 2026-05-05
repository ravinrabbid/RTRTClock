#include "tasks/StartUpTask.h"

#include "pico/cyw43_arch.h"

namespace RTRTClock::Tasks {

static uint32_t to_cyw43_auth(StartUpTask::WifiConfig::Auth auth) {
    switch (auth) {
    case StartUpTask::WifiConfig::Auth::OPEN:
        return CYW43_AUTH_OPEN;
    case StartUpTask::WifiConfig::Auth::WPA_TKIP_PSK:
        return CYW43_AUTH_WPA_TKIP_PSK;
    case StartUpTask::WifiConfig::Auth::WPA2_AES_PSK:
        return CYW43_AUTH_WPA2_AES_PSK;
    case StartUpTask::WifiConfig::Auth::WPA2_MIXED_PSK:
        return CYW43_AUTH_WPA2_MIXED_PSK;
    case StartUpTask::WifiConfig::Auth::WPA3_SAE_AES_PSK:
        return CYW43_AUTH_WPA3_SAE_AES_PSK;
    case StartUpTask::WifiConfig::Auth::WPA3_WPA2_AES_PSK:
        return CYW43_AUTH_WPA3_WPA2_AES_PSK;
    }

    return 0;
}

void StartUpTask::connectWifi() {
    cyw43_arch_enable_sta_mode();

    m_message_signal->signal(DisplayTask::Message::WIFI_CONNECTING);
    printf("Connecting to Wifi...\n");

    while (true) {
        if (cyw43_arch_wifi_connect_blocking(
                m_wifi_config.ssid.data(), m_wifi_config.password.data(),
                to_cyw43_auth(m_wifi_config.auth_method)) != 0) {
            m_message_signal->signal(DisplayTask::Message::WIFI_RETRYING);
            printf("Failed to connect, retrying...\n");
        } else {
            m_message_signal->signal(DisplayTask::Message::WIFI_CONNECTED);
            printf("Wifi connected.\n");

            break;
        }
    }
}

void StartUpTask::taskFunc() {
    if (cyw43_arch_init() != 0) {
        panic("cyw43_arch_init failed");
    }

    for (Task &task : m_tasks) {
        task.create();
    }

    connectWifi();

    remove();
}

} // namespace RTRTClock::Tasks