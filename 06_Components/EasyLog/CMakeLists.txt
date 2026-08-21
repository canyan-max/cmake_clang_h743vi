# ---------- EASYLOG_SRC ----------
set(EASYLOG_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/elog.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/elog_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/elog_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/elog_buf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/elog_async.c
)

add_library(EasyLog STATIC ${EASYLOG_SRC})

target_include_directories(EasyLog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
target_link_libraries(EasyLog PRIVATE SeggerRTT)
if(USE_JLINK_RTT)
    target_compile_definitions(EasyLog PRIVATE USE_JLINK_RTT)
endif()





