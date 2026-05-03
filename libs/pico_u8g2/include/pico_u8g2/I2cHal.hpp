#ifndef PICO_U8G2_I2CHAL_H_
#define PICO_U8G2_I2CHAL_H_

#include "u8g2.h"

#include "hardware/i2c.h"

#include <array>
#include <cstdint>
#include <functional>
#include <utility>

namespace PicoU8g2 {

class I2cHal {
  public:
    struct Config {
        enum class Rotation : uint8_t {
            DEG_0,
            DEG_90,
            DEG_180,
            DEG_270,
            MIRROR_H,
            MIRROR_V,
        };

        i2c_inst *i2c_block;
        uint8_t i2c_address;
        uint i2c_baudrate_hz;
        uint i2c_sda_pin;
        uint i2c_scl_pin;

        Rotation rotation;
    };

    using u8g2_setup_fn_t = std::function<void(u8g2_t *, const u8g2_cb_t *,
                                               u8x8_msg_cb, u8x8_msg_cb)>;

  private:
    static uint8_t byteCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                                void *arg_ptr);
    static uint8_t gpioAndDelayCallback(u8x8_t *u8x8, uint8_t msg,
                                        uint8_t arg_int, void *arg_ptr);

    Config m_config;
    u8g2_t m_u8g2;

    std::array<uint8_t, 32> m_send_buffer{0};
    size_t m_send_buffer_fill{0};

  public:
    I2cHal(const Config &i2c_config, u8g2_setup_fn_t setup_fn);

    ~I2cHal() = default;

    I2cHal(const I2cHal &) = delete;
    I2cHal &operator=(const I2cHal &) = delete;
    I2cHal(I2cHal &&) = delete;
    I2cHal &operator=(I2cHal &&) = delete;

    uint16_t displayWidth() { return u8g2_GetDisplayWidth(&m_u8g2); };
    uint16_t displayHeight() { return u8g2_GetDisplayHeight(&m_u8g2); };

    template <typename Ret, typename... FnArgs, typename... Args>
        requires(std::convertible_to<Args, FnArgs> && ...)
    Ret with_u8g2(Ret (*fn)(u8g2_t *u8g2, FnArgs...), Args &&...args) {
        return fn(&m_u8g2, std::forward<Args>(args)...);
    }
};

} // namespace PicoU8g2

#endif // PICO_U8G2_I2CHAL_H_