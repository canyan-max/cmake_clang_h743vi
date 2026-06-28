/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   draw_adapter.h
 *
 *@pardependencies    :
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of description.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef DRAW_ADAPTER_H
#define DRAW_ADAPTER_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stddef lib header file. */

/* forward declarations -----------------------------------------------------*/
typedef struct FRONT_DEF_T front_def_t;

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
uint8_t drv_draw_init(void *p_drv, void *drv_ops);
uint8_t drv_draw_clear_screen(void *p_drv);
uint8_t drv_draw_fill_screen(void *p_drv, uint16_t color);
uint8_t drv_draw_fill_rect(void     *p_drv,
                           uint16_t x,
                           uint16_t y,
                           uint16_t w,
                           uint16_t h,
                           uint16_t color);
uint8_t drv_draw_line(void     *p_drv,
                      uint16_t x0,
                      uint16_t y0,
                      uint16_t x1,
                      uint16_t y1,
                      uint16_t color);
uint8_t drv_draw_string(void              *p_drv,
                        const front_def_t *p_font,
                        uint16_t           x,
                        uint16_t           y,
                        const char        *p_str,
                        uint16_t           f_color,
                        uint16_t           b_color);
uint8_t drv_draw_image(void          *p_drv,
                       uint16_t       x,
                       uint16_t       y,
                       uint16_t       w,
                       uint16_t       h,
                       const uint8_t *p_pixels);
uint8_t drv_draw_dec(void              *p_drv,
                     const front_def_t *p_font,
                     uint16_t           x,
                     uint16_t           y,
                     int32_t            value,
                     uint16_t           f_color,
                     uint16_t           b_color);
uint8_t drv_draw_hex(void              *p_drv,
                     const front_def_t *p_font,
                     uint16_t           x,
                     uint16_t           y,
                     uint32_t           value,
                     uint16_t           f_color,
                     uint16_t           b_color);
uint8_t drv_draw_float(void              *p_drv,
                       const front_def_t *p_font,
                       uint16_t           x,
                       uint16_t           y,
                       float              value,
                       uint8_t            decimals,
                       uint16_t           f_color,
                       uint16_t           b_color);
#endif /* DRAW_ADAPTER_H */
