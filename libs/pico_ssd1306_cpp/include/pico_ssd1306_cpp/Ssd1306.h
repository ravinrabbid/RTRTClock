#ifndef PICO_SSD1306_CPP_SSD1306_H_
#define PICO_SSD1306_CPP_SSD1306_H_

#include <hardware/i2c.h>

#include <cstdint>
#include <span>

namespace Ssd1306 {

class Display {
  public:
    static const uint8_t HEIGHT = 64;
    static const uint8_t WIDTH = 128;
    static const uint8_t PAGES = HEIGHT / 8;
    static const size_t FRAME_SIZE = PAGES * WIDTH;

    using Frame_t = std::span<const uint8_t, FRAME_SIZE>;
    using Tile_t = std::span<const uint8_t>;

    enum class Control : uint8_t {
        COMMAND = 0x00,
        DATA = 0x40,
    };

    enum class Command : uint8_t {
        // Fundamental commands
        CONTRAST_CONTROL = 0x81,
        ENTIRE_DISPLAY_RAM = 0xA4,
        ENTIRE_DISPLAY_ON = 0xA5,
        NORMAL_DISPLAY = 0xA6,
        INVERSE_DISPLAY = 0xA7,
        DISPLAY_OFF = 0xAE,
        DISPLAY_ON = 0xAF,
        // Scrolling commands
        RIGHT_HORIZONTAL_SCROLL = 0x26,
        LEFT_HORIZONTAL_SCROLL = 0x27,
        VERTICAL_RIGHT_HORIZONTAL_SCROLL = 0x29,
        VERTICAL_LEFT_HORIZONTAL_SCROLL = 0x2A,
        DEACTIVATE_SCROLL = 0x2E,
        ACTIVATE_SCROLL = 0x2F,
        VERTICAL_SCROLL_AREA = 0xA3,
        // Addressing setting
        LOWER_COLUMN_START_ADDRESS = 0x00,
        HIGHER_COLUMN_START_ADDRESS = 0x10,
        MEMORY_ADDRESSING_MODE = 0x20,
        COLUMN_ADDRESS = 0x21,
        PAGE_ADDRESS = 0x22,
        PAGE_START_ADDRESS = 0xB0,
        // Hardware configuration
        DISPLAY_START_LINE = 0x40,
        SEGMENT_REMAP_0 = 0xA0,
        SEGMENT_REMAP_127 = 0xA1,
        MULTIPLEX_RATIO = 0xA8,
        COM_OUTPUT_SCAN_DIRECTION_NORMAL = 0xC0,
        COM_OUTPUT_SCAN_DIRECTION_REMAPPED = 0xC8,
        DISPLAY_OFFSET = 0xD3,
        COM_PINS_HARDWARE_CONFIGURATION = 0xDA,
        // Timing and driving scheme setting
        DISPLAY_CLOCK_DIVIDE_RATIO_OSCILLATOR_FREQUENCY = 0xD5,
        PRE_CHARGE_PERIOD = 0xD9,
        V_COMH_DESELECT_LEVEL = 0xDB,
        // Charge pump setting
        CHARGE_PUMP_SETTING = 0x8D
    };

    enum class MemoryAddressingMode : uint8_t {
        HORIZONTAL = 0x00,
        VERTICAL = 0x01,
        PAGE = 0x10,
    };

  private:
    template <size_t SIZE> struct CommandList {
        std::array<Command, SIZE + 1> data;

        constexpr CommandList(const std::array<Command, SIZE> &commands) {
            data[0] = static_cast<Command>(Control::COMMAND);
            std::copy(commands.cbegin(), commands.cend(), data.begin() + 1);
        }
    };

    uint8_t m_address;
    i2c_inst *m_i2c;

    std::array<uint8_t, FRAME_SIZE + 1> m_send_buffer;

    void writeCommand(Command cmd);
    template <size_t SIZE> void writeCommands(CommandList<SIZE> cmds);

    void writeData();

  public:
    Display(uint8_t address, i2c_inst *i2c);

    void show();

    void clearFrame();
    void putFrame(Frame_t frame);
    void putTile(Tile_t tile, int16_t x, int16_t y, uint8_t width,
                 uint8_t height);
    void putPixel(uint8_t x, uint8_t y, bool pixel);

    void powerOn();
    void powerOff();

    void setInvert(bool invert);
    void setHorizontalFlip(bool flip);
    void setVerticalFlip(bool flip);
    void setContrast(uint8_t contrast);
};

} // namespace Ssd1306

#endif // PICO_SSD1306_CPP_SSD1306_H_