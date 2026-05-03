#include "tasks/DisplayTask.h"
#include "tasks/LedBlinkTask.h"
#include "tasks/NtpClientTask.h"
#include "tasks/StartUpTask.h"

#include "pico/stdlib.h"

#include <cstdio>

#if (DEBUG_PRINT_RUNTIMESTATS == 1)
#include "utils/run_time_stats.h"
#endif // ( DEBUG_PRINT_RUNTIMESTATS == 1 )

using namespace RTRTClock;

namespace {

constexpr std::string NTP_SERVER = "pool.ntp.org";
constexpr uint32_t NTP_UPDATE_INTVERVAL = 60 * 60 * 1000;

constexpr Tasks::DisplayTask::Config DISPLAY_CONFIG = {
    .i2c_block = i2c1,
    .i2c_address = 0x3C,
    .i2c_baudrate_hz = 1'000'000,
    .i2c_sda_pin = 2,
    .i2c_scl_pin = 3,
    .rotation = Tasks::DisplayTask::Config::Rotation::DEG_0,
};

Tasks::LedBlinkTask led_task{tskIDLE_PRIORITY + 3UL, 200};
Tasks::LedBlinkTask led_task2{tskIDLE_PRIORITY + 3UL, 500};
Tasks::RtcTask rtc_task{tskIDLE_PRIORITY + 2UL};
Tasks::NtpClientTask ntp_client_task{tskIDLE_PRIORITY + 1UL, NTP_SERVER,
                                     NTP_UPDATE_INTVERVAL,
                                     rtc_task.get_ntp_update_signal()};
Tasks::DisplayTask display_task{tskIDLE_PRIORITY + 1UL, DISPLAY_CONFIG,
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