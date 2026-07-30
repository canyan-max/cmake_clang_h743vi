/**
 ******************************************************************************
 *@file               :   device_display.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>         /* stdint lib header file. */
#include "device_display.h" /* device_display lib header file. */
#include "bsp_drv_st7789.h"
#include "st_lcd_spi.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static st7789_driver_t g_lcd;
/* private  functions  ------------------------------------------------------*/

/* exported functions -------------------------------------------------------*/

platform_err_t device_display_init(void)
{
    st7789_state_t ret = st7789_driver_instruct(&g_lcd, &g_st7789_spi_ops);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_display_fill_screen(uint16_t color)
{
    st7789_state_t ret = g_lcd.pf_fill_screen(&g_lcd, color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_display_fill_rect(
    uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    st7789_state_t ret = g_lcd.pf_fill_rect(&g_lcd, x, y, w, h, color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_display_draw_line(
    uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    st7789_state_t ret = g_lcd.pf_draw_line(&g_lcd, x0, y0, x1, y1, color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_display_draw_string(const front_def_t *p_font,
                                          uint16_t           x,
                                          uint16_t           y,
                                          const char        *p_str,
                                          uint16_t           f_color,
                                          uint16_t           b_color)
{
    if(NULL == p_font || NULL == p_str)
    {
        return PLATFORM_ERR_PARAM;
    }
    st7789_state_t ret = g_lcd.pf_draw_string(&g_lcd, p_font, x, y, p_str,
                                              f_color, b_color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_display_draw_image(
    uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *p_pixels)
{
    if(NULL == p_pixels)
    {
        return PLATFORM_ERR_PARAM;
    }
    st7789_state_t ret = g_lcd.pf_draw_image(&g_lcd, x, y, w, h, p_pixels);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_display_draw_dec(const front_def_t *p_font,
                                       uint16_t           x,
                                       uint16_t           y,
                                       int32_t            value,
                                       uint16_t           f_color,
                                       uint16_t           b_color)
{
    if(NULL == p_font)
    {
        return PLATFORM_ERR_PARAM;
    }
    st7789_state_t ret = g_lcd.pf_draw_dec(&g_lcd, p_font, x, y, value, f_color,
                                           b_color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_display_draw_hex(const front_def_t *p_font,
                                       uint16_t           x,
                                       uint16_t           y,
                                       uint32_t           value,
                                       uint16_t           f_color,
                                       uint16_t           b_color)
{
    if(NULL == p_font)
    {
        return PLATFORM_ERR_PARAM;
    }
    st7789_state_t ret = g_lcd.pf_draw_hex(&g_lcd, p_font, x, y, value, f_color,
                                           b_color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_display_draw_float(const front_def_t *p_font,
                                         uint16_t           x,
                                         uint16_t           y,
                                         float              value,
                                         uint8_t            decimals,
                                         uint16_t           f_color,
                                         uint16_t           b_color)
{
    if(NULL == p_font)
    {
        return PLATFORM_ERR_PARAM;
    }
    st7789_state_t ret = g_lcd.pf_draw_float(&g_lcd, p_font, x, y, value,
                                             decimals, f_color, b_color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

/* end of  file -------------------------------------------------------------*/
