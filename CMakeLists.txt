cmake_minimum_required(VERSION 3.20...4.2)

if((NOT DEFINED ENV{PICO_SDK_PATH}) AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
    set(PICO_SDK_FETCH_FROM_GIT ON)
endif()

if((NOT DEFINED ENV{PICO_BOARD}) AND (NOT PICO_BOARD))
    set(PICO_BOARD "pico")
endif()

if((NOT DEFINED ENV{FREERTOS_KERNEL_PATH}) AND (NOT FREERTOS_KERNEL_PATH))
    set(FREERTOS_KERNEL_PATH "${CMAKE_SOURCE_DIR}/extern/FreeRTOS-Kernel")
endif()

include(cmake/pico_sdk_import.cmake)

project(RTRTClock VERSION 0.1.0
                  LANGUAGES C CXX ASM)

pico_sdk_init()

add_subdirectory(libs)
add_subdirectory(app)