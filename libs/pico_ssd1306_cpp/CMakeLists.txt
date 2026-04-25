set(LIB_NAME "pico_ssd1306_cpp")

set(${LIB_NAME}_SOURCES
    lib/Ssd1306.cpp
    )

add_library(${LIB_NAME}
    ${${LIB_NAME}_SOURCES}
    )

target_include_directories(${LIB_NAME} 
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/pico_ssd1306_cpp"
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

target_link_libraries(${LIB_NAME}
    PRIVATE
        pico_stdlib
    PUBLIC
        hardware_i2c
    )

set_target_properties(${LIB_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Set COMPILE_OPTIONS on source files directly
# to avoid errors pulled in from pico-sdk/FreeRTOS sources
set_source_files_properties(${${LIB_NAME}_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-Wall;-Wextra;-Werror"
)