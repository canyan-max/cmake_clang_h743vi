# Keil MDK layered-project integration

Keep these Groups owned by the project: Board, App, Service, Device, Device Interface, Platform Common, BSP, BSP Binding, Port Interface, and Implementation.

Keep these Groups external: Device Pack, CMSIS, vendor HAL/LL, startup, system file, generated peripheral initialization, linker/scatter file, and RTOS kernel.

`App → Service → Device → BSP/Binding → port_interface → Impl → vendor SDK`.

`00_Board` and `bsp_interface` are shared header contracts. Do not use Keil Group ordering as a substitute for correct source-level dependencies.

A `.uvprojx` encodes device-pack paths, compiler options, and target-specific settings. Generate or edit it only when those inputs are known. Prefer a vendor-generated MDK project as the platform base and add owned source Groups without modifying generated files except in documented user sections. Use relative paths where MDK supports them.
