#include "I2cHal.hpp"

#include "hardware/gpio.h"

#include <cstring>

namespace PicoU8g2 {

uint8_t I2cHal::byteCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                             void *arg_ptr) {
    if (u8x8_GetUserPtr(u8x8) == nullptr) {
        return 0;
    }
    I2cHal *hal = static_cast<I2cHal *>(u8x8_GetUserPtr(u8x8));

    switch (msg) {
    case U8X8_MSG_BYTE_INIT:
        // NOOP
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        hal->m_send_buffer_fill = 0;
        break;
    case U8X8_MSG_BYTE_SEND: {
        const auto *buffer = static_cast<const uint8_t *>(arg_ptr);
        const auto buffer_length = static_cast<size_t>(arg_int);

        const auto send_length =
            buffer_length + hal->m_send_buffer_fill > hal->m_send_buffer.size()
                ? hal->m_send_buffer.size() - hal->m_send_buffer_fill
                : buffer_length;

        std::copy(buffer, buffer + send_length,
                  hal->m_send_buffer.begin() + hal->m_send_buffer_fill);

        hal->m_send_buffer_fill += send_length;
    } break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        if (hal->m_send_buffer_fill > 0) {
            i2c_write_blocking(
                hal->m_config.i2c_block, hal->m_config.i2c_address,
                hal->m_send_buffer.data(), hal->m_send_buffer_fill, false);

            hal->m_send_buffer_fill = 0;
        }
        break;
    case U8X8_MSG_BYTE_SET_DC:
        // NOOP
        break;
    default:
        return 0;
    }
    return 1;
}

uint8_t I2cHal::gpioAndDelayCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                                     void *arg_ptr) {
    (void)u8x8;
    (void)arg_ptr;

    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        // NOOP
        break;
    case U8X8_MSG_DELAY_NANO:
        // NOOP
        break;
    case U8X8_MSG_DELAY_100NANO:
        sleep_us(1);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        sleep_us(static_cast<uint64_t>(arg_int) * 10);
        break;
    case U8X8_MSG_DELAY_MILLI:
        sleep_ms(static_cast<uint32_t>(arg_int));
        break;
    default:
        return 0;
    }
    return 1;
}

I2cHal::I2cHal(const Config &config, u8g2_setup_fn_t setup_fn)
    : m_config(config) {

    i2c_init(m_config.i2c_block, m_config.i2c_baudrate_hz);

    gpio_set_function(m_config.i2c_sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(m_config.i2c_scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(m_config.i2c_sda_pin);
    gpio_pull_up(m_config.i2c_scl_pin);

    const u8g2_cb_t *rotation_cb{nullptr};

    switch (m_config.rotation) {
    case Config::Rotation::DEG_0:
        rotation_cb = U8G2_R0;
        break;
    case Config::Rotation::DEG_90:
        rotation_cb = U8G2_R1;
        break;
    case Config::Rotation::DEG_180:
        rotation_cb = U8G2_R2;
        break;
    case Config::Rotation::DEG_270:
        rotation_cb = U8G2_R3;
        break;
    case Config::Rotation::MIRROR_H:
        rotation_cb = U8G2_MIRROR;
        break;
    case Config::Rotation::MIRROR_V:
        rotation_cb = U8G2_MIRROR_VERTICAL;
        break;
    }

    setup_fn(&m_u8g2, rotation_cb, &I2cHal::byteCallback,
             &I2cHal::gpioAndDelayCallback);

    u8g2_SetUserPtr(&m_u8g2, (void *)this);
    u8g2_InitDisplay(&m_u8g2);
    u8g2_ClearDisplay(&m_u8g2);
    u8g2_SetPowerSave(&m_u8g2, 0);
};
} // namespace PicoU8g2
