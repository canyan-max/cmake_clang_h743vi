# ---------- LetterShell ----------
set(LETTER_SHELL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shell.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shell_cmd_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shell_companion.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shell_ext.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shell_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.c
)
set(LETTER_SHELL_DEPEND_LIB
    FreeRTOS
)
add_library(letter_shell STATIC ${LETTER_SHELL_SRC})
target_include_directories(letter_shell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(letter_shell PUBLIC ${LETTER_SHELL_DEPEND_LIB})
target_compile_options(letter_shell PRIVATE -Wno-cast-function-type-mismatch)




