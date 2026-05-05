set(APP_NAME "${PROJECT_NAME}")

set(${APP_NAME}_SOURCES
    main.cpp
    )

add_executable(${APP_NAME}
    ${${APP_NAME}_SOURCES}
    )

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}/config
    )

target_link_libraries(${APP_NAME} PRIVATE
    pico_stdlib
    utils
    tasks
    )

set_target_properties(${APP_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    CXX_SCAN_FOR_MODULES NO
)

# Set COMPILE_OPTIONS on source files directly
# to avoid errors pulled in from pico-sdk/FreeRTOS sources
set_source_files_properties(${${APP_NAME}_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-Wall;-Wextra;-Werror"
)

enable_clang_tidy(${APP_NAME})

pico_set_program_name(${APP_NAME} "${PROJECT_NAME}")
pico_set_program_version(${APP_NAME} "${PROJECT_VERSION}")

pico_enable_stdio_uart(${APP_NAME} 0)
pico_enable_stdio_usb(${APP_NAME} 1)

pico_add_extra_outputs(${APP_NAME})