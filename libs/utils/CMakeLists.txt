set(LIB_NAME "utils")

set(${LIB_NAME}_SOURCES
    lib/run_time_stats.cpp
    )

add_library(${LIB_NAME}
    ${${LIB_NAME}_SOURCES}
    )

target_include_directories(${LIB_NAME} 
    PRIVATE
        ${CMAKE_BINARY_DIR}/config
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

target_link_libraries(${LIB_NAME}
    PRIVATE
        $<$<BOOL:${CONFIG_DEBUG_PRINT_RUNTIME_STATS}>:hardware_timer>
    PUBLIC
        FreeRTOS-Kernel-Static
    )

set_target_properties(${LIB_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    CXX_SCAN_FOR_MODULES NO
)

enable_clang_tidy(${LIB_NAME})

# Set COMPILE_OPTIONS on source files directly
# to avoid errors pulled in from pico-sdk/FreeRTOS sources
set_source_files_properties(${${LIB_NAME}_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-Wall;-Wextra;-Werror"
)