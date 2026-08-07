# ---------- platform_bsp_binding ----------
# Glue layer: connects BSP chip drivers to device_interface contracts.
# When replacing a peripheral chip, only this layer changes.

set(PLATFORM_BSP_BINDING_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_ov2640_intf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_st7789_intf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/bsp_at24_intf.c
)

add_library(platform_bsp_binding STATIC ${PLATFORM_BSP_BINDING_SRC})

target_link_libraries(platform_bsp_binding PRIVATE
    platform_bsp
    device_interface
    platform_mcu_interface
)
