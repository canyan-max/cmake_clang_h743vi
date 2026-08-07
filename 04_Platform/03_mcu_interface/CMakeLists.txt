# ---------- mcu_interface ----------
add_library(mcu_interface INTERFACE)
target_include_directories(mcu_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
if(USE_DEBUG_LOG)
    target_compile_definitions(mcu_interface INTERFACE USE_DEBUG_LOG)
    target_link_libraries(mcu_interface INTERFACE EasyLog)
endif()
