/**
 ******************************************************************************
 *@file               :   display_intf.h
 *@brief              :   Display BSP interface contract consumed by device layer.
 *                        Swap bsp_st7789_intf.c to replace display chip.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef DISPLAY_INTF_H
#define DISPLAY_INTF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "plat_error.h"

/* ---- forward declarations ------------------------------------------------ */
typedef struct FRONT_DEF_T front_def_t;

/* ---- defines ------------------------------------------------------------- */
#define BSP_DISPLAY_WIDTH   (240U)
#define BSP_DISPLAY_HEIGHT  (240U)

/* ---- ops struct ---------------------------------------------------------- */
typedef struct BSP_DISPLAY_OPS_T
{
    platform_err_t (*pf_init)(void);
    platform_err_t (*pf_fill_screen)(uint16_t color);
    platform_err_t (*pf_fill_rect)(uint16_t x, uint16_t y,
                                   uint16_t w, uint16_t h, uint16_t color);
    platform_err_t (*pf_draw_line)(uint16_t x0, uint16_t y0,
                                   uint16_t x1, uint16_t y1, uint16_t color);
    platform_err_t (*pf_draw_string)(const front_def_t *p_font,
                                     uint16_t x, uint16_t y, const char *p_str,
                                     uint16_t f_color, uint16_t b_color);
    platform_err_t (*pf_draw_image)(uint16_t x, uint16_t y,
                                    uint16_t w, uint16_t h,
                                    const uint8_t *p_pixels);
    platform_err_t (*pf_draw_dec)(const front_def_t *p_font,
                                  uint16_t x, uint16_t y, int32_t value,
                                  uint16_t f_color, uint16_t b_color);
    platform_err_t (*pf_draw_hex)(const front_def_t *p_font,
                                  uint16_t x, uint16_t y, uint32_t value,
                                  uint16_t f_color, uint16_t b_color);
    platform_err_t (*pf_draw_float)(const front_def_t *p_font,
                                    uint16_t x, uint16_t y,
                                    float value, uint8_t decimals,
                                    uint16_t f_color, uint16_t b_color);
} bsp_display_ops_t;

extern const bsp_display_ops_t g_display_bsp_ops;

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_INTF_H */
