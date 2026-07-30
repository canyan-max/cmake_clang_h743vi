/**
 ******************************************************************************
 *@file               :   device_indicator.c
 *@brief              :   Provide the device indicator  APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>           /* stdint lib header file. */
#include "device_indicator.h" /* device_indicator lib header file. */
#include "bsp_drv_led.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static led_driver_t g_leds[DEVICE_INDICATOR_NUM];
/* private  functions  ------------------------------------------------------*/
static inline led_driver_t *get_led(device_indicator_id_t id)
{
    if(id >= DEVICE_INDICATOR_NUM)
    {
        return NULL;
    }
    return &g_leds[id];
}
/* exported functions -------------------------------------------------------*/

platform_err_t device_indicator_init(device_indicator_id_t id)
{
    led_driver_t *p_led = get_led(id);
    if(NULL == p_led)
    {
        return PLATFORM_ERR_PARAM;
    }
    led_driver_state_t ret = bsp_led_init_by_id(p_led, (uint8_t)id);
    return (LED_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_indicator_on(device_indicator_id_t id)
{
    led_driver_t *p_led = get_led(id);
    if(NULL == p_led)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(LED_DRIVER_IS_INIT != p_led->is_init)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(NULL == p_led->p_led_ops || NULL == p_led->p_led_ops->pf_led_on)
    {
        return PLATFORM_ERR_PARAM;
    }
    led_driver_state_t ret = p_led->p_led_ops->pf_led_on(p_led);
    return (LED_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_indicator_off(device_indicator_id_t id)
{
    led_driver_t *p_led = get_led(id);
    if(NULL == p_led)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(LED_DRIVER_IS_INIT != p_led->is_init)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(NULL == p_led->p_led_ops || NULL == p_led->p_led_ops->pf_led_off)
    {
        return PLATFORM_ERR_PARAM;
    }
    led_driver_state_t ret = p_led->p_led_ops->pf_led_off(p_led);
    return (LED_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_indicator_blink(device_indicator_id_t id)
{
    led_driver_t *p_led = get_led(id);
    if(NULL == p_led)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(LED_DRIVER_IS_INIT != p_led->is_init)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(NULL == p_led->p_led_ops || NULL == p_led->p_led_ops->pf_led_blink)
    {
        return PLATFORM_ERR_PARAM;
    }
    led_driver_state_t ret = p_led->p_led_ops->pf_led_blink(p_led);
    return (LED_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

/* end of  file -------------------------------------------------------------*/
