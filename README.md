# RT-RTClock - A FreeRTOS based NTP-Clock

This Project explores FreeRTOS on the Raspberrypi PicoW by developing a C++ abstraction layer in the context of a simple desk clock.

## Building

See [pico-sdk readme](https://github.com/raspberrypi/pico-sdk/blob/master/README.md#quick-start-your-own-project) for a list of pre-requisites.

Use the environment variables `PICO_SDK_PATH` to use a local pico-sdk, and `PICO_BOARD` to select another target board.
By default the pico-sdk will be fetched from Github and the target board will be "pico_w".

```sh
git submodule update --init
cmake -B build
cmake --build build
```

The image will end up in `build/app/RTRTClock.uf2`.