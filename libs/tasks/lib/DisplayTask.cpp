#include "tasks/DisplayTask.h"

#include <array>
#include <variant>

namespace RTRTClock::Tasks {

namespace {

constexpr std::array<std::string_view, 12> MONTHS_STR = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
constexpr std::array<std::string_view, 7> DAYS_STR = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void print_time(PicoU8g2::I2cHal &display, const datetime_t &dt) {
    std::array<char, 3> hour_str{};
    std::array<char, 3> min_str{};

    snprintf(hour_str.data(), hour_str.size(), "%02u", dt.hour);
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
}

void print_date(PicoU8g2::I2cHal &display, const datetime_t &dt) {
    std::array<char, 20> date_str{};

    snprintf(date_str.data(), date_str.size(), "%s %s %02u, %04d",
             DAYS_STR.at(dt.dotw).data(), MONTHS_STR.at(dt.month - 1).data(), //NOLINT
             dt.day, dt.year);

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