# ---------- PROTOCOL_SRC ----------
set(PROTOCOL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/proto_simple.c
)

add_library(protocol STATIC ${PROTOCOL_SRC})

target_include_directories(protocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
