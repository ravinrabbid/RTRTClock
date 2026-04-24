#include "LedBlinkTask.h"

#include "hardware/gpio.h"

namespace Piclock::Tasks {

namespace {

void initGpio(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

} // namespace

void LedBlinkTask::taskFunc() {
    initGpio(m_pin);

    const TickType_t frequency = pdMS_TO_TICKS(m_frequency);
    TickType_t last_wake_time = xTaskGetTickCount();

    int led_state = 0;

    while (true) {
        gpio_put(m_pin, (led_state = !led_state));

        xTaskDelayUntil(&last_wake_time, frequency);
    }
}

} // namespace Piclock::Tasks