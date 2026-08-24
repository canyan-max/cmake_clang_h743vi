# External chip driver boundaries

## Ownership

Separate the system by the knowledge each part owns:

| Part | Owns |
| --- | --- |
| Chip driver | Commands, registers, addressing rules, family differences, conversions, page boundaries, chip state and protocol sequencing |
| Driver transport | The minimum host operations needed to execute that protocol |
| Board resources | Semantic resource IDs, fitted addresses, electrical levels, dimensions and other static assembly facts without vendor types |
| BSP | Exact fitted model, static chip instance, transport adapter and stable board capability API |
| Platform bus contract | Generic bounded I2C, SPI, UART, GPIO, delay and time operations using opaque resource IDs |
| MCU binding and implementation | Resource-to-pin/handle mapping, vendor SDK, HAL/LL, interrupts, DMA and register access |
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

Board resources may be consumed independently by BSP and the MCU
implementation. Vendor-specific pin and peripheral-handle bindings are private
to the MCU implementation and must not be included by BSP or Service.

## Board data versus driver data

Keep a value in the driver instance when the protocol logic needs it at
runtime, for example capacity, page size, address width, selected family model,
base device address, calibration state, or an immutable transport pointer.

Keep a value in Board resources when it is a static assembled-board fact, for
example semantic bus/GPIO IDs, address-pin state, fitted address, active level,
or display dimensions. Keep the concrete fitted model, static driver instance,
transport adapter, and initialization assembly in BSP. Keep HAL handles, ports,
pins, and vendor peripheral instances in the MCU binding.

A property can be supplied by BSP and stored by the driver. Ownership answers
who selects the value; storage answers who needs it while executing.

## Platform resource identifiers

Treat `plat_gpio_id_t`, `plat_i2c_id_t`, `plat_spi_id_t`, and similar public
types as opaque category-specific identifiers. A project may define them over
one small `plat_resource_id_t` storage type, but public APIs should retain the
category-specific aliases rather than accepting one generic ID everywhere.

The generic Platform contract must not define board instances such as I2C0,
LED1, or a protocol UART. Board resources assign semantic IDs and resource
counts; BSP passes the appropriate board-purpose ID; the MCU implementation
validates it and maps it to a vendor handle through its private binding. Keep a
compile-time size check beside fixed mapping tables where the language and
toolchain support it.

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

