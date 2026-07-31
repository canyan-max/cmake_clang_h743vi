/**
 ******************************************************************************
 *@file               :   bsp_st7789_intf.c
 *@brief              :   ST7789 implementation of display_intf.
 *                        Swap this file when replacing display chip.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "bsp_st7789.h"
#include "display_intf.h"

/* variables ----------------------------------------------------------------*/
static st7789_dev_t g_lcd;

/* Private functions --------------------------------------------------------*/
static platform_err_t display_bsp_init(void)
{
    st7789_state_t ret = bsp_st7789_init(&g_lcd);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t display_bsp_fill_screen(uint16_t color)
{
    st7789_state_t ret = st7789_fill_screen(&g_lcd, color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t display_bsp_fill_rect(uint16_t x, uint16_t y,
                                             uint16_t w, uint16_t h,
                                             uint16_t color)
{
    st7789_state_t ret = st7789_fill_rect(&g_lcd, x, y, w, h, color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t display_bsp_draw_line(uint16_t x0, uint16_t y0,
                                             uint16_t x1, uint16_t y1,
                                             uint16_t color)
{
    st7789_state_t ret = st7789_draw_line(&g_lcd, x0, y0, x1, y1, color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t display_bsp_draw_string(const front_def_t *p_font,
                                               uint16_t x, uint16_t y,
                                               const char *p_str,
                                               uint16_t f_color,
                                               uint16_t b_color)
{
    st7789_state_t ret = st7789_draw_string(&g_lcd, p_font, x, y, p_str,
                                             f_color, b_color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t display_bsp_draw_image(uint16_t x, uint16_t y,
                                              uint16_t w, uint16_t h,
                                              const uint8_t *p_pixels)
{
    st7789_state_t ret = st7789_draw_image(&g_lcd, x, y, w, h, p_pixels);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t display_bsp_draw_dec(const front_def_t *p_font,
                                            uint16_t x, uint16_t y,
                                            int32_t value,
                                            uint16_t f_color, uint16_t b_color)
{
    st7789_state_t ret = st7789_draw_dec(&g_lcd, p_font, x, y, value,
                                          f_color, b_color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t display_bsp_draw_hex(const front_def_t *p_font,
                                            uint16_t x, uint16_t y,
                                            uint32_t value,
                                            uint16_t f_color, uint16_t b_color)
{
    st7789_state_t ret = st7789_draw_hex(&g_lcd, p_font, x, y, value,
                                          f_color, b_color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t display_bsp_draw_float(const front_def_t *p_font,
                                              uint16_t x, uint16_t y,
                                              float value, uint8_t decimals,
                                              uint16_t f_color, uint16_t b_color)
{
    st7789_state_t ret = st7789_draw_float(&g_lcd, p_font, x, y, value,
                                            decimals, f_color, b_color);
    return (ST7789_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

/* Exported variables -------------------------------------------------------*/
const bsp_display_ops_t g_display_bsp_ops = {
    .pf_init        = display_bsp_init,
    .pf_fill_screen = display_bsp_fill_screen,
    .pf_fill_rect   = display_bsp_fill_rect,
    .pf_draw_line   = display_bsp_draw_line,
    .pf_draw_string = display_bsp_draw_string,
    .pf_draw_image  = display_bsp_draw_image,
    .pf_draw_dec    = display_bsp_draw_dec,
    .pf_draw_hex    = display_bsp_draw_hex,
    .pf_draw_float  = display_bsp_draw_float,
};

/* end of file --------------------------------------------------------------*/
