#include "tasks/DisplayTask.h"

#include "pico/cyw43_arch.h"
#include "timers.h"

#include <array>
#include <variant>

namespace RTRTClock::Tasks {

namespace {

constexpr std::array<std::string_view, 12> MONTHS_STR = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
constexpr std::array<std::string_view, 7> DAYS_STR = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void print_time(PicoU8g2::I2cHal &display, const datetime_t &dt,
                DisplayTask::ClockMode mode) {
    std::array<char, 3> hour_str{};
    std::array<char, 3> min_str{};

    auto hour = dt.hour;
    if (mode == DisplayTask::ClockMode::CLOCK12H) {
        hour = (hour > 12) ? static_cast<int8_t>(hour - 12) : hour;
    }

    snprintf(hour_str.data(), hour_str.size(), "%2u", hour);
    snprintf(min_str.data(), min_str.size(), "%02u", dt.min);

    display.with_u8g2(u8g2_SetFont, u8g2_font_7Segments_26x42_mn);
    display.with_u8g2(u8g2_SetFontPosTop);

    display.with_u8g2(u8g2_DrawStr, 0, 0, hour_str.data());
    display.with_u8g2(u8g2_DrawStr,
                      display.displayWidth() -
                          display.with_u8g2(u8g2_GetStrWidth, min_str.data()) +
                          5,
                      0, min_str.data());

    display.with_u8g2(u8g2_DrawDisc, (display.displayWidth() / 2) - 1, 11, 2,
                      U8G2_DRAW_ALL);
    display.with_u8g2(u8g2_DrawDisc, (display.displayWidth() / 2) - 1, 31, 2,
                      U8G2_DRAW_ALL);

    if (mode == DisplayTask::ClockMode::CLOCK12H) {
        display.with_u8g2(u8g2_SetFont, u8g2_font_pxplusibmvga8_mr);
        display.with_u8g2(u8g2_SetFontPosTop);

        if (dt.hour > 12) {
            display.with_u8g2(u8g2_DrawStr, 0, 0, "PM");
        } else {
            display.with_u8g2(u8g2_DrawStr, 0, 0, "AM");
        }
    }
}

void print_date(PicoU8g2::I2cHal &display, const datetime_t &dt) {
    std::array<char, 20> date_str{};

    // NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)
    snprintf(date_str.data(), date_str.size(), "%s %s %02u, %04d",
             DAYS_STR.at(dt.dotw).data(), MONTHS_STR.at(dt.month - 1).data(),
             dt.day, dt.year);
    // NOLINTEND(bugprone-suspicious-stringview-data-usage)

    display.with_u8g2(u8g2_SetFont, u8g2_font_pxplusibmvga8_mr);
    display.with_u8g2(u8g2_SetFontPosBottom);

    display.with_u8g2(u8g2_DrawStr, 0, display.displayHeight(),
                      date_str.data());
}

constexpr std::string_view to_string(DisplayTask::Message message) {
    switch (message) {
    case DisplayTask::Message::WIFI_CONNECTING:
        return "Connecting...";
    case DisplayTask::Message::WIFI_CONNECTED:
        return "Connected.";
    case DisplayTask::Message::WIFI_RETRYING:
        return "Retrying...";
    case DisplayTask::Message::WAIT_SYNC:
        return "Wait for sync...";
    case DisplayTask::Message::NONE:
        return "";
    }
    return "Unknown";
}

void print_message(PicoU8g2::I2cHal &display, DisplayTask::Message message) {
    display.with_u8g2(u8g2_SetFont, u8g2_font_pxplusibmvga8_mr);
    display.with_u8g2(u8g2_SetFontPosBottom);

    display.with_u8g2(u8g2_DrawStr, 0, display.displayHeight(),
                      to_string(message).data());
}

void print_temperature(PicoU8g2::I2cHal &display, float temperature) {
    std::array<char, 8> temperature_str{};

    snprintf(temperature_str.data(), temperature_str.size(),
             "%.1f"
             "\xb0"
             "C",
             temperature);

    display.with_u8g2(u8g2_SetFont, u8g2_font_pxplusibmvga8_mf);
    display.with_u8g2(u8g2_SetFontPosBottom);

    const auto temperature_str_width =
        display.with_u8g2(u8g2_GetStrWidth, temperature_str.data());

    display.with_u8g2(u8g2_DrawStr,
                      display.displayWidth() - temperature_str_width,
                      display.displayHeight(), temperature_str.data());
}

void print_wifi(PicoU8g2::I2cHal &display) {
    std::array<char, 7> wifi_str{};

    int32_t rssi = 0;

    cyw43_arch_lwip_begin();
    const int link_status =
        cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
    if (link_status == CYW43_LINK_UP) {
        cyw43_wifi_get_rssi(&cyw43_state, &rssi);
    }
    cyw43_arch_lwip_end();

    const auto rssi_to_percent = [](int32_t rssi) -> uint8_t {
        if (rssi >= -30) {
            return 100;
        }
        if (rssi <= -90) {
            return 0;
        }
        return (rssi + 90) * 100 / 60;
    };

    if (link_status == CYW43_LINK_UP) {
        snprintf(wifi_str.data(), wifi_str.size(), "W:%3d%%",
                 rssi_to_percent(rssi));
    } else {
        snprintf(wifi_str.data(), wifi_str.size(), "W: NC");
    }

    display.with_u8g2(u8g2_SetFont, u8g2_font_pxplusibmvga8_mf);
    display.with_u8g2(u8g2_SetFontPosBottom);

    display.with_u8g2(u8g2_DrawStr, 0, display.displayHeight(),
                      wifi_str.data());
}

DisplayTask::StatusBarMode nextMode(DisplayTask::StatusBarMode current) {
    switch (current) {
    case DisplayTask::StatusBarMode::DATE:
        return DisplayTask::StatusBarMode::TEMPERATURE;
    case DisplayTask::StatusBarMode::TEMPERATURE:
        return DisplayTask::StatusBarMode::DATE;
    }
    return current;
}

DisplayTask::ClockMode nextMode(DisplayTask::ClockMode current) {
    switch (current) {
    case DisplayTask::ClockMode::CLOCK24H:
        return DisplayTask::ClockMode::CLOCK12H;
    case DisplayTask::ClockMode::CLOCK12H:
        return DisplayTask::ClockMode::CLOCK24H;
    }
    return current;
}

void message_timer_cb(TimerHandle_t handle) {
    auto *const display_task =
        static_cast<DisplayTask *>(pvTimerGetTimerID(handle));

    display_task->clearMessage();
}

void statusbar_cycle_timer_cb(TimerHandle_t handle) {
    auto *const display_task =
        static_cast<DisplayTask *>(pvTimerGetTimerID(handle));

    display_task->cycleStatusBarMode();
}

} // namespace

void DisplayTask::taskFunc() {
    StaticTimer_t message_timer_buffer{};
    StaticTimer_t statusbar_cycle_timer_buffer{};

    auto *message_timer =
        xTimerCreateStatic("DisplyMsgTmr", pdMS_TO_TICKS(2000), pdFALSE, this,
                           message_timer_cb, &message_timer_buffer);

    auto *statusbar_cycle_timer = xTimerCreateStatic(
        "StatusBarTmr", pdMS_TO_TICKS(10000), pdTRUE, this,
        statusbar_cycle_timer_cb, &statusbar_cycle_timer_buffer);

    xTimerStart(statusbar_cycle_timer, 0);

    while (true) {
        const auto message = m_signals.take();

        m_display.with_u8g2(u8g2_ClearBuffer);

        std::visit(
            [&](auto &&msg) {
                using T = std::decay_t<decltype(msg)>;
                if constexpr (std::is_same_v<T, datetime_t>) {
                    m_datetime = msg;
                } else if constexpr (std::is_same_v<T, float>) {
                    m_temperature = msg;
                } else if constexpr (std::is_same_v<T, Message>) {
                    m_message = msg;
                    if (m_message != Message::NONE) {
                        xTimerReset(message_timer, 0);
                    }
                } else if constexpr (std::is_same_v<T, Command>) {
                    switch (msg) {
                    case Command::CYCLE_STATUS_BAR:
                        m_statusbar_mode = nextMode(m_statusbar_mode);
                        break;
                    case Command::CYCLE_CLOCK_MODE:
                        m_clock_mode = nextMode(m_clock_mode);
                        break;
                    }
                } else {
                    static_assert(false, "non-exhaustive visitor!");
                }
            },
            message);

        if (m_datetime) {
            print_time(m_display, *m_datetime, m_clock_mode);
        }

        if (m_message != Message::NONE) {
            print_message(m_display, m_message);
        } else if (m_datetime) {
            switch (m_statusbar_mode) {
            case StatusBarMode::DATE: {
                if (m_datetime) {
                    print_date(m_display, *m_datetime);
                }
            } break;
            case StatusBarMode::TEMPERATURE:
                print_wifi(m_display);
                print_temperature(m_display, m_temperature);
                break;
            }
        } else {
            print_message(m_display, Message::WAIT_SYNC);
        }
        m_display.with_u8g2(u8g2_SendBuffer);
    }
}

DisplayTask::MessageSignal_t::ptr_t DisplayTask::getMessageSignal() const {
    return m_message_signal;
}

DisplayTask::CommandSignal_t::ptr_t DisplayTask::getCommandSignal() const {
    return m_command_signal;
}

void DisplayTask::clearMessage() { m_message_signal->signal(Message::NONE); }

void DisplayTask::cycleStatusBarMode() {
    m_command_signal->signal(Command::CYCLE_STATUS_BAR);
}

void DisplayTask::cycleClockMode() {
    m_command_signal->signal(Command::CYCLE_CLOCK_MODE);
}

} // namespace RTRTClock::Tasks