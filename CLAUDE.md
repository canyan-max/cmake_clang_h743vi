# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Flash

```bash
# Build (Debug)
./compile.sh
# Or manually:
cmake --preset Debug -B build/Debug -G Ninja
cmake --build build/Debug -j 16

# Flash via J-Link
./flash.sh
```

- **Compiler**: starm-clang (ST's LLVM/Clang for ARM embedded), target `arm-none-eabi -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard`
- **C library**: picolibc
- **Generator**: Ninja
- Alternaive GCC toolchain exists at `cmake/gcc-arm-none-eabi.cmake` but presets use starm-clang.

## Hardware

- **MCU**: STM32H743VIT6 (Cortex-M7, 480 MHz, 2048 KB Flash)
- **Console UART**: LPUART1 (PA9/PA10, 115200 baud) — used by LetterShell
- **Other peripherals**: I2C1 (PB8/PB9), SPI4 (PE11/PE12/PE14), TIM17 (HAL timebase), GPIO LEDs on PE9/PE10
- **Debug probe**: J-Link (SWD, 4000 kHz)

## Architecture

```
Application (Core/Src/main.c, freertos.c, cmds.c)
  └─ bsp_handle/        — higher-level driver wrappers
       └─ bsp_drivers/   — driver objects (LED, AT24C02 EEPROM)
            └─ st_platform/ — platform abstraction over HAL
                 └─ Drivers/ — STM32 HAL + CMSIS
```

**Driver pattern**: Drivers use C function-pointer tables (vtables). `led_driver_t` and `at24_driver_t` each contain an `ops` struct with `init/deinit/read/write` function pointers. Upper layers depend on the interface, not the implementation.

**Shell**: LetterShell runs over LPUART1. Commands are registered via `SHELL_EXPORT_CMD(cmd_name, description, function_ptr)` macro in any `.c` file — the linker collects them into the `.shellCommand` section. See `Core/Src/cmds.c` for examples.

**FreeRTOS**: V10.6.2 with CMSIS-RTOS V2 wrapper. Uses the Cortex-M4F port (`ARM_CM4F`), heap_4.c, TIM17 as HAL timebase (SysTick reserved for RTOS). **FPU is disabled** in FreeRTOS config (`configENABLE_FPU 0`) — no lazy FPU context saving.

## Key Rules

- **Kfifo** (`Middlewares/Kfifo/`): size must be a power of 2. Single-producer/single-consumer design — not internally thread-safe, add external mutex for multi-task use.
- **CubeMX re-generation**: Files under `Core/`, `Drivers/`, and `cmake/stm32cubemx/` are CubeMX-managed. User code goes between `USER CODE BEGIN/END` comments in `Core/Src/` files, or better, in `bsp_drivers/`, `bsp_handle/`, `st_platform/`, or `Middlewares/`.
- **Shell commands**: define in any `.c` file with `SHELL_EXPORT_CMD`; no need to register manually.
