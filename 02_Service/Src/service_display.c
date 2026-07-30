/**
 ******************************************************************************
 *@file               :   service_display.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "service_display.h"
#include "device_display.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* private  functions  ------------------------------------------------------*/

/* exported functions -------------------------------------------------------*/
platform_err_t service_display_init(void)
{
    platform_err_t ret = device_display_init();
    if(PLATFORM_ERR_OK != ret)
    {
        return ret;
    }
    return device_display_fill_screen(0x0000U);
}

platform_err_t service_display_fill_screen(uint16_t color)
{
    return device_display_fill_screen(color);
}

platform_err_t service_display_show_frame(const uint8_t *p_pixels,
                                          uint16_t       x,
                                          uint16_t       y)
{
    return device_display_draw_image(x, y, 240U, 240U, p_pixels);
}

/* end of  file -------------------------------------------------------------*/
