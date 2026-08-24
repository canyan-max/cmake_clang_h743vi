---
name: develop-mcu-chip-driver
description: Design, migrate, integrate, or review portable external MCU chip drivers for devices connected through I2C, SPI, UART, or similar buses. Use for sensors, EEPROMs, flashes, RTCs, displays, cameras, and other off-chip devices where chip protocol must be separated from board binding and MCU/HAL transport. Do not use for MCU-internal peripheral implementations or whole-project scaffolding.
---

# Develop MCU Chip Driver

Build a reusable C driver that owns chip protocol knowledge while the target
project owns board wiring and MCU transport implementation. Preserve the
repository's existing architecture and naming unless the user explicitly asks
for an architectural change.

## Select the work mode

- For architecture discussion or API design, read
  [references/driver-boundaries.md](references/driver-boundaries.md) and
  [references/driver-api-patterns.md](references/driver-api-patterns.md). Do not
  edit files until the user asks to implement the design.
- For implementation or migration, read all three references, including
  [references/integration-and-verification.md](references/integration-and-verification.md).
- For a review, read the boundaries and the review checklist in the integration
  reference. Report findings without changing code unless a fix is requested.

## Establish facts before designing

Inspect the target repository's instructions, neighboring drivers, build
targets, public error types, and bus contracts. Use the chip datasheet or an
authoritative protocol specification for commands, timing, addressing, reset
behavior, and model differences. Clearly mark assumptions when the exact model
or electrical connection is not known.

Decide these items before implementation:

- the complete public capabilities the chip should expose;
- protocol rules shared by the family and real model-specific differences;
- the smallest host operations required by the protocol;
- finite timeout, memory, concurrency, and transaction-atomicity rules;
- how logical host-resource IDs are owned by Board and resolved by the MCU
  implementation without exposing vendor handles upward;
- which values belong to the reusable instance and which belong to board
  assembly.

Avoid abstractions added only for hypothetical reuse. Add a configuration
object, opaque context, work buffer, Device layer, or separate build target only
when the current requirements justify it.

## Preserve scope and verify proportionally

When asked to implement, make focused changes to the chip driver, its BSP
adapter, and the owning build files. Do not refactor unrelated layers or update
other skills as a side effect. Build the affected target or preset. Run host
tests or hardware smoke tests when they exist and are in scope; do not flash
hardware without explicit authorization.

