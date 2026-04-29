#include "RtcTask.h"

#include "hardware/rtc.h"

#include "pico/util/datetime.h"

namespace RTRTClock::Tasks {

void RtcTask::taskFunc() {
    const TickType_t period = pdMS_TO_TICKS(1000);
    TickType_t last_wake_time = xTaskGetTickCount();

    rtc_init();

    auto datetime = m_signal->take();
    rtc_set_datetime(&datetime);

    while (true) {
        auto datetime = m_signal->try_take();
        if (datetime) {
            rtc_set_datetime(&*datetime);
            printf("\nNTP update\n");
        }

        datetime_t t{};
        char datetime_str[256];
        rtc_get_datetime(&t);
        datetime_to_str(datetime_str, sizeof(datetime_str), &t);
        printf("\r%s      ", datetime_str);

        xTaskDelayUntil(&last_wake_time, period);
    }
}

RtcTask::signal_t::ptr_t RtcTask::get_update_signal() { return m_signal; }

} // namespace RTRTClock::Tasks