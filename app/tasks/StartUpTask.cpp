#include "StartUpTask.h"

#include "RTRTClockConfig.h"

#include "pico/cyw43_arch.h"

namespace RTRTClock::Tasks {

namespace {

void connect_wifi() {
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wifi...\n");
    while (true) {
        if (cyw43_arch_wifi_connect_blocking(Config::WIFI_SSID.data(),
                                             Config::WIFI_PASSWORD.data(),
                                             Config::WIFI_AUTH_METHOD)) {
            printf("Failed to connect, retrying...\n");
        } else {
            printf("Wifi connected.\n");
            break;
        }
    }
}

} // namespace

void StartUpTask::taskFunc() {
    vTaskDelay(pdTICKS_TO_MS(2000));

    if (cyw43_arch_init() != 0) {
        panic("cyw43_arch_init failed");
    }

    connect_wifi();

    for (Task &task : m_tasks) {
        task.create();
    }

    remove();
}

} // namespace RTRTClock::Tasks