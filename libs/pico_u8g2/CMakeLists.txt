set(LIB_NAME "pico_u8g2")

include("${CMAKE_SOURCE_DIR}/cmake/u8g2_import.cmake")

set(${LIB_NAME}_SOURCES
    lib/I2cHal.cpp
    )

add_library(${LIB_NAME}
    ${${LIB_NAME}_SOURCES}
    )

target_include_directories(${LIB_NAME} 
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/pico_u8g2"
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

target_link_libraries(${LIB_NAME}
    PRIVATE
        pico_stdlib
    PUBLIC
        hardware_i2c
        u8g2
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