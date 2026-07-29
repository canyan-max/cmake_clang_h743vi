# ---------- service ----------
set(SERVICE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_display.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/service_camera.c
)

set(SERVICE_DEPEND_LIB
    platform_device
)

add_library(service STATIC ${SERVICE_SRC})

target_include_directories(service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(service PRIVATE ${SERVICE_DEPEND_LIB})
