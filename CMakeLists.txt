cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME cmake-clang-h743vi)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# EasyLogger / Segger RTT switches — set OFF to compile them out entirely.
# NOTE: the USE_DEBUG_LOG / USE_JLINK_RTT defines themselves are attached to
# the platform_mcu_interface / EasyLog targets (not added globally here), so
# that any translation unit seeing USE_DEBUG_LOG also always has EasyLog's
# include path — a blanket add_compile_definitions() here previously put
# USE_DEBUG_LOG on every file in the project (HAL, FreeRTOS, ...) without the
# matching -I, which also confused clangd's header-only fallback resolution.
option(USE_DEBUG_LOG "Compile in EasyLogger-based debug logging" ON)
option(USE_JLINK_RTT "Route EasyLogger output through Segger RTT (else UART)" ON)


# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)
# Add user-defined sources of the cmakelists files in the project
add_subdirectory(Middlewares/LetterShell)
add_subdirectory(Middlewares/Kfifo)
add_subdirectory(Middlewares/Soc)
add_subdirectory(Middlewares/Dwt)
add_subdirectory(Middlewares/Front)
add_subdirectory(Middlewares/Minimp3)
add_subdirectory(Middlewares/Protocol)
add_subdirectory(Middlewares/SEGGER_RTT)
add_subdirectory(Middlewares/EasyLog)
# add_subdirectory(Middlewares/SeggerHardFault)
add_subdirectory(02_Service)
add_subdirectory(03_Device_interface)
add_subdirectory(03_Device)
add_subdirectory(04_Platform/01_common)
add_subdirectory(04_Platform/03_mcu_interface)
add_subdirectory(04_Platform/02_bsp)
add_subdirectory(04_Platform/02_bsp_binding)
add_subdirectory(05_Impl/01_mcu)
# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/cmds.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Core/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    # Add user defined libraries
    # SeggerHardFault
    letter_shell
    kfifo
    Soc
    service
    platform_common
    platform_bsp
    platform_bsp_binding
    platform_mcu_interface
    impl_mcu
    device
    dwt
    front
    minimp3
    EasyLog
)

# generate .hex and .bin files from the .elf file after building the executable
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating hex and bin files from ${CMAKE_PROJECT_NAME}.elf"

)
message("Generating the hex and bin files from the elf file: ${CMAKE_PROJECT_NAME}.elf")