/**
 ******************************************************************************
 *@file               :   st7789_driver.h
 *@brief              :   Portable ST7789 display-controller contract.
 *@version            :   V3.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef ST7789_DRIVER_H
#define ST7789_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef struct FRONT_DEF_T front_def_t;

typedef enum ST7789_STATE_T
{
    ST7789_OK = 0U,
    ST7789_ERROR,
    ST7789_BUSY,
    ST7789_TIMEOUT,
    ST7789_INVALID_PARAM,
    ST7789_NOT_INITIALIZED,
} st7789_state_t;

/**
 * @brief Asynchronous ST7789 transfer completion callback.
 * @note Called from the transport's interrupt context and must not block.
 */
typedef void (*st7789_async_complete_cb_t)(st7789_state_t status,
                                           void          *p_context);

typedef struct ST7789_TRANSPORT_T
{
    st7789_state_t (*pf_write)(const uint8_t *p_data,
                               uint32_t       len,
                               uint32_t       timeout_ms);
    st7789_state_t (*pf_write_dma_blocking)(const uint8_t *p_data,
                                            uint32_t       len,
                                            uint32_t       timeout_ms);
    st7789_state_t (*pf_write_dma_async)(
        const uint8_t             *p_data,
        uint32_t                   len,
        st7789_async_complete_cb_t callback,
        void                      *p_context);
    st7789_state_t (*pf_abort_dma)(void);
    st7789_state_t (*pf_set_command_mode)(uint8_t command_mode);
    void (*pf_delay_ms)(uint32_t delay_ms);
} st7789_transport_t;

typedef struct ST7789_DEV_T
{
    const st7789_transport_t *p_transport;
    uint8_t                  *p_work_buffer;
    uint32_t                  work_buffer_size;
    uint32_t                  transfer_timeout_ms;
    uint16_t                  width;
    uint16_t                  height;
    uint8_t                   is_init;
    volatile uint8_t          async_busy;
    st7789_async_complete_cb_t async_callback;
    void                      *p_async_context;
} st7789_dev_t;

/**
 * @brief Initialize an ST7789 instance using caller-owned storage.
 * @param p_dev Driver instance owned by the caller.
 * @param p_transport Immutable board transport adapter.
 * @param width Active panel width in pixels.
 * @param height Active panel height in pixels.
 * @param p_work_buffer DMA-capable row staging buffer owned by the caller.
 * @param work_buffer_size Staging-buffer capacity; at least width * 2 bytes.
 * @param transfer_timeout_ms Finite timeout for each transport transaction.
 * @retval ST7789_OK on success; otherwise an ST7789 status.
 */
st7789_state_t st7789_init(st7789_dev_t             *p_dev,
                           const st7789_transport_t *p_transport,
                           uint16_t                  width,
                           uint16_t                  height,
                           uint8_t                  *p_work_buffer,
                           uint32_t                  work_buffer_size,
                           uint32_t                  transfer_timeout_ms);

st7789_state_t st7789_fill_screen(st7789_dev_t *p_dev, uint16_t color);
st7789_state_t st7789_fill_rect(st7789_dev_t *p_dev,
                                uint16_t      x,
                                uint16_t      y,
                                uint16_t      w,
                                uint16_t      h,
                                uint16_t      color);
st7789_state_t st7789_draw_line(st7789_dev_t *p_dev,
                                uint16_t      x0,
                                uint16_t      y0,
                                uint16_t      x1,
                                uint16_t      y1,
                                uint16_t      color);
st7789_state_t st7789_draw_char(st7789_dev_t      *p_dev,
                                const front_def_t *p_font,
                                uint16_t           x,
                                uint16_t           y,
                                char               ch,
                                uint16_t           f_color,
                                uint16_t           b_color);
st7789_state_t st7789_draw_string(st7789_dev_t      *p_dev,
                                  const front_def_t *p_font,
                                  uint16_t           x,
                                  uint16_t           y,
                                  const char        *p_str,
                                  uint16_t           f_color,
                                  uint16_t           b_color);
st7789_state_t st7789_draw_image(st7789_dev_t  *p_dev,
                                 uint16_t       x,
                                 uint16_t       y,
                                 uint16_t       w,
                                 uint16_t       h,
                                 const uint8_t *p_pixels);

/**
 * @brief Start a non-blocking RGB565 big-endian image transfer.
 * @param p_dev Initialized ST7789 instance.
 * @param x Destination x coordinate.
 * @param y Destination y coordinate.
 * @param w Image width in pixels.
 * @param h Image height in pixels.
 * @param p_pixels Contiguous DMA-accessible w*h*2-byte source buffer.
 * @param callback Optional completion/error callback invoked from ISR context.
 * @param p_context Opaque value forwarded to callback.
 * @note p_pixels must remain valid and unchanged until callback execution or
 *       st7789_abort_async() returns.
 */
st7789_state_t
st7789_draw_image_async(st7789_dev_t              *p_dev,
                        uint16_t                   x,
                        uint16_t                   y,
                        uint16_t                   w,
                        uint16_t                   h,
                        const uint8_t             *p_pixels,
                        st7789_async_complete_cb_t callback,
                        void                      *p_context);

/**
 * @brief Abort the active asynchronous image transfer.
 * @note No completion callback is made for an explicit abort.
 */
st7789_state_t st7789_abort_async(st7789_dev_t *p_dev);
st7789_state_t st7789_draw_dec(st7789_dev_t      *p_dev,
                               const front_def_t *p_font,
                               uint16_t           x,
                               uint16_t           y,
                               int32_t            value,
                               uint16_t           f_color,
                               uint16_t           b_color);
st7789_state_t st7789_draw_hex(st7789_dev_t      *p_dev,
                               const front_def_t *p_font,
                               uint16_t           x,
                               uint16_t           y,
                               uint32_t           value,
                               uint16_t           f_color,
                               uint16_t           b_color);
st7789_state_t st7789_draw_float(st7789_dev_t      *p_dev,
                                 const front_def_t *p_font,
                                 uint16_t           x,
                                 uint16_t           y,
                                 float              value,
                                 uint8_t            decimals,
                                 uint16_t           f_color,
                                 uint16_t           b_color);
st7789_state_t st7789_deinit(st7789_dev_t *p_dev);

#ifdef __cplusplus
}
#endif

#endif /* ST7789_DRIVER_H */
