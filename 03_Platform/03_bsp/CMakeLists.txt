# ---------- platform_bsp ----------
set(PLATFORM_BSP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_led.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_at24.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_st7789.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_ov2640.c
)

set(PLATFORM_BSP_DEPEND_LIB
    letter_shell
    front
    impl_mcu
)


add_library(platform_bsp STATIC ${PLATFORM_BSP_SRC})

target_include_directories(platform_bsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(platform_bsp PRIVATE ${PLATFORM_BSP_DEPEND_LIB})
