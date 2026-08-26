add_library(button STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/button.c
)

target_include_directories(button PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
