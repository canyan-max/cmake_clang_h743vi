# ---------- bsp_interface ----------
# Public BSP driver contracts shared with their MCU implementations.
add_library(bsp_interface INTERFACE)
target_include_directories(bsp_interface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# ---------- bsp ----------
set(PLATFORM_BSP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_led.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_eeprom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_st7789.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_ov2640.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_uart.c
)

set(PLATFORM_BSP_DEPEND_LIB
    common
    mcu_interface
    board_config
    letter_shell
    front
    kfifo
    at24cxx_driver
)


add_library(bsp STATIC ${PLATFORM_BSP_SRC})

target_include_directories(bsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(bsp PRIVATE ${PLATFORM_BSP_DEPEND_LIB} bsp_interface)
