# ---------- Minimp3 ----------
set(MINIMP3_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/minimp3_impl.c
)

add_library(minimp3 STATIC ${MINIMP3_SRC})

target_include_directories(minimp3 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# if use high level api can uncomment the following line to reduce code size
# target_compile_definitions(minimp3 PRIVATE MINIMP3_ONLY_MP3)