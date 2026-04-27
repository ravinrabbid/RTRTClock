#include "tasks/LedBlinkTask.h"
#include "tasks/StartUpTask.h"

#include "pico_ssd1306_cpp/Ssd1306.h"

#include "pico/stdlib.h"

#include <stdio.h>

#if (DEBUG_PRINT_RUNTIMESTATS == 1)
#include "utils/run_time_stats.h"
#endif // ( DEBUG_PRINT_RUNTIMESTATS == 1 )

using namespace RTRTClock;

namespace {

Tasks::LedBlinkTask led_task{tskIDLE_PRIORITY + 2UL, 200};
Tasks::LedBlinkTask led_task2{tskIDLE_PRIORITY + 2UL, 500};

std::array tasks{std::ref<Tasks::Task>(led_task),
                 std::ref<Tasks::Task>(led_task2)};
Tasks::StartUpTask startup_task{tskIDLE_PRIORITY + 1UL, tasks};

void launch_tasks() {
    printf("Launching tasks\n");

    led_task.create();
    led_task2.create();

#if (DEBUG_PRINT_RUNTIMESTATS == 1)
    Utils::RunTimeStats::print_stats_task_create();
#endif // ( DEBUG_PRINT_RUNTIMESTATS == 1 )

    vTaskStartScheduler();
}

} // namespace

int main() {
    stdio_init_all();

    printf("Starting RTRTClock\n");

    launch_tasks();

    return 0;
}