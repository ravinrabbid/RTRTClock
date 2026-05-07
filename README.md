# RT-RTClock - A FreeRTOS based NTP-Clock

This Project explores FreeRTOS on the RaspberryPi Pico W by developing a C++ abstraction layer in the context of a simple desk clock.

## Building

See [pico-sdk readme](https://github.com/raspberrypi/pico-sdk/blob/master/README.md#quick-start-your-own-project) for a list of pre-requisites.

Use the cmake or environment variables `PICO_SDK_PATH` to use a local pico-sdk, and `PICO_BOARD` to select another target board.
By default the pico-sdk will be fetched from Github and the target board will be "pico_w".

```sh
git submodule update --init
cmake -B build -DCONFIG_WIFI_SSID="your_wifi_ssid" -DCONFIG_WIFI_PASSWORD="your_wifi_password"
cmake --build build
```

The image will end up in `build/app/RTRTClock.uf2`.

Add `-DCONFIG_DEBUG_PRINT_RUNTIME_STATS=ON` to enable extended heap and task run-time statistics on serial console.

## Hardware

A RaspberryPi Pico W and a SSD1306 128x64 I²C display connected to GPIO2(SDA) and GPIO3(SCL).

## Configuration

In `RTRTClockConfig.h.in` you can change:

- Wifi authentication method (default: WPA2_AES_PSK)
- NTP server (default: pool.ntp.org)
- NTP update interval (default: 1 hour)
- Timezone (default: CET/CEST)
- Temperature update interval (default: 20 seconds)
- Temperature offset (default: -4.0°C)
- Temperature unit (default: Celsius)
- Display clock mode (default: 24h)
- Display hardware config (default: I²C block 1, Address 0x3C, 1MHz, Pin 2+3)