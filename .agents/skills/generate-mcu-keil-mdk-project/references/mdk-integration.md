# Keil MDK layered-project integration

Keep these Groups owned by the project when they contain files: Board, App,
Service, Platform Common, BSP, MCU Interface, MCU Implementation, and reusable
Components/ChipDrivers.

Keep these Groups external: Device Pack, CMSIS, vendor HAL/LL, startup, system file, generated peripheral initialization, linker/scatter file, and RTOS kernel.

The default dependency direction is:

```text
App -> Service -> public bsp_* capabilities
                     |-> reusable ChipDrivers
                     `-> generic plat_* MCU interfaces -> MCU Implementation
                                                          -> vendor SDK

Board resources -> BSP
Board resources -> MCU Implementation <- target-specific Board binding
```

`board_resources.h` is a vendor-independent contract shared by BSP and MCU
Implementation. A target-specific Board binding maps those resources to vendor
pins and handles and is private to MCU Implementation. `bsp_*` public headers
are the Service-facing board capability contract. Do not use Keil Group
ordering as a substitute for correct source-level dependencies.

Do not create generic Device, Device Interface, or BSP Binding Groups merely to
forward calls. Add an independent device abstraction only for a current need
such as aggregation, complex lifecycle, concurrency ownership, or replaceable
device semantics.

A `.uvprojx` encodes device-pack paths, compiler options, and target-specific
settings. Generate or edit it only when those inputs are known. Prefer a
vendor-generated MDK project as the platform base and add owned source Groups
without modifying generated files except in documented user sections. Use
relative paths where MDK supports them.

Keil include paths may be target-wide and therefore cannot enforce every layer
boundary. Add only required public include directories, never a repository-wide
catch-all, and verify forbidden includes separately. In particular, Service
must not include Board, `plat_*`, HAL, or generated peripheral headers; BSP must
not include the target-specific Board binding or HAL headers.
