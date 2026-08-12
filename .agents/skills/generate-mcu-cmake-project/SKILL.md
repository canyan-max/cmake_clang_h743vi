---
name: generate-mcu-cmake-project
description: Generate or migrate a portable layered MCU firmware project using CMake and Ninja. Use when creating a new embedded C project, introducing the 00_Board to 05_Impl architecture, separating vendor SDK code, or fixing CMake target dependency boundaries for an MCU project.
---

# MCU CMake Project Generator

Create only the project-owned architecture. Keep vendor SDK, startup code, linker scripts, generated initialization, and toolchain files outside the reusable layers.

## Collect inputs

Confirm the target directory, toolchain, vendor SDK location, RTOS choice, board name, and required peripherals. Treat the SDK as optional: the skeleton must be useful before a chip is selected.

Read [references/layered-architecture.md](references/layered-architecture.md) and [references/c-comment-style.md](references/c-comment-style.md) and [references/layered-code-patterns.md](references/layered-code-patterns.md) before creating or migrating layers.

## Generate layout

Create `00_Board/Inc`, `01_App/{Inc,Src}`, `02_Service/{Inc,Src}`, `03_Device/{Inc,Src}`, `03_Device_interface/Inc`, `04_Platform/{01_common,02_bsp,02_bsp_binding,03_port_interface}/{Inc,Src}`, `05_Impl/<target>/{Inc,Src}`, and `platform_sdk/<vendor>/`.

Use `00_Board` for resource mapping only. Apply the referenced C and header comment style to every generated project-owned source file. Put business configuration in App or Service. Keep vendor headers and SDK source under `platform_sdk` or another clearly external directory.

## CMake rules

- Create `INTERFACE` targets for header-only contracts: `board_config`, `bsp_interface`, and `port_interface`.
- Use `STATIC` libraries for implementations. Never solve missing headers by linking an implementation library.
- Export a dependency as `PUBLIC` when it appears in a public header; otherwise use `PRIVATE`.
- Let `05_Impl/<target>` depend on vendor SDK, `board_config`, `port_interface`, and `bsp_interface`; never link the `bsp` implementation library.
- Add each owned source in its owning layer's `CMakeLists.txt`; do not keep a global owned-source list.

## Runtime boundary

Keep reset/startup, vector table, early clock setup, vendor ISR dispatch, and linker scripts in the external platform package. Convert hardware events to public Device/BSP events in `05_Impl` or the startup adapter. Let App own task loops and Service own business flows.

## Verify

Configure from an empty build directory, build with the official command, inspect target dependencies for cycles and public-header leaks, and report vendor-specific prerequisites outside the skeleton.



## Generate minimal or layered skeleton

Run scripts/generate_project.py --kind <cmake|keil> --profile <minimal|layered> --output <empty-directory>. Use minimal for Board/App/BSP/Port/Impl. Use layered when Service, Device, Interface, and Binding are needed. The generator refuses to overwrite non-empty directories.
