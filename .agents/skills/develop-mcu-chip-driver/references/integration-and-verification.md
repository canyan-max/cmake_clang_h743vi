# Integration and verification

## Inspect before editing

Read repository instructions and identify:

- the reusable component directory and its public-header convention;
- the BSP implementation and board-resource configuration locations;
- the generic bus interface and MCU implementation;
- existing error, timeout, naming, comment, and CMake conventions;
- user-owned uncommitted changes that must be preserved.

Do not rename layers or migrate unrelated code merely to match an example in
this reference.

## Integrate the reusable driver

Place chip-protocol source and headers in the repository's reusable component
area. In the current layered project this is:

```text
06_Components/ChipDrivers/Inc/<chip>_driver.h
06_Components/ChipDrivers/Src/<chip>_driver.c
```

Drivers with the same dependency boundary are collected into the directory
library:

```cmake
set(CHIP_DRIVERS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/<chip>_driver.c
)

add_library(chip_drivers STATIC ${CHIP_DRIVERS_SRC})
target_include_directories(chip_drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
```

Add a new source to the existing list rather than recreating the target. Split
an independent target only when a driver has special third-party dependencies,
compile options, release packaging, or independent test requirements. Never
compile component source as part of the BSP target.

## Assemble the board instance

The BSP should:

1. define a static chip instance;
2. implement static transport callbacks using public `plat_*` operations;
3. convert Platform errors into the driver's transport error subset;
4. provide an immutable transport table;
5. select the fitted model, address, bus, pins, capacity, page size, and other
   board-known values during driver initialization;
6. expose stable board capability functions and translate driver errors to the
   BSP's public error type.

The BSP may privately depend on the component library, MCU-interface contract,
and board-resource target. The reusable driver must have no reverse dependency.
If driver headers appear in a BSP public header, reconsider whether chip details
are leaking through the board capability.

## Verification sequence

Match verification to the change and repository instructions:

1. Run formatting or diff checks used by the project.
2. Build the affected official preset and confirm no new warnings or errors.
3. Inspect includes and target dependencies for HAL leakage, public-header
   leakage, reverse dependencies, or implementation-library cycles.
4. If host tests exist, exercise range boundaries, zero lengths, null pointers,
   page/register boundaries, timeout paths, transport failures, and model rules.
5. Flash only with explicit authorization. For hardware smoke tests, verify
   initialization, one non-destructive read, and bounded failure behavior.
6. Perform a write/read test only when authorized. Preserve and restore the
   original contents when practical, and report if the operation is destructive.

Do not add a test directory or framework solely for symmetry. Add tests when
requested or when the repository already has an applicable test path.

## Review checklist

- The driver can compile without HAL, Board, Platform, RTOS, or business code.
- Public APIs express chip capabilities rather than byte-level protocol steps.
- Transport callbacks are minimal and have explicit transaction semantics.
- All public buffers, ranges, enum values, and initialization states are checked.
- Bus and chip waits have finite timeouts.
- Board wiring and the fitted model are selected by BSP.
- Context, config, work buffers, model tables, and Device layers have a current
  requirement rather than a hypothetical one.
- Memory ownership, concurrency, DMA alignment, and cache responsibility are
  explicit where applicable.
- CMake ownership follows the source layer and introduces no dependency cycle.

