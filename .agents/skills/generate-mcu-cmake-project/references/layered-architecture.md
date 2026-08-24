# Portable layered MCU architecture

| Area | Owns |
| --- | --- |
| `00_Board` | Vendor-independent resources and a separate target-specific MCU binding |
| `01_App` | Task loops, application startup and scheduling policy |
| `02_Service` | Business flow and product behavior |
| `04_Platform/01_common` | Common error and platform-wide public types |
| `04_Platform/02_bsp` | Stable board capabilities and concrete chip assembly |
| `04_Platform/03_mcu_interface` | Generic `plat_*` MCU contracts with opaque category IDs |
| `05_Impl/01_mcu` | Vendor implementation of `plat_*`, resource mapping, ISR, DMA and cache handling |
| `06_Components` | Reusable chip drivers, protocols, algorithms and software utilities |
| `platform_sdk/<vendor>` | External SDK, startup, linker and generated code |

The default dependency direction is:

```text
App -> Service -> public bsp_* capabilities
                     |-> reusable ChipDrivers
                     `-> generic plat_* MCU interfaces -> MCU Implementation
                                                          -> vendor SDK

Board resources -> BSP
Board resources -> MCU Implementation <- target-specific Board binding
```

`board_resources` is a shared, vendor-independent input for BSP and MCU
Implementation. The target binding is private to MCU Implementation.
`bsp_interface` is consumed by Service, never by MCU Implementation. Do not
create BSP-to-Impl implementation-library cycles; BSP calls `mcu_interface`,
while the final firmware links both implementations.

Do not create generic Device, Device Interface, or BSP Binding layers merely to
forward calls. Add an independent device abstraction only when it owns
aggregation, complex lifecycle, concurrency arbitration, buffering/filtering,
recovery, or genuinely replaceable-device semantics. Service normally uses
stable public `bsp_*` APIs; permit only infrastructure `plat_log`, `plat_time`,
or explicitly justified diagnostics. Vendor SDK/HAL types stop at MCU
Implementation.

## CMake target boundaries

- `board_resources`: header-only Board facts, consumed privately by BSP and MCU
  Implementation.
- `board_target_binding`: header-only vendor mappings, consumed privately only
  by MCU Implementation.
- `platform_common`: shared public error and common types.
- `mcu_interface`: header-only `plat_*` contract; no Board, BSP, HAL, or vendor
  dependency.
- `impl_mcu`: vendor implementation; privately consumes the two Board targets,
  `mcu_interface`, common types, and the external SDK.
- `bsp_interface`: Service-facing public BSP headers.
- `bsp`: privately consumes Board resources, `mcu_interface`, and optional
  ChipDrivers; it does not consume `impl_mcu`.
- Service depends on `bsp`; App depends on Service when Service exists.

An executable or firmware image links the top owned layer and `impl_mcu`.
Implementation libraries do not depend upward merely to make final symbols
available.
