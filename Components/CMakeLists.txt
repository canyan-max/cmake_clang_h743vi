add_library(at24cxx_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ChipDrivers/Src/at24cxx_driver.c
)

target_include_directories(at24cxx_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ChipDrivers/Inc
)
