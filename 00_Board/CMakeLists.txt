# ---------- board_resources ----------
# Board-specific facts shared by BSP and MCU implementation only.
add_library(board_resources INTERFACE)
target_include_directories(board_resources INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# ---------- board_stm32h743_binding ----------
# Private STM32/CubeMX bindings consumed only by the MCU implementation.
add_library(board_stm32h743_binding INTERFACE)
target_include_directories(board_stm32h743_binding INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
target_link_libraries(board_stm32h743_binding INTERFACE
    stm32cubemx
)
