#include "Ssd1306.h"

#include <algorithm>
#include <array>

namespace Ssd1306 {

namespace {

inline Display::Command commandOr(Display::Command cmd, uint8_t val) {
    return static_cast<Display::Command>(static_cast<uint8_t>(cmd) | val);
}

} // namespace

void Display::writeCommand(Display::Command cmd) {
    writeCommands(CommandList(std::array{cmd}));
}

template <size_t SIZE>
void Display::writeCommands(Display::CommandList<SIZE> cmds) {
    i2c_write_blocking(
        m_i2c, m_address,
        reinterpret_cast<std::underlying_type_t<Command> *>(cmds.data.data()),
        cmds.data.size(), false);
}

Display::Display(uint8_t address, i2c_inst *i2c)
    : m_address(address), m_i2c(i2c) {

    const CommandList init_commands(std::array{
        Command::DISPLAY_OFF,
        Command::DISPLAY_CLOCK_DIVIDE_RATIO_OSCILLATOR_FREQUENCY,
        static_cast<Command>(0x80), // Datasheet default
        Command::MULTIPLEX_RATIO,
        static_cast<Command>(HEIGHT - 1),
        Command::DISPLAY_OFFSET,
        static_cast<Command>(0x00),
        commandOr(Command::DISPLAY_START_LINE, 0x00),
        Command::CHARGE_PUMP_SETTING,
        static_cast<Command>(0x14), // Internal VCC
        Command::MEMORY_ADDRESSING_MODE,
        static_cast<Command>(MemoryAddressingMode::HORIZONTAL),
        Command::SEGMENT_REMAP_0,
        Command::COM_OUTPUT_SCAN_DIRECTION_NORMAL,
        Command::COM_PINS_HARDWARE_CONFIGURATION,
        static_cast<Command>(WIDTH > 2 * HEIGHT ? 0x02 : 0x12),
        Command::CONTRAST_CONTROL,
        static_cast<Command>(0x9F),
        Command::PRE_CHARGE_PERIOD,
        static_cast<Command>(0xF1), // Internal VCC
        Command::V_COMH_DESELECT_LEVEL,
        static_cast<Command>(0x30),
        Command::ENTIRE_DISPLAY_RAM,
        Command::NORMAL_DISPLAY,
        Command::DISPLAY_ON,
    });

    writeCommands(init_commands);

    clearFrame();
    show();
}

void Display::show() {
    const CommandList addressing_commands(std::array{
        Command::COLUMN_ADDRESS,
        static_cast<Command>(0x00),
        static_cast<Command>(WIDTH - 1),
        Command::PAGE_ADDRESS,
        static_cast<Command>(0x00),
        static_cast<Command>(HEIGHT / 8 - 1),
    });

    writeCommands(addressing_commands);

    m_send_buffer.at(0) = static_cast<uint8_t>(Control::DATA);

    i2c_write_blocking(m_i2c, m_address, m_send_buffer.data(),
                       m_send_buffer.size(), false);
}

void Display::clearFrame() {
    std::fill(m_send_buffer.begin() + 1, m_send_buffer.end(), 0x00);
}

void Display::putFrame(Frame_t frame) { putTile(frame, 0, 0, WIDTH, HEIGHT); }

void Display::putTile(Tile_t tile, int16_t x, int16_t y, uint8_t width,
                      uint8_t height) {

    const uint8_t tile_bytes_per_row = (width + 7) / 8;

    for (uint8_t tile_row = 0; tile_row < height; ++tile_row) {
        const int16_t buffer_row = y + tile_row;
        if (buffer_row < 0 || buffer_row >= HEIGHT) {
            continue;
        }

        const uint8_t buffer_page = static_cast<uint8_t>(buffer_row) / 8;
        const uint8_t buffer_page_bit = static_cast<uint8_t>(buffer_row) % 8;

        for (uint8_t tile_column = 0; tile_column < width; ++tile_column) {
            const int16_t buffer_column = x + tile_column;
            if (buffer_column < 0 || buffer_column >= WIDTH) {
                continue;
            }

            const uint16_t tile_byte =
                tile_row * tile_bytes_per_row + (tile_column / 8);
            const uint8_t tile_bit = 7 - (tile_column % 8);
            const bool pixel = (tile[tile_byte] >> tile_bit) & 0x01;

            if (pixel) {
                m_send_buffer[buffer_page * WIDTH + buffer_column + 1] |=
                    static_cast<std::uint8_t>(1 << buffer_page_bit);
            } else {
                m_send_buffer[buffer_page * WIDTH + buffer_column + 1] &=
                    static_cast<std::uint8_t>(~(1 << buffer_page_bit));
            }
        }
    }
}

void Display::putPixel(uint8_t x, uint8_t y, bool pixel) {
    if (x >= WIDTH || y >= HEIGHT) {
        return;
    }

    const uint8_t page = y / 8;
    const uint8_t bit = y % 8;
    const uint8_t column = x;

    if (pixel) {
        m_send_buffer[page * WIDTH + column + 1] |=
            static_cast<std::uint8_t>(1 << bit);
    } else {
        m_send_buffer[page * WIDTH + column + 1] &=
            static_cast<std::uint8_t>(~(1 << bit));
    }
}

void Display::powerOn() { writeCommand(Command::DISPLAY_ON); }

void Display::powerOff() { writeCommand(Command::DISPLAY_OFF); }

void Display::setInvert(bool invert) {
    writeCommand(invert ? Command::INVERSE_DISPLAY : Command::NORMAL_DISPLAY);
}

void Display::setHorizontalFlip(bool flip) {
    writeCommand(flip ? Command::SEGMENT_REMAP_127 : Command::SEGMENT_REMAP_0);
}

void Display::setVerticalFlip(bool flip) {
    writeCommand(flip ? Command::COM_OUTPUT_SCAN_DIRECTION_REMAPPED
                      : Command::COM_OUTPUT_SCAN_DIRECTION_NORMAL);
}

void Display::setContrast(uint8_t contrast) {
    const CommandList cmds(
        std::array{Command::CONTRAST_CONTROL, static_cast<Command>(contrast)});

    writeCommands(cmds);
}

} // namespace Ssd1306