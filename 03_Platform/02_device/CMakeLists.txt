# ---------- platform_device ----------
set(PLATFORM_DEVICE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_display.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_eeprom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_indicator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/device_camera.c
)

set(PLATFORM_DEVICE_PRIVATE_LIB
    platform_bsp
    impl_mcu
    platform_mcu_interface
)

add_library(platform_device STATIC ${PLATFORM_DEVICE_SRC})

target_include_directories(platform_device PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(platform_device PUBLIC platform_common)
target_link_libraries(platform_device PRIVATE ${PLATFORM_DEVICE_PRIVATE_LIB})
