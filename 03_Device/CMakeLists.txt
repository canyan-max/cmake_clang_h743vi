# ---------- device ----------
set(DEVICE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_display.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_eeprom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_indicator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_camera.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_uart.c
)

set(DEVICE_PRIVATE_LIB
    platform_bsp_binding
    platform_bsp
    platform_mcu_interface
    device_interface
)

add_library(device STATIC ${DEVICE_SRC})

target_include_directories(device PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(device PUBLIC platform_common)
target_link_libraries(device PRIVATE ${DEVICE_PRIVATE_LIB})
