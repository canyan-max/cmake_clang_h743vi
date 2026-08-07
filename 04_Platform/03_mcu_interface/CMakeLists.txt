# ---------- platform_mcu_interface ----------
add_library(platform_mcu_interface INTERFACE)
target_include_directories(platform_mcu_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
if(USE_DEBUG_LOG)
    target_compile_definitions(platform_mcu_interface INTERFACE USE_DEBUG_LOG)
    target_link_libraries(platform_mcu_interface INTERFACE EasyLog)
endif()
