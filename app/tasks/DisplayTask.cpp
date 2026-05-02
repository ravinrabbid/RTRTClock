#include "DisplayTask.h"

namespace RTRTClock::Tasks {

void DisplayTask::taskFunc() {
    const TickType_t period = pdMS_TO_TICKS(1000);
    TickType_t last_wake_time = xTaskGetTickCount();

    while (true) {
        u8g2_ClearBuffer(m_display.getU8g2());

        u8g2_DrawBox(m_display.getU8g2(), 5, 5, 10, 10);
        u8g2_SetFont(m_display.getU8g2(), u8g2_font_scrum_tf);
        u8g2_DrawStr(m_display.getU8g2(), 0, 50, "Hello World");

        u8g2_SendBuffer(m_display.getU8g2());

        xTaskDelayUntil(&last_wake_time, period);
    }
}
} // namespace RTRTClock::Tasks