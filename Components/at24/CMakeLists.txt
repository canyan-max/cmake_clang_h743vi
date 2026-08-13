add_library(at24_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/at24.c
)

target_include_directories(at24_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
