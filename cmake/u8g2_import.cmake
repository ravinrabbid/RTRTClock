if (NOT U8G2_PATH)
    message(FATAL_ERROR "u8g2 location was not specified. Please set U8G2_PATH.")
endif()

add_subdirectory(${U8G2_PATH} U8G2)

target_compile_options(u8g2 PUBLIC
    -ffunction-sections -fdata-sections
    )

target_link_options(u8g2 PUBLIC
    -Wl,-gc-sections
    )

target_compile_definitions(u8g2 PUBLIC
    U8X8_WITH_USER_PTR=1
    )
