# ---------- FRONT_SRC ----------
set(FRONT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/front.c
)

add_library(front STATIC ${FRONT_SRC})

target_include_directories(front PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)





