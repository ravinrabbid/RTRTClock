#include "FreeRTOS.h"
#include "task.h"

#include "hardware/timer.h"

#include <stdio.h>

namespace RTRTClock::Utils::RunTimeStats {

namespace {

const size_t STATS_BUFFER_SIZE = 1024;
StackType_t print_stats_task_stack[configMINIMAL_STACK_SIZE + STATS_BUFFER_SIZE];
StaticTask_t print_stats_task_buffer;

void print_stats_task_func(void *params) {
    (void)params;

    char stats_buffer[STATS_BUFFER_SIZE];

    while (true) {
        vTaskGetRunTimeStatistics(stats_buffer, STATS_BUFFER_SIZE);
        printf("\n--- Runtime Stats ---\n%s\n", stats_buffer);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

} // namespace

void print_stats_task_create() {
    xTaskCreateStatic(print_stats_task_func, "Print Stats", configMINIMAL_STACK_SIZE + STATS_BUFFER_SIZE, NULL,
                      tskIDLE_PRIORITY + 1UL, print_stats_task_stack, &print_stats_task_buffer);
}

} // namespace RTRTClock::Utils::RunTimeStats

uint32_t get_run_time_counter_value() { return time_us_32(); }