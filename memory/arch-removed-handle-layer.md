---
name: arch-simplified-fixed-hardware-layers
description: Removed redundant Handle, Device-interface, Binding, and Device forwarding layers.
metadata:
  type: project
---

Removed the redundant Handle, Device-interface, BSP-binding, and Device layers.

BSP exposes stable board capabilities and owns fitted-device assembly, DMA
buffers, cache maintenance, ISR bridging, and bounded transport adapters.
Reusable chip protocol remains in `06_Components/ChipDrivers`; HAL access
remains in `05_Impl/01_mcu`.

Current architecture: App → Service → BSP → ChipDrivers / plat_* → MCU Impl

Service owns application orchestration such as camera/display flow, OSD, and
protocol parsing. It consumes public BSP capabilities without seeing board
pins, HAL types, or MCU handles.

**Why:** This fixed-hardware product has no runtime device replacement. The
removed layers mostly validated parameters and forwarded one-to-one calls,
duplicating BSP capability APIs without owning lifecycle or policy.

**How to apply:** When switching a fitted chip (for example ST7789 to another
display controller), preserve the public `bsp_display_*` contract and replace
the private BSP assembly plus reusable chip driver.

## Camera/display pipeline status (2026-08-22)

- ST7789 protocol is implemented by `06_Components/ChipDrivers`; BSP assembles
  the fitted display over the generic `plat_spi` transport.
- SPI frame output uses interrupt-driven DMA with bounded chunking. Service
  starts an asynchronous display transfer and App waits on a dedicated indexed
  FreeRTOS task notification with a 100 ms timeout.
- STM32 HAL DCMI callbacks terminate in `05_Impl/01_mcu` and are converted to
  `plat_dcmi` events. BSP owns two 115,200-byte D2 SRAM camera buffers and uses
  acquire/release states with alternating Snapshot capture, so DCMI never
  overwrites a frame while SPI DMA is displaying it.
- `OV2640_PROFILE_RGB565_CIF_60FPS` uses native 400x296 CIF timing. Hardware
  measurements after flashing were 59.88 FPS and 59.82 FPS, with DCMI overrun
  and error counters both zero. The user confirmed no artifacts; CIF has a
  small color shift and lower detail than the previous SVGA 30 FPS profile.
- SPI4 currently runs at 60 MHz. The tested panel is stable, but this exceeds
  the conservative ST7789V serial timing specification, so extended temperature
  and duration testing is required before treating 60 FPS as a production
  guarantee.

Latest Debug memory use: DTCMRAM 12,784 bytes, AXI RAM 131,552 bytes, RAM_D2
225 KiB (78.12%), Flash 117,004 bytes. Debug build, J-Link flash, COM14 boot,
double-buffer ownership, and short-run hardware display were verified.
