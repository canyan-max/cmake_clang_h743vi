# Layered C code patterns

Copy and rename these patterns selectively. Keep the file layout from `c-comment-style.md`; do not copy vendor types above `05_Impl`.

## Common error contract

```c
typedef enum PLATFORM_ERR_T
{
    PLATFORM_ERR_OK = 0,
    PLATFORM_ERR_PARAM,
    PLATFORM_ERR_HW,
    PLATFORM_ERR_TIMEOUT,
} platform_err_t;
```

Return `PLATFORM_ERR_PARAM` for public argument validation. Let Binding translate chip-driver states to `PLATFORM_ERR_HW` or `PLATFORM_ERR_TIMEOUT`.

## Device wrapper: validate then forward

```c
platform_err_t device_example_write(const uint8_t *p_data, uint16_t size)
{
    if((NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }
    return g_example_ops.pf_write(p_data, size);
}
```

Use this in `03_Device` when the API represents device semantics. Validate public inputs here; keep chip protocol details in BSP/Binding.

## Binding: translate a chip driver to a stable contract

```c
static platform_err_t example_binding_write(const uint8_t *p_data,
                                            uint16_t       size)
{
    chip_state_t ret = bsp_chip_write(p_data, size);

    switch(ret)
    {
        case CHIP_OK:
            return PLATFORM_ERR_OK;
        case CHIP_TIMEOUT:
            return PLATFORM_ERR_TIMEOUT;
        case CHIP_ERROR:
        default:
            return PLATFORM_ERR_HW;
    }
}

const example_ops_t g_example_ops = {
    .pf_write = example_binding_write,
};
```

Place this in `02_bsp_binding`. The binding owns chip-state to platform-error translation. Omit Binding for a simple, non-replaceable GPIO capability.

## GPIO port contract: use an ID enum

```c
/* 03_port_interface/Inc/port_gpio.h */
typedef enum PORT_GPIO_ID_T
{
    PORT_GPIO_ID_0 = 0,
    PORT_GPIO_ID_NUM,
} port_gpio_id_t;

platform_err_t port_gpio_write(port_gpio_id_t id, uint8_t level);

/* 05_Impl/<target>/Src/port_gpio.c */
platform_err_t port_gpio_write(port_gpio_id_t id, uint8_t level)\n{\n    if(id >= PORT_GPIO_ID_NUM)\n    {\n        return PLATFORM_ERR_PARAM;\n    }\n    /* Map id to the selected SDK's GPIO port and pin here. */\n    return PLATFORM_ERR_OK;\n}
```

Prefer a resource ID enum over `void *p_port` for new portable templates. Return `platform_err_t` from port APIs so invalid IDs and vendor failures propagate upward. Keep vendor port pointers and pin values private to `05_Impl`. Retain an opaque handle only when the resource is genuinely runtime-selected.

## App task loop

```c
void app_main_task(void *p_argument)
{
    (void)p_argument;
    if(PLATFORM_ERR_OK != service_app_init())
    {
        /* Log or enter the project-defined safe state. */
    }
    for(;;)
    {
        service_app_process();
        /* Use RTOS wait or a bare-metal scheduler hook as selected. */
    }
}
```

Keep RTOS primitives in App unless the project deliberately provides an OS port. Keep business operations in Service.
