#include "tasks/LedBlinkTask.h"
#include "tasks/NtpClientTask.h"
#include "tasks/StartUpTask.h"

#include "pico_ssd1306_cpp/Ssd1306.h"

#include "pico/stdlib.h"

#include <cstdio>

#if (DEBUG_PRINT_RUNTIMESTATS == 1)
#include "utils/run_time_stats.h"
#endif // ( DEBUG_PRINT_RUNTIMESTATS == 1 )

using namespace RTRTClock;

namespace {

inline constexpr std::string NTP_SERVER = "pool.ntp.org";
inline constexpr uint32_t NTP_UPDATE_INTVERVAL = 60 * 60 * 1000;

Tasks::LedBlinkTask led_task{tskIDLE_PRIORITY + 2UL, 200};
Tasks::LedBlinkTask led_task2{tskIDLE_PRIORITY + 2UL, 500};
Tasks::RtcTask rtc_task{tskIDLE_PRIORITY + 2UL};
Tasks::NtpClientTask ntp_client_task{tskIDLE_PRIORITY + 1UL, NTP_SERVER,
                                     NTP_UPDATE_INTVERVAL,
                                     rtc_task.get_update_signal()};

std::array tasks{
    std::ref<Tasks::Task>(led_task), std::ref<Tasks::Task>(led_task2),
    std::ref<Tasks::Task>(rtc_task), std::ref<Tasks::Task>(ntp_client_task)};
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