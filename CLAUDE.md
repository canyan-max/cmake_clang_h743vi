# CLAUDE.md

This file provides guidance to Claude Code when working with code in this repository.

## Current Work

> **当前正在做的事，做完划掉，新的加进来。**

- [ ] （示例）给 AT24C02 驱动加上写保护检查
- [ ] （示例）移植 SPI Flash 驱动

---

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
- Alternative GCC toolchain exists at `cmake/gcc-arm-none-eabi.cmake` but presets use starm-clang.

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

## Coding Conventions

- **C standard**: C11
- **Naming**: types `xxx_t`, functions `module_action`, macros `UPPER_CASE`, files `driver_<peripheral>.c/.h`
- **Include order**: own header → bsp → middleware → HAL → standard library
- **Error handling**: return 0 on success, negative `int32_t` on error

## Common Tasks

### Add a shell command
1. Write handler: `static void my_cmd(int argc, char *argv[])`
2. Register: `SHELL_EXPORT_CMD(mycmd, "Description", my_cmd)`
3. Rebuild — done

### Add a new peripheral driver
1. Create `bsp_drivers/driver_<name>.c` and `.h` — implement ops (init/read/write)
2. Wrap in `bsp_handle/` if it needs a FreeRTOS task
3. Add shell commands for testing in `Core/Src/cmds.c`
4. Add source files to `CMakeLists.txt`

### Debug a HardFault
1. Check task stack overflow (increase `configMINIMAL_STACK_SIZE` or task stack depth)
2. Check for float usage in tasks (FPU disabled — float operations will fault)
3. Check kfifo buffer sizes are power-of-2
