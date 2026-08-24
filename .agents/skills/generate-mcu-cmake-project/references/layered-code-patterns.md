# Layered C code patterns

Copy and rename these patterns selectively. Keep the file layout from
`c-comment-style.md`. Names that start with `vendor_` are placeholders for the
selected SDK and belong only in MCU Implementation.

## Common error contract

```c
typedef enum PLATFORM_ERR_T
{
    PLATFORM_ERR_OK = 0,
    PLATFORM_ERR_PARAM,
    PLATFORM_ERR_BUSY,
    PLATFORM_ERR_HW,
    PLATFORM_ERR_TIMEOUT,
} platform_err_t;
```

Public APIs validate their own inputs and preserve finite timeout semantics.
BSP adapters translate reusable chip-driver status into the project's public
error contract; a separate Binding layer is not required for that conversion.

## Board resources and target binding

Keep vendor-independent facts in `board_resources.h`:

```c
#include <stdint.h>

typedef uint8_t board_gpio_resource_id_t;

#define BOARD_GPIO_STATUS_LED      ((board_gpio_resource_id_t)0U)
#define BOARD_GPIO_RESOURCE_COUNT  ((board_gpio_resource_id_t)1U)
#define BOARD_STATUS_LED_ON_LEVEL  (1U)
```

Keep the selected MCU or SDK mapping in a separate target-specific header:

```c
/* board_<target>_binding.h: MCU Implementation only. */
#include "vendor_gpio.h"

#define BOARD_STATUS_LED_PORT  VENDOR_GPIO_PORT_A
#define BOARD_STATUS_LED_PIN   VENDOR_GPIO_PIN_1
```

BSP includes `board_resources.h`, but never includes the target binding. Service
and reusable Components include neither Board header.

## Platform resource IDs

Use one small storage type while preserving category-specific API types:

```c
/* plat_resource.h */
#include <stdint.h>
typedef uint8_t plat_resource_id_t;

/* plat_gpio.h */
typedef plat_resource_id_t plat_gpio_id_t;

platform_err_t plat_gpio_write(plat_gpio_id_t id, uint8_t level);
```

Do not put `LED1`, `I2C0`, protocol UART names, resource counts, or vendor
handles in the generic `plat_*` contract. Board owns the semantic IDs and
counts. Public APIs retain `plat_gpio_id_t`, `plat_i2c_id_t`, and similar aliases
instead of accepting a single unqualified `plat_resource_id_t` everywhere.

## MCU resource mapping

MCU Implementation is the only layer that includes both Board resources and
the target binding:

```c
typedef struct MCU_GPIO_RESOURCE_T
{
    vendor_gpio_port_t *p_port;
    uint16_t            pin;
} mcu_gpio_resource_t;

static const mcu_gpio_resource_t
    s_gpio_resources[BOARD_GPIO_RESOURCE_COUNT] = {
        [BOARD_GPIO_STATUS_LED] = {
            .p_port = BOARD_STATUS_LED_PORT,
            .pin    = BOARD_STATUS_LED_PIN,
        },
};

_Static_assert((sizeof(s_gpio_resources) / sizeof(s_gpio_resources[0])) ==
                   BOARD_GPIO_RESOURCE_COUNT,
               "GPIO resource table size mismatch");

platform_err_t plat_gpio_write(plat_gpio_id_t id, uint8_t level)
{
    if((uint32_t)id >= (uint32_t)BOARD_GPIO_RESOURCE_COUNT)
    {
        return PLATFORM_ERR_PARAM;
    }

    vendor_gpio_write(s_gpio_resources[id].p_port,
                      s_gpio_resources[id].pin,
                      level);
    return PLATFORM_ERR_OK;
}
```

Adapt vendor names and error conversion to the selected SDK. Preserve range
checks and compile-time table-size checks.

## BSP board capability

BSP gives Service a stable board capability and privately selects Board
resources:

```c
platform_err_t bsp_status_led_set(uint8_t enabled)
{
    uint8_t level = (0U != enabled) ? BOARD_STATUS_LED_ON_LEVEL
                                    : (uint8_t)!BOARD_STATUS_LED_ON_LEVEL;

    return plat_gpio_write(BOARD_GPIO_STATUS_LED, level);
}
```

For an external chip, BSP owns the static chip instance and transport adapter,
selects the fitted model and Board resource aliases, and maps driver errors.
The reusable driver owns commands, registers, addressing, conversion, and chip
protocol sequencing. Its Component must compile without Board, BSP, `plat_*`,
HAL, generated SDK, RTOS, or business headers.

## Service and App

Service may know the capability category but not the selected bus, chip model,
pin, DMA channel, or vendor handle:

```c
platform_err_t service_status_set(uint8_t active)
{
    return bsp_status_led_set(active);
}
```

App invokes Service and owns task or scheduler orchestration. It does not call
BSP, `plat_*`, Board, HAL, or generated peripheral APIs directly.

## Optional device abstraction

Do not generate a Device layer merely to validate and forward BSP calls. Add a
separate device abstraction only when it owns a real product concern such as
cross-device aggregation, complex lifecycle, concurrency arbitration,
buffering/filtering, recovery policy, or genuinely replaceable device
semantics.
