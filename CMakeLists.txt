cmake_minimum_required(VERSION 3.27...4.2)

set(CONFIG_WIFI_SSID "" CACHE STRING "WiFi SSID")
set(CONFIG_WIFI_PASSWORD "" CACHE STRING "WiFi password")

set(U8G2_PATH ${CMAKE_SOURCE_DIR}/extern/u8g2 CACHE PATH "Path to u8g2.")

option(CONFIG_DEBUG_PRINT_RUNTIME_STATS "Print task run-time statistics on serial" OFF)

if((NOT CONFIG_WIFI_SSID) OR (NOT CONFIG_WIFI_PASSWORD))
    message(FATAL_ERROR "CONFIG_WIFI_SSID and CONFIG_WIFI_PASSWORD must be set.")
endif()

if((NOT DEFINED ENV{PICO_SDK_PATH}) AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
    set(PICO_SDK_FETCH_FROM_GIT ON)
endif()

if(NOT PICO_BOARD)
    set(PICO_BOARD "pico_w")
endif()

if((NOT DEFINED ENV{FREERTOS_KERNEL_PATH}) AND (NOT FREERTOS_KERNEL_PATH))
    set(FREERTOS_KERNEL_PATH ${CMAKE_SOURCE_DIR}/extern/FreeRTOS-Kernel) 
endif()

include(cmake/pico_sdk_import.cmake)

project(RTRTClock VERSION 0.1.0 LANGUAGES C CXX ASM)

include(cmake/clang-tidy.cmake)
include(cmake/u8g2_import.cmake)
include(cmake/FreeRTOS_Kernel_import.cmake)

configure_file(
    config/FreeRTOSConfig.h.in
    ${CMAKE_BINARY_DIR}/config/FreeRTOSConfig.h
    @ONLY
    )
    
configure_file(
    config/RTRTClockConfig.h.in
    ${CMAKE_BINARY_DIR}/config/RTRTClockConfig.h
    @ONLY
    )

pico_sdk_init()

add_subdirectory(libs)
add_subdirectory(app)