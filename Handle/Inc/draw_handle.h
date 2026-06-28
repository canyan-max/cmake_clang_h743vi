/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   draw_handle.h
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
#ifndef DRAW_HANDLE_H
#define DRAW_HANDLE_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stddef lib header file. */

/* forward declarations -----------------------------------------------------*/
typedef struct FRONT_DEF_T front_def_t;

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t (*pf_drv_init)(void *p_drv, void *drv_ops);
    uint8_t (*pf_fill_screen)(void *p_drv, uint16_t color);
    uint8_t (*pf_fill_rect)(void    *p_drv,
                            uint16_t x,
                            uint16_t y,
                            uint16_t w,
                            uint16_t h,
                            uint16_t color);
    uint8_t (*pf_draw_line)(void    *p_drv,
                            uint16_t x0,
                            uint16_t y0,
                            uint16_t x1,
                            uint16_t y1,
                            uint16_t color);
    uint8_t (*pf_draw_string)(void              *p_drv,
                              const front_def_t *p_font,
                              uint16_t           x,
                              uint16_t           y,
                              const char        *p_str,
                              uint16_t           f_color,
                              uint16_t           b_color);
    uint8_t (*pf_draw_image)(void          *p_drv,
                             uint16_t       x,
                             uint16_t       y,
                             uint16_t       w,
                             uint16_t       h,
                             const uint8_t *p_pixels);
    uint8_t (*pf_draw_dec)(void              *p_drv,
                           const front_def_t *p_font,
                           uint16_t           x,
                           uint16_t           y,
                           int32_t            value,
                           uint16_t           f_color,
                           uint16_t           b_color);
    uint8_t (*pf_draw_hex)(void              *p_drv,
                           const front_def_t *p_font,
                           uint16_t           x,
                           uint16_t           y,
                           uint32_t           value,
                           uint16_t           f_color,
                           uint16_t           b_color);
    uint8_t (*pf_draw_float)(void              *p_drv,
                             const front_def_t *p_font,
                             uint16_t           x,
                             uint16_t           y,
                             float              value,
                             uint8_t            decimals,
                             uint16_t           f_color,
                             uint16_t           b_color);
} draw_handle_ops_t;

typedef struct
{
    const draw_handle_ops_t *p_ops;
    void                    *p_drv;
} draw_handle_t;
/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
uint8_t draw_handle_fill_screen(draw_handle_t *p_handle, uint16_t color);
uint8_t draw_handle_fill_rect(draw_handle_t *p_handle,
                              uint16_t       x,
                              uint16_t       y,
                              uint16_t       w,
                              uint16_t       h,
                              uint16_t       color);
uint8_t draw_handle_draw_line(draw_handle_t *p_handle,
                              uint16_t       x0,
                              uint16_t       y0,
                              uint16_t       x1,
                              uint16_t       y1,
                              uint16_t       color);
uint8_t draw_handle_draw_string(draw_handle_t     *p_handle,
                                const front_def_t *p_font,
                                uint16_t           x,
                                uint16_t           y,
                                const char        *p_str,
                                uint16_t           f_color,
                                uint16_t           b_color);
uint8_t draw_handle_draw_image(draw_handle_t *p_handle,
                               uint16_t       x,
                               uint16_t       y,
                               uint16_t       w,
                               uint16_t       h,
                               const uint8_t *p_pixels);
uint8_t draw_handle_draw_dec(draw_handle_t     *p_handle,
                             const front_def_t *p_font,
                             uint16_t           x,
                             uint16_t           y,
                             int32_t            value,
                             uint16_t           f_color,
                             uint16_t           b_color);
uint8_t draw_handle_draw_hex(draw_handle_t     *p_handle,
                             const front_def_t *p_font,
                             uint16_t           x,
                             uint16_t           y,
                             uint32_t           value,
                             uint16_t           f_color,
                             uint16_t           b_color);
uint8_t draw_handle_draw_float(draw_handle_t     *p_handle,
                               const front_def_t *p_font,
                               uint16_t           x,
                               uint16_t           y,
                               float              value,
                               uint8_t            decimals,
                               uint16_t           f_color,
                               uint16_t           b_color);
uint8_t draw_handle_instruct(draw_handle_t           *p_handle,
                             const draw_handle_ops_t *p_ops,
                             void                    *p_drv,
                             void                    *p_drv_ops);
#endif /* DRAW_HANDLE_H */
