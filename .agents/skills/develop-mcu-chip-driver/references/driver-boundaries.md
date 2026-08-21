# External chip driver boundaries

## Ownership

Separate the system by the knowledge each part owns:

| Part | Owns |
| --- | --- |
| Chip driver | Commands, registers, addressing rules, family differences, conversions, page boundaries, chip state and protocol sequencing |
| Driver transport | The minimum host operations needed to execute that protocol |
| Board/BSP | Exact fitted model, address straps, selected bus, control pins, power/reset wiring, static instance and transport adapter |
| Platform bus contract | Generic bounded I2C, SPI, UART, GPIO, delay and time operations |
| MCU implementation | Vendor SDK, HAL/LL handles, interrupts, DMA and register access |
| Device layer | Optional product-facing lifecycle, buffering, filtering, aggregation or replaceable-device semantics |

The reusable driver must compile without board, Platform, MCU SDK, HAL, RTOS,
or application headers. Do not create chip-specific functions such as
`plat_at24_*` in the generic Platform layer.

## Dependency direction

The expected direction is:

```text
Service or Device
        -> BSP concrete capability
        -> reusable chip driver
        -> driver-defined transport callbacks
        -> generic Platform bus contract
        -> MCU/HAL implementation
```

The chip driver never depends upward on BSP or sideways on a particular
Platform bus API. The BSP adapter is the only place that understands both the
driver transport contract and the project's `plat_*` contract.

## Board data versus driver data

Keep a value in the driver instance when the protocol logic needs it at
runtime, for example capacity, page size, address width, selected family model,
base device address, calibration state, or an immutable transport pointer.

Keep a value in Board/BSP when it describes the assembled product, for example
the chosen bus ID, address-pin state, chip-select GPIO, reset/power pin, fitted
model, or the concrete values passed into driver initialization.

A property can be supplied by BSP and stored by the driver. Ownership answers
who selects the value; storage answers who needs it while executing.

## Optional layers and abstractions

- Do not add a Device layer when it would only validate and forward BSP calls.
- Add Device when it owns lifecycle, DMA buffers, concurrency, filtering,
  recovery, aggregation, or a genuinely replaceable semantic API.
- Do not use one universal transport for every chip. Define the smallest
  transport that matches each protocol.
- Add an opaque transport context only for real multi-instance, runtime routing,
  or reusable adapter requirements. A statically bound single-instance adapter
  can omit it.
- Do not initialize or deinitialize a shared MCU bus from a single chip driver.

## Transport semantics

Transport operations must make timeout and transaction semantics explicit.
Use unshifted 7-bit I2C addresses at portable boundaries. State whether an I2C
write/read requires repeated START and whether SPI chip select is controlled by
the transport or by separate callbacks. A compound transaction must not be
interleaved with another client.

Distinguish bus-transfer timeout from chip-operation timeout. The driver decides
when the chip protocol requires a ready poll or state wait; the adapter performs
the requested bounded host operation.

