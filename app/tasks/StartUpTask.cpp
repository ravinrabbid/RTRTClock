#include "StartUpTask.h"

#include "pico/cyw43_arch.h"

namespace RTRTClock::Tasks {

namespace {

void connect_wifi() {
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wifi...\n");
    while (true) {
        if (cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASSWORD,
                                             CYW43_AUTH_WPA2_AES_PSK)) {
            printf("Failed to connect, retrying...\n");
        } else {
            printf("Wifi connected.\n");
            break;
        }
    }
}

} // namespace

void StartUpTask::taskFunc() {
    vTaskDelay(pdTICKS_TO_MS(5000));

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