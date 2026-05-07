#include "tasks/TemperatureTask.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"

namespace RTRTClock::Tasks {

void TemperatureTask::taskFunc() {
    const TickType_t peroid = pdMS_TO_TICKS(m_peroid);
    TickType_t last_wake_time = xTaskGetTickCount();

    adc_init();
    adc_select_input(4);

    const float voltage_factor = 3.3F / (1 << 12);

    while (true) {
        adc_set_temp_sensor_enabled(true);
        const float adc_result =
            static_cast<float>(adc_read()) * voltage_factor;
        adc_set_temp_sensor_enabled(false);

        const float temperature =
            (27.0F - ((adc_result - 0.706F) / 0.001721F)) + m_offset;

        printf("Temp: %.1f\n", temperature);

        m_temperature_signal->signal(temperature);

        xTaskDelayUntil(&last_wake_time, peroid);
    }
}

TemperatureTask::TemperatureSignal_t::ptr_t
TemperatureTask::getTemperatureSignal() const {
    return m_temperature_signal;
};

} // namespace RTRTClock::Tasks