# ---------- service ----------
set(SERVICE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_display.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_camera.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_indicator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_osd.c
)

set(SERVICE_DEPEND_LIB
    platform_device
    platform_mcu_interface
    front
)

add_library(service STATIC ${SERVICE_SRC})

target_include_directories(service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(service PRIVATE ${SERVICE_DEPEND_LIB})
