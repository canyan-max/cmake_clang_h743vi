# ---------- service ----------
set(SERVICE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_osd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_uart_test.c
)

set(SERVICE_DEPEND_LIB
    platform_device
    platform_mcu_interface
    front
    protocol
    stm32cubemx
)

add_library(service STATIC ${SERVICE_SRC})

target_include_directories(service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(service PRIVATE ${SERVICE_DEPEND_LIB})
