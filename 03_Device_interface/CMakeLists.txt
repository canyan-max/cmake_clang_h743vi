# ---------- device_interface ----------
add_library(device_interface INTERFACE)
target_include_directories(device_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
target_link_libraries(device_interface INTERFACE platform_common)
