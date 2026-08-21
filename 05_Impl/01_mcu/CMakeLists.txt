# ---------- impl_mcu ----------
set(IMPL_MCU_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_dcmi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_st7789.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_sys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mcu_log.c
)
set(IMPL_MCU_DEPEND_LIB
    mcu_interface
    bsp_interface
    board_config
    stm32cubemx
    common
)

add_library(impl_mcu STATIC ${IMPL_MCU_SRC})
target_include_directories(impl_mcu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
target_link_libraries(impl_mcu PRIVATE ${IMPL_MCU_DEPEND_LIB})
