---
name: generate-mcu-cmake-project
description: Generate or migrate a portable layered MCU firmware project using CMake and Ninja. Use when creating a new embedded C project, introducing the 00_Board to 05_Impl architecture, separating vendor SDK code, or fixing CMake target dependency boundaries for an MCU project.
---

# MCU CMake Project Generator

Create only the project-owned architecture. Keep vendor SDK, startup code,
linker scripts, generated initialization, and toolchain files outside the
reusable layers. Preserve an existing repository's layer names unless the user
explicitly requests migration to this template.

## Collect inputs

Confirm the target directory, toolchain, vendor SDK location, RTOS choice, board name, and required peripherals. Treat the SDK as optional: the skeleton must be useful before a chip is selected.

Read [references/layered-architecture.md](references/layered-architecture.md) and [references/c-comment-style.md](references/c-comment-style.md) and [references/layered-code-patterns.md](references/layered-code-patterns.md) before creating or migrating layers.

## Generate layout

Create project-owned `00_Board`, `01_App`, `02_Service`, `04_Platform`,
`05_Impl`, and optional `06_Components` directories as described in the
reference. Do not create generic Device, Device Interface, or BSP Binding
layers by default.

Keep vendor-independent resources in `board_resources.h`; keep target pin and
handle bindings in a target-specific Board header included only by MCU
Implementation. Apply the referenced C and header comment style to generated
project-owned source files. Put business configuration in App or Service. Keep
vendor headers and SDK source under `platform_sdk` or another clearly external
directory.

## CMake rules

- Create separate `INTERFACE` targets for `board_resources`, the target-specific
  Board binding, `platform_common`, `mcu_interface`, and `bsp_interface` when
  they are header-only contracts.
- Use `STATIC` libraries for implementations. Never solve missing headers by linking an implementation library.
- Export a dependency as `PUBLIC` when it appears in a public header; otherwise use `PRIVATE`.
- Let MCU Implementation depend privately on the vendor SDK,
  `board_resources`, the target-specific Board binding, `mcu_interface`, and
  common types. It must not depend on `bsp_interface`, the `bsp`
  implementation, Service, App, or ChipDrivers.
- Let BSP depend privately on `board_resources`, `mcu_interface`, and required
  ChipDrivers while exporting only `bsp_interface` and common public types.
- Add each owned source in its owning layer's `CMakeLists.txt`; do not keep a global owned-source list.

## Runtime boundary

Keep reset/startup, vector table, early clock setup, vendor ISR dispatch, and
linker scripts in the external platform package. MCU Implementation converts
hardware events into `plat_*` callbacks or events; BSP consumes those events
and exposes board capabilities. Let App own task loops and Service own business
flows.

## Verify

Configure from an empty build directory, build with the official command, inspect target dependencies for cycles and public-header leaks, and report vendor-specific prerequisites outside the skeleton.



## Generate minimal or layered skeleton

Run `scripts/generate_project.py --kind cmake --profile <minimal|layered>
--output <empty-directory>`. Minimal creates buildable architecture boundaries
and a simple LED path without coupling App directly to BSP. Layered additionally
creates a Service example. Add an independent Device abstraction only for a
current aggregation, complex lifecycle, concurrency, or replaceable-device
requirement. The generator refuses to overwrite non-empty directories.
