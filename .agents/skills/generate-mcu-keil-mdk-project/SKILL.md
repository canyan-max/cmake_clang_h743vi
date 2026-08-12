---
name: generate-mcu-keil-mdk-project
description: Generate or migrate a portable layered MCU firmware project for Keil MDK. Use when creating an MDK-ARM project, applying the 00_Board to 05_Impl architecture to a vendor IDE project, organizing Keil Groups, or separating project-owned sources from device-pack and vendor-generated code.
---

# MCU Keil MDK Project Generator

Create the same portable source architecture as the CMake version while treating `.uvprojx`, device packs, startup files, scatter files, and vendor-generated initialization as target-specific external platform content.

## Collect inputs

Confirm MDK version, device/pack, compiler version, project path, board name, RTOS, and required peripherals. Do not invent a `.uvprojx` for an unknown device pack or compiler configuration; create or import into the vendor-provided MDK project instead.

Read [references/mdk-integration.md](references/mdk-integration.md) and [references/c-comment-style.md](references/c-comment-style.md) and [references/layered-code-patterns.md](references/layered-code-patterns.md) before editing Groups or include paths.

## Generate source layout

Create the project-owned `00_Board` through `05_Impl/<target>` structure described in the reference. Apply the referenced C and header comment style to every generated project-owned source file. Place vendor code in `platform_sdk/<vendor>` or retain its existing generated directory unchanged.

## MDK integration

- Add project-owned files to Groups matching their layer: Board, App, Service, Device, Device Interface, Platform Common, BSP, BSP Binding, Port Interface, and Implementation.
- Keep startup, system initialization, vectors, scatter file, vendor drivers, device pack, and RTOS kernel in separate external Groups.
- Add include paths only for public headers required by each Group; do not add every repository directory globally.
- Define CPU/FPU options, memory layout, device-pack settings, and target macros in MDK Target Options, not shared application headers.
- Keep board mapping in `00_Board`, product options in App/Service, and vendor handle types below `05_Impl`.
- Maintain a plain-text Group/source manifest if a vendor tool owns `.uvprojx` regeneration.

## Verify

Build the active Debug target in MDK or with the official `UV4.exe -b` command. Confirm Group membership matches ownership, upper layers exclude vendor headers, and warnings are reported separately from errors.



## Generate minimal or layered skeleton

Run scripts/generate_project.py --kind <cmake|keil> --profile <minimal|layered> --output <empty-directory>. Use minimal for Board/App/BSP/Port/Impl. Use layered when Service, Device, Interface, and Binding are needed. The generator refuses to overwrite non-empty directories.
