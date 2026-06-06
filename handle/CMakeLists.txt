# ---------- HANDLE_SRC ----------
set(HANDLE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/storage_handle.c
)
add_library(handle STATIC ${HANDLE_SRC})

target_include_directories(handle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(handle PRIVATE adapter)