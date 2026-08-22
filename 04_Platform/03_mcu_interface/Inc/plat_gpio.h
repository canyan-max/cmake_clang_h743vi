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

/* typedef ------------------------------------------------------------------*/
typedef enum PLAT_GPIO_ID_T
{
    PLAT_GPIO_ID_LED1 = 0U,
    PLAT_GPIO_ID_LED2,
    PLAT_GPIO_ID_KEY1,
    PLAT_GPIO_ID_KEY2,
    PLAT_GPIO_ID_CAMERA_PWDN,
    PLAT_GPIO_ID_DISPLAY_DC,
    PLAT_GPIO_ID_DISPLAY_BACKLIGHT,
    PLAT_GPIO_ID_NUM,
} plat_gpio_id_t;

/* function  ----------------------------------------------------------------*/
platform_err_t plat_gpio_write(plat_gpio_id_t id, uint8_t level);
platform_err_t plat_gpio_read(plat_gpio_id_t id, uint8_t *p_level);
platform_err_t plat_gpio_toggle(plat_gpio_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_GPIO_H */
