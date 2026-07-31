/**
 ******************************************************************************
 *@file               :   device_indicator.c
 *@brief              :   Provide the device indicator APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "device_indicator.h"
#include "bsp_led.h"

/* exported functions -------------------------------------------------------*/

platform_err_t device_indicator_init(device_indicator_id_t id)
{
    if(id >= DEVICE_INDICATOR_NUM) { return PLATFORM_ERR_PARAM; }
    return PLATFORM_ERR_OK;
}

platform_err_t device_indicator_on(device_indicator_id_t id)
{
    if(id >= DEVICE_INDICATOR_NUM) { return PLATFORM_ERR_PARAM; }
    bsp_led_on((uint8_t)id);
    return PLATFORM_ERR_OK;
}

platform_err_t device_indicator_off(device_indicator_id_t id)
{
    if(id >= DEVICE_INDICATOR_NUM) { return PLATFORM_ERR_PARAM; }
    bsp_led_off((uint8_t)id);
    return PLATFORM_ERR_OK;
}

platform_err_t device_indicator_blink(device_indicator_id_t id)
{
    if(id >= DEVICE_INDICATOR_NUM) { return PLATFORM_ERR_PARAM; }
    bsp_led_toggle((uint8_t)id);
    return PLATFORM_ERR_OK;
}

/* end of  file -------------------------------------------------------------*/
