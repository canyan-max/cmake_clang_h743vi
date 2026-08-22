/**
 ******************************************************************************
 *@file               :   bsp_display.h
 *@brief              :   Board display capability contract.
 *@version            :   V3.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_DISPLAY_H
#define BSP_DISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "plat_error.h"

typedef struct FRONT_DEF_T front_def_t;

typedef enum BSP_DISPLAY_PIXEL_FORMAT_T
{
    BSP_DISPLAY_PIXEL_FORMAT_RGB565 = 0U,
    BSP_DISPLAY_PIXEL_FORMAT_NUM,
} bsp_display_pixel_format_t;

typedef struct BSP_DISPLAY_INFO_T
{
    uint16_t                   canvas_width;
    uint16_t                   canvas_height;
    bsp_display_pixel_format_t input_format;
} bsp_display_info_t;

/**
 * @brief Asynchronous display-operation completion callback.
 * @note Invoked from interrupt context and must not block.
 */
typedef void (*bsp_display_async_complete_cb_t)(platform_err_t status);

/**
 * @brief Return the board display's logical drawing capability.
 * @param p_info Destination for canvas dimensions and accepted pixel format.
 * @retval PLATFORM_ERR_OK on success; PLATFORM_ERR_PARAM for NULL.
 * @note This describes the public drawing contract, not the controller, bus,
 *       DMA channel, orientation register, or fitted pin wiring.
 */
platform_err_t bsp_display_get_info(bsp_display_info_t *p_info);

platform_err_t bsp_display_init(void);
platform_err_t bsp_display_fill_screen(uint16_t color);
platform_err_t bsp_display_fill_rect(
    uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
platform_err_t bsp_display_draw_line(
    uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
platform_err_t bsp_display_draw_string(const front_def_t *p_font,
                                       uint16_t           x,
                                       uint16_t           y,
                                       const char        *p_str,
                                       uint16_t           f_color,
                                       uint16_t           b_color);
platform_err_t bsp_display_draw_image(
    uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *p_pixels);

/**
 * @brief Start a non-blocking RGB565 image transfer to the board display.
 * @note p_pixels must remain valid and unchanged until callback execution or
 *       bsp_display_abort_async() returns.
 */
platform_err_t bsp_display_draw_image_async(
    uint16_t                        x,
    uint16_t                        y,
    uint16_t                        w,
    uint16_t                        h,
    const uint8_t                  *p_pixels,
    bsp_display_async_complete_cb_t callback);

/**
 * @brief Abort the active asynchronous display transfer.
 * @note No completion callback is made for an explicit abort.
 */
platform_err_t bsp_display_abort_async(void);
platform_err_t bsp_display_draw_dec(const front_def_t *p_font,
                                    uint16_t           x,
                                    uint16_t           y,
                                    int32_t            value,
                                    uint16_t           f_color,
                                    uint16_t           b_color);
platform_err_t bsp_display_draw_hex(const front_def_t *p_font,
                                    uint16_t           x,
                                    uint16_t           y,
                                    uint32_t           value,
                                    uint16_t           f_color,
                                    uint16_t           b_color);
platform_err_t bsp_display_draw_float(const front_def_t *p_font,
                                      uint16_t           x,
                                      uint16_t           y,
                                      float              value,
                                      uint8_t            decimals,
                                      uint16_t           f_color,
                                      uint16_t           b_color);

#ifdef __cplusplus
}
#endif

#endif /* BSP_DISPLAY_H */
