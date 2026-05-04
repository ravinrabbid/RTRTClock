#include "DisplayTask.h"

#include <variant>

namespace RTRTClock::Tasks {

namespace {

constexpr std::array<std::string_view, 12> MONTHS_STR = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
constexpr std::array<std::string_view, 7> DAYS_STR = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void print_time(PicoU8g2::I2cHal &display, const datetime_t &dt) {
    char hour_str[3];
    char min_str[3];

    snprintf(hour_str, sizeof(hour_str), "%02u", dt.hour);
    snprintf(min_str, sizeof(min_str), "%02u", dt.min);

    display.with_u8g2(u8g2_SetFont, u8g2_font_7Segments_26x42_mn);
    display.with_u8g2(u8g2_SetFontPosTop);

    display.with_u8g2(u8g2_DrawStr, 0, 0, hour_str);
    display.with_u8g2(u8g2_DrawStr,
                      display.displayWidth() -
                          display.with_u8g2(u8g2_GetStrWidth, min_str) + 5,
                      0, min_str);

    display.with_u8g2(u8g2_DrawDisc, (display.displayWidth() / 2) - 1, 11, 2,
                      U8G2_DRAW_ALL);
    display.with_u8g2(u8g2_DrawDisc, (display.displayWidth() / 2) - 1, 31, 2,
                      U8G2_DRAW_ALL);
}

void print_date(PicoU8g2::I2cHal &display, const datetime_t &dt) {
    char date_str[20];

    snprintf(date_str, sizeof(date_str), "%s %s %02u, %04d",
             DAYS_STR[dt.dotw].data(), MONTHS_STR[dt.month - 1].data(), dt.day,
             dt.year);

    display.with_u8g2(u8g2_SetFont, u8g2_font_pxplusibmvga8_mr);
    display.with_u8g2(u8g2_SetFontPosBottom);

    display.with_u8g2(u8g2_DrawStr, 0, display.displayHeight(), date_str);
}

constexpr std::string_view to_string(DisplayTask::Message message) {
    switch (message) {
    case DisplayTask::Message::WIFI_CONNECTING:
        return "Connecting...";
    case DisplayTask::Message::WIFI_CONNECTED:
        return "Connected.";
    case DisplayTask::Message::WIFI_RETRYING:
        return "Retrying...";
    }
    return "Unknown";
}

void print_message(PicoU8g2::I2cHal &display, DisplayTask::Message message) {
    display.with_u8g2(u8g2_SetFont, u8g2_font_pxplusibmvga8_mr);
    display.with_u8g2(u8g2_SetFontPosBottom);

    display.with_u8g2(u8g2_DrawStr, 0, display.displayHeight(),
                      to_string(message).data());
}

} // namespace

void DisplayTask::taskFunc() {
    while (true) {
        const auto message = m_signals.take();

        m_display.with_u8g2(u8g2_ClearBuffer);

        std::visit(
            [&](auto &&msg) {
                using T = std::decay_t<decltype(msg)>;
                if constexpr (std::is_same_v<T, datetime_t>) {
                    print_time(m_display, msg);
                    print_date(m_display, msg);
                } else if constexpr (std::is_same_v<T, Message>) {
                    print_message(m_display, msg);
                } else {
                    static_assert(false, "non-exhaustive visitor!");
                }
            },
            message);

        m_display.with_u8g2(u8g2_SendBuffer);
    }
}

DisplayTask::MessageSignal_t::ptr_t DisplayTask::getMessageSignal() const {
    return m_message_signal;
}

} // namespace RTRTClock::Tasks