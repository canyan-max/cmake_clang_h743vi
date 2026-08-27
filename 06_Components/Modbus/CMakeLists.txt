set(MODBUS_RTU_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/modbus_rtu.c
)

add_library(modbus_rtu STATIC ${MODBUS_RTU_SRC})

target_include_directories(modbus_rtu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
