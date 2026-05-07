#include "tasks/DisplayTask.h"
#include "tasks/LedBlinkTask.h"
#include "tasks/NtpClientTask.h"
#include "tasks/StartUpTask.h"
#include "tasks/TemperatureTask.h"
#include "utils/run_time_stats.h"

#include "RTRTClockConfig.h"

#include "pico/stdlib.h"

using namespace RTRTClock;

namespace {

constexpr UBaseType_t LOWEST_PRIO = tskIDLE_PRIORITY + 1;
constexpr UBaseType_t LOW_PRIO = tskIDLE_PRIORITY + 2;
constexpr UBaseType_t MEDIUM_PRIO = tskIDLE_PRIORITY + 3;
constexpr UBaseType_t HIGH_PRIO = tskIDLE_PRIORITY + 4;
constexpr UBaseType_t HIGHEST_PRIO = tskIDLE_PRIORITY + 5;

void launch_tasks() {
    Tasks::LedBlinkTask led_task{LOWEST_PRIO, 1000};

    Tasks::RtcTask rtc_task{HIGHEST_PRIO, Config::RTC_TZ};
    Tasks::TemperatureTask temperature_task{
        MEDIUM_PRIO,                         //
        Config::TEMPERATURE_UPDATE_INTERVAL, //
        Config::TEMPERATURE_OFFSET};
    Tasks::NtpClientTask ntp_client_task{MEDIUM_PRIO,                 //
                                         Config::NTP_SERVER,          //
                                         Config::NTP_UPDATE_INTERVAL, //
                                         rtc_task.getNtpUpdateSignal()};
    Tasks::DisplayTask display_task{HIGH_PRIO,                        //
                                    Config::DISPLAY_CONFIG,           //
                                    Config::DISPLAY_CLOCK_MODE,       //
                                    Config::DISPLAY_TEMPERATURE_UNIT, //
                                    rtc_task.getMinuteSignal(),       //
                                    temperature_task.getTemperatureSignal()};

    std::array tasks{
        std::ref<Tasks::Task>(led_task),         //
        std::ref<Tasks::Task>(rtc_task),         //
        std::ref<Tasks::Task>(temperature_task), //
        std::ref<Tasks::Task>(ntp_client_task),  //
        std::ref<Tasks::Task>(display_task)      //
    };

    Tasks::StartUpTask startup_task{
        LOW_PRIO, //
        tasks,    //
        Tasks::StartUpTask::WifiConfig{.ssid = std::string{Config::WIFI_SSID},
                                       .password =
                                           std::string{Config::WIFI_PASSWORD},
                                       .auth_method = Config::WIFI_AUTH}, //
        display_task.getMessageSignal()};

    printf("Launching tasks\n");

    startup_task.create();

    Utils::RunTimeStats::print_stats_task_create(LOWEST_PRIO);

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