# ---------- platform_device_interface ----------
add_library(platform_device_interface INTERFACE)
target_include_directories(platform_device_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
target_link_libraries(platform_device_interface INTERFACE platform_common)
