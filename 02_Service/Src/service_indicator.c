/**
 ******************************************************************************
 *@file               :   service_indicator.c
 *@brief              :   Provide the service indicator APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "service_indicator.h"

/* exported functions -------------------------------------------------------*/
platform_err_t service_indicator_init(void)
{
    platform_err_t ret = device_indicator_init(DEVICE_INDICATOR_1);
    if(PLATFORM_ERR_OK != ret)
    {
        return ret;
    }
    return device_indicator_init(DEVICE_INDICATOR_2);
}

platform_err_t service_indicator_on(device_indicator_id_t id)
{
    return device_indicator_on(id);
}

platform_err_t service_indicator_off(device_indicator_id_t id)
{
    return device_indicator_off(id);
}

platform_err_t service_indicator_blink(device_indicator_id_t id)
{
    return device_indicator_blink(id);
}

/* end of file --------------------------------------------------------------*/
