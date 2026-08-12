# Portable layered MCU architecture

| Area | Owns |
| --- | --- |
| `00_Board` | Board resource mapping, device addresses, feature presence |
| `01_App` | Task loops, application startup and scheduling policy |
| `02_Service` | Business flow and product behavior |
| `03_Device` | Device-semantic APIs and device state |
| `03_Device_interface` | Replaceable-device contracts |
| `04_Platform` | BSP drivers, binding and portable platform contracts |
| `05_Impl/<target>` | Implementation of platform contracts for a chip/SDK |
| `platform_sdk/<vendor>` | External SDK, startup, linker and generated code |

`App → Service → Device → BSP/Binding → port_interface → Impl → vendor SDK`.

`00_Board` is a shared configuration input for BSP and Impl. `bsp_interface` is a shared driver contract for BSP and Impl. Do not create `BSP ↔ Impl` implementation-library cycles.

Create `Device_interface + BSP binding` only for replaceable, stateful, or protocol-heavy devices. Keep simple GPIO capabilities as public BSP APIs until a real device-semantic API is needed. Allow Service to use stable public `bsp_*` and `plat_*` APIs for diagnostics and infrastructure; forbid vendor SDK/HAL types above Impl.
