#include "tasks/DisplayTask.h"
#include "tasks/LedBlinkTask.h"
#include "tasks/NtpClientTask.h"
#include "tasks/StartUpTask.h"

#include "RTRTClockConfig.h"

#include "pico/stdlib.h"

#include <cstdio>

#if (DEBUG_PRINT_RUNTIMESTATS == 1)
#include "utils/run_time_stats.h"
#endif // ( DEBUG_PRINT_RUNTIMESTATS == 1 )

using namespace RTRTClock;

namespace {

Tasks::LedBlinkTask led_task{tskIDLE_PRIORITY + 3UL, 200};
Tasks::LedBlinkTask led_task2{tskIDLE_PRIORITY + 3UL, 500};
Tasks::RtcTask rtc_task{tskIDLE_PRIORITY + 2UL, Config::RTC_TZ};
Tasks::NtpClientTask ntp_client_task{tskIDLE_PRIORITY + 1UL, Config::NTP_SERVER,
                                     Config::NTP_UPDATE_INTERVAL,
                                     rtc_task.get_ntp_update_signal()};
Tasks::DisplayTask display_task{tskIDLE_PRIORITY + 1UL, Config::DISPLAY_CONFIG,
                                rtc_task.get_minute_signal()};

std::array tasks{
    std::ref<Tasks::Task>(led_task),        //
    std::ref<Tasks::Task>(led_task2),       //
    std::ref<Tasks::Task>(rtc_task),        //
    std::ref<Tasks::Task>(ntp_client_task), //
    std::ref<Tasks::Task>(display_task)     //
};
Tasks::StartUpTask startup_task{tskIDLE_PRIORITY + 1UL, tasks};

void launch_tasks() {
    printf("Launching tasks\n");

    startup_task.create();

#if (DEBUG_PRINT_RUNTIMESTATS == 1)
    Utils::RunTimeStats::print_stats_task_create();
#endif // ( DEBUG_PRINT_RUNTIMESTATS == 1 )

    vTaskStartScheduler();

    panic("\"vTaskStartScheduler\" should never return");
}

} // namespace

int main() {
    stdio_init_all();

    printf("Starting RTRTClock\n");

    launch_tasks();

    return 0;
}