/**
 ******************************************************************************
 *@file               :   draw_adapter.h
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
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
/**
 * @brief            :  [drv_draw_init
                        drv init the draw operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, drv_ops]
 */
uint8_t drv_draw_init(void *p_drv, void *drv_ops);
/**
 * @brief            :  [drv_draw_fill_screen
                        drv fill screen operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, color]
 */
uint8_t drv_draw_fill_screen(void *p_drv, uint16_t color);
/**
 * @brief            :  [drv_draw_fill_rect
                        drv fill rectangle operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, x, y, w, h, color]
 */
uint8_t drv_draw_fill_rect(void     *p_drv,
                           uint16_t x,
                           uint16_t y,
                           uint16_t w,
                           uint16_t h,
                           uint16_t color);
/**
 * @brief            :  [drv_draw_line
                        drv draw line operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, x0, y0, x1, y1, color]
 */
uint8_t drv_draw_line(void     *p_drv,
                      uint16_t x0,
                      uint16_t y0,
                      uint16_t x1,
                      uint16_t y1,
                      uint16_t color);
/**
 * @brief            :  [drv_draw_string
                        drv draw string operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, p_font, x, y, p_str, f_color, b_color]
 */
uint8_t drv_draw_string(void              *p_drv,
                        const front_def_t *p_font,
                        uint16_t           x,
                        uint16_t           y,
                        const char        *p_str,
                        uint16_t           f_color,
                        uint16_t           b_color);
/**
 * @brief            :  [drv_draw_image
                        drv draw image operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, x, y, w, h, p_pixels]
 */
uint8_t drv_draw_image(void          *p_drv,
                       uint16_t       x,
                       uint16_t       y,
                       uint16_t       w,
                       uint16_t       h,
                       const uint8_t *p_pixels);
/**
 * @brief            :  [drv_draw_dec
                        drv draw decimal operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, p_font, x, y, value, f_color, b_color]
 */
uint8_t drv_draw_dec(void              *p_drv,
                     const front_def_t *p_font,
                     uint16_t           x,
                     uint16_t           y,
                     int32_t            value,
                     uint16_t           f_color,
                     uint16_t           b_color);
/**
 * @brief            :  [drv_draw_hex
                        drv draw hex operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, p_font, x, y, value, f_color, b_color]
 */
uint8_t drv_draw_hex(void              *p_drv,
                     const front_def_t *p_font,
                     uint16_t           x,
                     uint16_t           y,
                     uint32_t           value,
                     uint16_t           f_color,
                     uint16_t           b_color);
/**
 * @brief            :  [drv_draw_float
                        drv draw float operation ]
 * @retval           :  [0 successfully,
                         1 invalid parameter,
                         2 driver error]
 * @param[in]        :  [p_drv, p_font, x, y, value, decimals, f_color, b_color]
 */
uint8_t drv_draw_float(void              *p_drv,
                       const front_def_t *p_font,
                       uint16_t           x,
                       uint16_t           y,
                       float              value,
                       uint8_t            decimals,
                       uint16_t           f_color,
                       uint16_t           b_color);
#endif /* DRAW_ADAPTER_H */
