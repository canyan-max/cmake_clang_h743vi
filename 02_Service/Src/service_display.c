/**
 ******************************************************************************
 *@file               :   service_display.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>          /* stdint lib header file. */
#include "service_display.h" /* service_display lib header file. */
#include "device_display.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* private  functions  ------------------------------------------------------*/

/* exported functions -------------------------------------------------------*/
void service_display_init(void)
{
    device_display_init();
    device_display_fill_screen(0x0000U);
}

void service_display_fill_screen(uint16_t color)
{
    device_display_fill_screen(color);
}

void service_display_show_frame(
    const uint8_t *p_pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    device_display_draw_image(x, y, w, h, p_pixels);
}

/* end of  file -------------------------------------------------------------*/
