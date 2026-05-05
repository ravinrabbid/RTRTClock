#include "FreeRTOS.h"

#include "utils/run_time_stats.h"

#ifdef CONFIG_DEBUG_PRINT_RUNTIME_STATS

#include "task.h"

#include "hardware/timer.h"

#include <array>
#include <cstdio>

namespace RTRTClock::Utils::RunTimeStats {

namespace {

constexpr size_t STATS_BUFFER_SIZE = 1024;

std::array<StackType_t, configMINIMAL_STACK_SIZE + STATS_BUFFER_SIZE>
    print_stats_task_stack;
StaticTask_t print_stats_task_buffer;

void print_stats_task_func(void *params) {
    (void)params;

    std::array<char, STATS_BUFFER_SIZE> stats_buffer{};

    while (true) {
        vTaskGetRunTimeStatistics(stats_buffer.data(), STATS_BUFFER_SIZE);
        printf("\n--- Runtime Stats ---\n%s\n", stats_buffer.data());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

} // namespace

void print_stats_task_create() {

    xTaskCreateStatic(print_stats_task_func, "Print Stats",
                      configMINIMAL_STACK_SIZE + STATS_BUFFER_SIZE, nullptr,
                      tskIDLE_PRIORITY + 1UL, print_stats_task_stack.data(),
                      &print_stats_task_buffer);
}

} // namespace RTRTClock::Utils::RunTimeStats

uint32_t get_run_time_counter_value() { return time_us_32(); }

#else // CONFIG_DEBUG_PRINT_RUNTIME_STATS

namespace RTRTClock::Utils::RunTimeStats {

void print_stats_task_create() {}

} // namespace RTRTClock::Utils::RunTimeStats

#endif // CONFIG_DEBUG_PRINT_RUNTIME_STATS