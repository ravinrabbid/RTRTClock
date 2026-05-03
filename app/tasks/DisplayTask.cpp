#include "DisplayTask.h"

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

} // namespace

void DisplayTask::taskFunc() {
    while (true) {
        const auto dt = m_minute_signal->take();

        m_display.with_u8g2(u8g2_ClearBuffer);

        print_time(m_display, dt);
        print_date(m_display, dt);

        m_display.with_u8g2(u8g2_SendBuffer);
    }
}
} // namespace RTRTClock::Tasks