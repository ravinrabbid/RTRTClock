#include "StartUpTask.h"

#include "RTRTClockConfig.h"

#include "pico/cyw43_arch.h"

namespace RTRTClock::Tasks {

void StartUpTask::connectWifi() {
    cyw43_arch_enable_sta_mode();

    m_message_signal->signal(DisplayTask::Message::WIFI_CONNECTING);
    printf("Connecting to Wifi...\n");

    while (true) {
        if (cyw43_arch_wifi_connect_blocking(Config::WIFI_SSID.data(),
                                             Config::WIFI_PASSWORD.data(),
                                             Config::WIFI_AUTH_METHOD)) {
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