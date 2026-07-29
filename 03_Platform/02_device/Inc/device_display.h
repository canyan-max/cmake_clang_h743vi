/**
 ******************************************************************************
 *@file               :   device_display.h
 *
 *@brief              :   Display device — owns ST7789 driver, exposes clean API.
 *                        Service layer calls this; never sees BSP/Impl types.
 *
 *@version            :   V2.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef DEVICE_DISPLAY_H
#define DEVICE_DISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "platform_error.h"

typedef struct FRONT_DEF_T front_def_t;

platform_err_t device_display_init(void);
platform_err_t device_display_fill_screen(uint16_t color);
platform_err_t device_display_fill_rect(uint16_t x, uint16_t y,
                                         uint16_t w, uint16_t h,
                                         uint16_t color);
platform_err_t device_display_draw_line(uint16_t x0, uint16_t y0,
                                         uint16_t x1, uint16_t y1,
                                         uint16_t color);
platform_err_t device_display_draw_string(const front_def_t *p_font,
                                           uint16_t x, uint16_t y,
                                           const char *p_str,
                                           uint16_t f_color, uint16_t b_color);
platform_err_t device_display_draw_image(uint16_t x, uint16_t y,
                                          uint16_t w, uint16_t h,
                                          const uint8_t *p_pixels);
platform_err_t device_display_draw_dec(const front_def_t *p_font,
                                        uint16_t x, uint16_t y,
                                        int32_t value,
                                        uint16_t f_color, uint16_t b_color);
platform_err_t device_display_draw_hex(const front_def_t *p_font,
                                        uint16_t x, uint16_t y,
                                        uint32_t value,
                                        uint16_t f_color, uint16_t b_color);
platform_err_t device_display_draw_float(const front_def_t *p_font,
                                          uint16_t x, uint16_t y,
                                          float value, uint8_t decimals,
                                          uint16_t f_color, uint16_t b_color);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_DISPLAY_H */
