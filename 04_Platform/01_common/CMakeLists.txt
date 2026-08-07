# ---------- common ----------
set(PLATFORM_COMMON_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/platform_version.c
)

add_library(common STATIC ${PLATFORM_COMMON_SRC})

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
