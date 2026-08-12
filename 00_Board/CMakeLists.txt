# ---------- board_config ----------
# Board-specific facts shared by BSP and MCU implementation only.
add_library(board_config INTERFACE)
target_include_directories(board_config INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
target_link_libraries(board_config INTERFACE stm32cubemx)
