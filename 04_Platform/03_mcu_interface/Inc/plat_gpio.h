/**
 ******************************************************************************
 *@file               :   plat_gpio.h
 *@brief              :   MCU GPIO abstraction for board control signals.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef PLAT_GPIO_H
#define PLAT_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "plat_error.h"
#include "plat_resource.h"

/* typedef ------------------------------------------------------------------*/
/** @brief Opaque GPIO resource identifier assigned by the active Board. */
typedef plat_resource_id_t plat_gpio_id_t;

/* function  ----------------------------------------------------------------*/
platform_err_t plat_gpio_write(plat_gpio_id_t id, uint8_t level);
platform_err_t plat_gpio_read(plat_gpio_id_t id, uint8_t *p_level);
platform_err_t plat_gpio_toggle(plat_gpio_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_GPIO_H */
