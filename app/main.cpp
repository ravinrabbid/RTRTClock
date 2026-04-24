#include "tasks/LedBlinkTask.h"

#include "pico/stdlib.h"

#include <stdio.h>
using namespace Piclock;

namespace {

Tasks::LedBlinkTask led_task{tskIDLE_PRIORITY + 2UL, PICO_DEFAULT_LED_PIN, 200};
Tasks::LedBlinkTask led_task2{tskIDLE_PRIORITY + 2UL, PICO_DEFAULT_LED_PIN, 500};

void launch_tasks() {
    printf("Launching tasks\n");

    led_task.create();
    led_task2.create();

    vTaskStartScheduler();
}

} // namespace

int main() {
    stdio_init_all();

    printf("Starting piclock\n");

    launch_tasks();

    return 0;
}