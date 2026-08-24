---
name: generate-mcu-keil-mdk-project
description: Generate or migrate a portable layered MCU firmware project for Keil MDK. Use when creating an MDK-ARM project, applying the 00_Board to 05_Impl architecture to a vendor IDE project, organizing Keil Groups, or separating project-owned sources from device-pack and vendor-generated code.
---

# MCU Keil MDK Project Generator

Create a portable source architecture while treating `.uvprojx`, device packs,
startup files, scatter files, and vendor-generated initialization as
target-specific external platform content. Preserve an existing repository's
layer names unless the user explicitly requests migration to this template.

## Collect inputs

Confirm MDK version, device/pack, compiler version, project path, board name, RTOS, and required peripherals. Do not invent a `.uvprojx` for an unknown device pack or compiler configuration; create or import into the vendor-provided MDK project instead.

Read [references/mdk-integration.md](references/mdk-integration.md) and [references/c-comment-style.md](references/c-comment-style.md) and [references/layered-code-patterns.md](references/layered-code-patterns.md) before editing Groups or include paths.

## Generate source layout

Create the project-owned `00_Board`, `01_App`, `02_Service`, `04_Platform`,
`05_Impl`, and optional `06_Components` structure described in the reference.
Do not create generic Device, Device Interface, or BSP Binding layers by
default. Apply the referenced C and header comment style to generated
project-owned source files. Retain vendor code in its generated directory, or
place manually managed vendor content in an explicitly external SDK directory.

## MDK integration

- Add project-owned files to Groups matching their layer: Board, App, Service,
  Platform Common, BSP, MCU Interface, MCU Implementation, and reusable
  Components when present.
- Keep startup, system initialization, vectors, scatter file, vendor drivers, device pack, and RTOS kernel in separate external Groups.
- Add include paths only for public headers required by each Group; do not add every repository directory globally.
- Define CPU/FPU options, memory layout, device-pack settings, and target macros in MDK Target Options, not shared application headers.
- Keep vendor-independent resources in `board_resources.h`; keep target pin and
  handle bindings in a target-specific Board header that only MCU
  Implementation includes. Keep product options in App/Service and all vendor
  types and calls in MCU Implementation or generated vendor code.
- Maintain a plain-text Group/source manifest if a vendor tool owns `.uvprojx` regeneration.

## Verify

Build the active Debug target in MDK or with the official `UV4.exe -b` command. Confirm Group membership matches ownership, upper layers exclude vendor headers, and warnings are reported separately from errors.



## Generate minimal or layered skeleton

Run `scripts/generate_project.py --kind keil --profile <minimal|layered>
--output <empty-directory>`. Minimal creates the architecture boundaries and a
simple LED path without coupling App directly to BSP. Layered additionally
creates a Service example. Add an independent Device abstraction only when the
actual product needs cross-device aggregation, complex lifecycle, concurrency
ownership, or replaceable-device semantics. The generator refuses to overwrite
non-empty directories.
