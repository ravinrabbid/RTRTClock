#include "RtcTask.h"

#include "hardware/rtc.h"

#include "pico/time.h"
#include "pico/util/datetime.h"

namespace RTRTClock::Tasks {

namespace {

constexpr datetime_t ALARM_ON_MINUTE = {.year = -1,
                                        .month = -1,
                                        .day = -1,
                                        .dotw = -1,
                                        .hour = -1,
                                        .min = -1,
                                        .sec = 0};

RtcTask::datetime_signal_t::ptr_t minute_signal{nullptr};

void alarm_callback() {
    datetime_t now;
    rtc_get_datetime(&now);

    if (minute_signal) {
        minute_signal->signal_from_isr(now);
    }
}

} // namespace

void RtcTask::taskFunc() {
    datetime_t datetime{};

    setenv("TZ", m_tz.data(), 1);
    tzset();

    const auto wait_and_set_time = [&]() {
        auto time = m_ntp_update_signal->take();
        printf("RTC received NTP update.\n");

        time_to_datetime(time, &datetime);

        rtc_set_datetime(&datetime);
        sleep_us(64); // Wait for RTC to settle
    };

    rtc_init();

    wait_and_set_time();
    m_minute_signal->signal(datetime);

    minute_signal = m_minute_signal;

    while (true) {
        rtc_set_alarm(&ALARM_ON_MINUTE, alarm_callback);
        wait_and_set_time();
    }
}

RtcTask::time_signal_t::ptr_t RtcTask::get_ntp_update_signal() const {
    return m_ntp_update_signal;
}

RtcTask::datetime_signal_t::ptr_t RtcTask::get_minute_signal() const {
    return m_minute_signal;
}

} // namespace RTRTClock::Tasks