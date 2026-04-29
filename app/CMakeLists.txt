set(APP_NAME "${PROJECT_NAME}")

option(USE_DEBUG_PRINT_RUNTIMESTATS "Print task run-time statistics on serial" OFF)

set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID")
set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password")

include("${CMAKE_SOURCE_DIR}/cmake/FreeRTOS_Kernel_import.cmake")

set(${APP_NAME}_SOURCES
    $<$<BOOL:${USE_DEBUG_PRINT_RUNTIMESTATS}>:utils/run_time_stats.cpp>
    main.cpp
    tasks/LedBlinkTask.cpp
    tasks/NtpClientTask.cpp
    tasks/RtcTask.cpp
    tasks/StartUpTask.cpp
    )

add_executable(${APP_NAME}
    ${${APP_NAME}_SOURCES}
    )

target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/config"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    )

target_link_libraries(${APP_NAME} PRIVATE
    pico_stdlib
    hardware_rtc
    $<$<BOOL:${USE_DEBUG_PRINT_RUNTIMESTATS}>:hardware_timer>
    pico_cyw43_arch_lwip_sys_freertos
    FreeRTOS-Kernel-Static
    FreeRTOS-Kernel-Heap4
    pico_ssd1306_cpp
    )

set_target_properties(${APP_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)


target_compile_definitions(${APP_NAME} PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    $<$<BOOL:${USE_DEBUG_PRINT_RUNTIMESTATS}>:DEBUG_PRINT_RUNTIMESTATS=1>
)


# Set COMPILE_OPTIONS on source files directly
# to avoid errors pulled in from pico-sdk/FreeRTOS sources
set_source_files_properties(${${APP_NAME}_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-Wall;-Wextra;-Werror"
)

pico_set_program_name(${APP_NAME} "${PROJECT_NAME}")
pico_set_program_version(${APP_NAME} "${PROJECT_VERSION}")

pico_enable_stdio_uart(${APP_NAME} 0)
pico_enable_stdio_usb(${APP_NAME} 1)

pico_add_extra_outputs(${APP_NAME})