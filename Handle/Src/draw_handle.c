/**
 ******************************************************************************
 *@file               :   draw_handle.c
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 * 
 *@pardependencies    :   draw_handle.c
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>      /* stdint lib header file. */
#include "draw_handle.h" /* draw_handle lib header file. */

/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
/**
  * @brief            :  [draw_handle_fill_screen]
  * @retval           :  [0 successful,
                          1 invalid parameter]
  * @param[in]        :  [draw_handle_t *p_handle, color]
  */
uint8_t draw_handle_fill_screen(draw_handle_t *p_handle, uint16_t color)
{
    if(NULL == p_handle)
    {
        return 1;
    }

    return p_handle->p_ops->pf_fill_screen(p_handle->p_drv, color);
}
/**
 * @brief            :  [draw_handle_fill_rect]
 * @retval           :  [0 successful,
                         1 invalid parameter]
 * @param[in]        :  [draw_handle_t *p_handle, x, y, w, h, color]
 */
uint8_t draw_handle_fill_rect(draw_handle_t *p_handle,
                              uint16_t       x,
                              uint16_t       y,
                              uint16_t       w,
                              uint16_t       h,
                              uint16_t       color)
{
    if(NULL == p_handle)
    {
        return 1;
    }

    return p_handle->p_ops->pf_fill_rect(p_handle->p_drv, x, y, w, h, color);
}
/**
 * @brief            :  [draw_handle_draw_line]
 * @retval           :  [0 successful,
                         1 invalid parameter]
 * @param[in]        :  [draw_handle_t *p_handle, x0, y0, x1, y1, color]
 */
uint8_t draw_handle_draw_line(draw_handle_t *p_handle,
                              uint16_t       x0,
                              uint16_t       y0,
                              uint16_t       x1,
                              uint16_t       y1,
                              uint16_t       color)
{
    if(NULL == p_handle)
    {
        return 1;
    }

    return p_handle->p_ops->pf_draw_line(p_handle->p_drv, x0, y0, x1, y1,
                                         color);
}
/**
 * @brief            :  [draw_handle_draw_string]
 * @retval           :  [0 successful,
                         1 invalid parameter]
 * @param[in]        :  [draw_handle_t *p_handle, p_font, x, y, p_str, f_color,
 b_color]
 */
uint8_t draw_handle_draw_string(draw_handle_t     *p_handle,
                                const front_def_t *p_font,
                                uint16_t           x,
                                uint16_t           y,
                                const char        *p_str,
                                uint16_t           f_color,
                                uint16_t           b_color)
{
    if(NULL == p_handle || NULL == p_font || NULL == p_str)
    {
        return 1;
    }

    return p_handle->p_ops->pf_draw_string(p_handle->p_drv, p_font, x, y, p_str,
                                           f_color, b_color);
}
/**
  * @brief            :  [draw_handle_draw_image]
  * @retval           :  [0 successful,
                          1 invalid parameter]
  * @param[in]        :  [draw_handle_t *p_handle, x, y, w, h, p_pixels]
  */
uint8_t draw_handle_draw_image(draw_handle_t *p_handle,
                               uint16_t       x,
                               uint16_t       y,
                               uint16_t       w,
                               uint16_t       h,
                               const uint8_t *p_pixels)
{
    if(NULL == p_handle || NULL == p_pixels)
    {
        return 1;
    }

    return p_handle->p_ops->pf_draw_image(p_handle->p_drv, x, y, w, h,
                                          p_pixels);
}
/**
  * @brief            :  [draw_handle_draw_dec]
  * @retval           :  [0 successful,
                          1 invalid parameter]
  * @param[in]        :  [draw_handle_t *p_handle, p_font, x, y, value, f_color,
                          b_color]
  */
uint8_t draw_handle_draw_dec(draw_handle_t     *p_handle,
                             const front_def_t *p_font,
                             uint16_t           x,
                             uint16_t           y,
                             int32_t            value,
                             uint16_t           f_color,
                             uint16_t           b_color)
{
    if(NULL == p_handle || NULL == p_font)
    {
        return 1;
    }

    return p_handle->p_ops->pf_draw_dec(p_handle->p_drv, p_font, x, y, value,
                                        f_color, b_color);
}
/**
  * @brief            :  [draw_handle_draw_hex]
  * @retval           :  [0 successful,
                          1 invalid parameter]
  * @param[in]        :  [draw_handle_t *p_handle, p_font, x, y, value, f_color,
  b_color]
  */
uint8_t draw_handle_draw_hex(draw_handle_t     *p_handle,
                             const front_def_t *p_font,
                             uint16_t           x,
                             uint16_t           y,
                             uint32_t           value,
                             uint16_t           f_color,
                             uint16_t           b_color)
{
    if(NULL == p_handle || NULL == p_font)
    {
        return 1;
    }

    return p_handle->p_ops->pf_draw_hex(p_handle->p_drv, p_font, x, y, value,
                                        f_color, b_color);
}
/**
  * @brief            :  [draw_handle_draw_float]
  * @retval           :  [0 successful,
                          1 invalid parameter]
  * @param[in]        :  [draw_handle_t *p_handle, p_font, x, y, value,
  decimals, f_color, b_color]
  */
uint8_t draw_handle_draw_float(draw_handle_t     *p_handle,
                               const front_def_t *p_font,
                               uint16_t           x,
                               uint16_t           y,
                               float              value,
                               uint8_t            decimals,
                               uint16_t           f_color,
                               uint16_t           b_color)
{
    if(NULL == p_handle || NULL == p_font)
    {
        return 1;
    }

    return p_handle->p_ops->pf_draw_float(p_handle->p_drv, p_font, x, y, value,
                                          decimals, f_color, b_color);
}
/**
 * @brief            :  [draw_handle_instruct]
 * @retval           :  [0 successful,
                         1 invalid parameter]
 * @param[in]        :  [draw_handle_t *p_handle, \
                                const draw_handle_ops_t *p_ops, \
                                void *p_drv, \
                                void *p_drv_ops]
 */
uint8_t draw_handle_instruct(draw_handle_t           *p_handle,
                             const draw_handle_ops_t *p_ops,
                             void                    *p_drv,
                             void                    *p_drv_ops)
{
    if(NULL == p_handle || NULL == p_drv || NULL == p_ops || NULL == p_drv_ops)
    {
        return 1;
    }

    p_handle->p_ops = p_ops;
    p_handle->p_drv = p_drv;

    return p_handle->p_ops->pf_drv_init(p_handle->p_drv, p_drv_ops);
}
/* end of  file -------------------------------------------------------------*/
