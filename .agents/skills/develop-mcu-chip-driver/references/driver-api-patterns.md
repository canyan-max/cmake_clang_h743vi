# Driver and API patterns

## Public API

Expose complete chip capabilities rather than internal protocol steps. An
EEPROM API should provide initialization, bounded read, bounded write, and
readiness; page splitting and address encoding remain private. A sensor API
should return measurements rather than requiring callers to send commands,
poll busy bits, and convert raw samples themselves.

Public operations must validate null pointers, sizes, address ranges,
initialization state, enum values, and finite timeout requirements. Use
fixed-width integer types across module boundaries. Every hardware or chip-state
poll must terminate on a finite timeout.

Use standard `<stdint.h>` types directly for numeric widths. Do not introduce
project aliases such as `plat_u8_t` or `chip_u16_t`; they add no domain meaning.
Define project types for semantics such as status, model, resource ID, or state.
Each public header should include the standard headers required by its own
declarations instead of relying on transitive includes.

## Instance shape

A simple instance normally contains only state required by protocol execution:

```c
typedef struct EXAMPLE_DRIVER_T
{
    const example_transport_t *p_transport;
    uint32_t                   protocol_property;
    example_model_t            model;
    uint8_t                    initialized;
} example_driver_t;
```

This is a pattern, not a mandatory field list:

- Store family properties directly when only a few values are required.
- Introduce a config structure only when the parameter set is genuinely complex
  or must be passed around independently.
- Do not keep a work buffer when operations can use caller-provided buffers.
- If a bounded scratch buffer is unavoidable, make ownership, size, lifetime,
  alignment, and DMA/cache responsibility explicit.
- Avoid dynamic allocation. If a target project permits an allocator, require a
  documented lifetime and failure path rather than silently using it.

Initialization injects every required dependency and leaves the instance
uninitialized on failure. A chip driver's deinitialization must not tear down a
shared bus.

## Family and model support

Start from verified protocol differences, not a speculative model table. Add a
model enum or descriptor only when it selects real behavior such as address
encoding, register layout, capacity rules, command set, or timing. BSP selects
the fitted model and concrete properties; the driver applies the family rule.

Keep address resolution, page splitting, CRC, register encoding, conversion,
and protocol sequencing private unless callers genuinely need those operations
as part of the chip's public capability.

## Error contract

Provide enough information to distinguish at least:

- invalid parameter;
- not initialized;
- address or size outside the supported range;
- busy or not ready;
- chip-operation timeout;
- transport failure.

A small driver may let transport callbacks return the driver's status type when
that avoids two nearly identical enums. The BSP adapter maps project platform
errors into the transport subset and maps driver results back into the BSP's
public error contract. Do not collapse every failure into one generic error.

## Memory and concurrency

Default to static, bounded storage. The chip instance is not automatically
thread-safe; document that callers serialize access unless the surrounding BSP
or service provides locking. Keep RTOS mutexes out of a reusable driver. Ensure
compound bus transactions are atomic at the transport or Platform boundary.

