set(CHIP_DRIVERS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/at24cxx_driver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ov2640_driver.c
)

add_library(chip_drivers STATIC ${CHIP_DRIVERS_SRC})

target_include_directories(chip_drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
