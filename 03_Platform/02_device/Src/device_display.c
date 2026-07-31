/**
 ******************************************************************************
 *@file               :   device_display.c
 *@brief              :   Provide the device of display
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "device_display.h"
#include "display_intf.h"

/* exported functions -------------------------------------------------------*/

platform_err_t device_display_init(void)
{
    return g_display_bsp_ops.pf_init();
}

platform_err_t device_display_fill_screen(uint16_t color)
{
    return g_display_bsp_ops.pf_fill_screen(color);
}

platform_err_t device_display_fill_rect(
    uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    return g_display_bsp_ops.pf_fill_rect(x, y, w, h, color);
}

platform_err_t device_display_draw_line(
    uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    return g_display_bsp_ops.pf_draw_line(x0, y0, x1, y1, color);
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
    return g_display_bsp_ops.pf_draw_string(p_font, x, y, p_str,
                                             f_color, b_color);
}

platform_err_t device_display_draw_image(
    uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *p_pixels)
{
    if(NULL == p_pixels)
    {
        return PLATFORM_ERR_PARAM;
    }
    return g_display_bsp_ops.pf_draw_image(x, y, w, h, p_pixels);
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
    return g_display_bsp_ops.pf_draw_dec(p_font, x, y, value, f_color, b_color);
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
    return g_display_bsp_ops.pf_draw_hex(p_font, x, y, value, f_color, b_color);
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
    return g_display_bsp_ops.pf_draw_float(p_font, x, y, value, decimals,
                                            f_color, b_color);
}

/* end of  file -------------------------------------------------------------*/
