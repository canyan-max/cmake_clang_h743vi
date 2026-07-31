# ---------- impl_mcu ----------
set(IMPL_MCU_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_at24.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_st7789.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_ov2640.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_sys.c
)
set(IMPL_MCU_DEPEND_LIB
    platform_bsp
    platform_mcu_interface
    stm32cubemx
    letter_shell
    dwt
)

add_library(impl_mcu STATIC ${IMPL_MCU_SRC})
target_include_directories(impl_mcu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
target_link_libraries(impl_mcu PRIVATE ${IMPL_MCU_DEPEND_LIB})
