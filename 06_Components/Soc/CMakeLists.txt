# ---------- Soc ----------
set(SOC_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ampere_hour.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ocv_calc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/soc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/direct_cur_resistan.c
)
add_library(Soc STATIC ${SOC_SRC})

target_include_directories(Soc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)


# target_compile_options(Soc PRIVATE -Wno-cast-function-type-mismatch)




