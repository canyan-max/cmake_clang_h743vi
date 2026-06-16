# ---------- SeggerHardFault ----------
set(SeggerHardFault_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER_HardFaultHandler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/HardFaultHandler.S
)
add_library(SeggerHardFault  OBJECT  ${SeggerHardFault_SRC})





