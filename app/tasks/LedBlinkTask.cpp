#include "LedBlinkTask.h"

#include "pico/cyw43_arch.h"

namespace RTRTClock::Tasks {

void LedBlinkTask::taskFunc() {
    const TickType_t frequency = pdMS_TO_TICKS(m_frequency);
    TickType_t last_wake_time = xTaskGetTickCount();

    int led_state = 0;

    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (led_state = !led_state));

        xTaskDelayUntil(&last_wake_time, frequency);
    }
}

} // namespace RTRTClock::Tasks