/**
 ******************************************************************************
 *@file               :   bsp_display.c
 *@brief              :   Assemble the fitted ST7789 panel and board transport.
 *@version            :   V3.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

#include <stddef.h>
#include "bsp_display.h"
#include "st7789_driver.h"
#include "plat_gpio.h"
#include "plat_spi.h"
#include "plat_sys.h"
#include "board_resources.h"

#define BSP_ST7789_TRANSFER_TIMEOUT_MS  (100U)
#define BSP_ST7789_WORK_BUFFER_SIZE     (BOARD_DISPLAY_WIDTH * 2U)

static st7789_dev_t s_display;
__attribute__((section(".ram_dma_buffers"), aligned(32))) static uint8_t
    s_display_work_buffer[BSP_ST7789_WORK_BUFFER_SIZE];
static st7789_async_complete_cb_t volatile s_transport_async_callback;
static void * volatile s_transport_async_context;
static bsp_display_async_complete_cb_t volatile s_display_async_callback;
static const bsp_display_info_t s_display_info = {
    .canvas_width  = BOARD_DISPLAY_WIDTH,
    .canvas_height = BOARD_DISPLAY_HEIGHT,
    .input_format  = BSP_DISPLAY_PIXEL_FORMAT_RGB565,
};

static st7789_state_t bsp_st7789_convert_platform_error(platform_err_t error)
{
    switch(error)
    {
        case PLATFORM_ERR_OK:
            return ST7789_OK;
        case PLATFORM_ERR_PARAM:
            return ST7789_INVALID_PARAM;
        case PLATFORM_ERR_BUSY:
            return ST7789_BUSY;
        case PLATFORM_ERR_TIMEOUT:
            return ST7789_TIMEOUT;
        case PLATFORM_ERR_HW:
        default:
            return ST7789_ERROR;
    }
}

static platform_err_t bsp_st7789_convert_driver_status(st7789_state_t status)
{
    switch(status)
    {
        case ST7789_OK:
            return PLATFORM_ERR_OK;
        case ST7789_INVALID_PARAM:
            return PLATFORM_ERR_PARAM;
        case ST7789_BUSY:
            return PLATFORM_ERR_BUSY;
        case ST7789_TIMEOUT:
            return PLATFORM_ERR_TIMEOUT;
        case ST7789_ERROR:
        case ST7789_NOT_INITIALIZED:
        default:
            return PLATFORM_ERR_HW;
    }
}

static st7789_state_t
bsp_st7789_write(const uint8_t *p_data, uint32_t len, uint32_t timeout_ms)
{
    return bsp_st7789_convert_platform_error(
        plat_spi_write(PLAT_SPI_ID_0, p_data, len, timeout_ms));
}

static st7789_state_t bsp_st7789_write_dma_blocking(const uint8_t *p_data,
                                                    uint32_t       len,
                                                    uint32_t       timeout_ms)
{
    return bsp_st7789_convert_platform_error(
        plat_spi_write_dma_blocking(PLAT_SPI_ID_0, p_data, len, timeout_ms));
}

static void bsp_st7789_dma_complete_from_isr(plat_spi_id_t  id,
                                             platform_err_t status,
                                             void          *p_context)
{
    ((void)id);
    ((void)p_context);
    st7789_async_complete_cb_t callback = s_transport_async_callback;
    void *p_callback_context = s_transport_async_context;
    s_transport_async_callback = NULL;
    s_transport_async_context  = NULL;

    if(NULL != callback)
    {
        callback(bsp_st7789_convert_platform_error(status),
                 p_callback_context);
    }
}

static st7789_state_t bsp_st7789_write_dma_async(
    const uint8_t             *p_data,
    uint32_t                   len,
    st7789_async_complete_cb_t callback,
    void                      *p_context)
{
    if((NULL == callback) || (NULL != s_transport_async_callback))
    {
        return (NULL == callback) ? ST7789_INVALID_PARAM : ST7789_BUSY;
    }

    s_transport_async_callback = callback;
    s_transport_async_context  = p_context;
    platform_err_t status = plat_spi_write_dma_async(
        PLAT_SPI_ID_0, p_data, len, bsp_st7789_dma_complete_from_isr, NULL);
    if(PLATFORM_ERR_OK != status)
    {
        s_transport_async_callback = NULL;
        s_transport_async_context  = NULL;
    }
    return bsp_st7789_convert_platform_error(status);
}

static st7789_state_t bsp_st7789_abort_dma(void)
{
    platform_err_t status = plat_spi_abort_dma(PLAT_SPI_ID_0);
    s_transport_async_callback = NULL;
    s_transport_async_context  = NULL;
    return bsp_st7789_convert_platform_error(status);
}

static st7789_state_t bsp_st7789_set_command_mode(uint8_t command_mode)
{
    uint8_t level = (0U != command_mode) ? BOARD_DISPLAY_DC_COMMAND_LEVEL
                                         : BOARD_DISPLAY_DC_DATA_LEVEL;
    return bsp_st7789_convert_platform_error(
        plat_gpio_write(BOARD_GPIO_DISPLAY_DC, level));
}

static void bsp_st7789_delay_ms(uint32_t delay_ms)
{
    plat_delay_ms(delay_ms);
}

static const st7789_transport_t s_display_transport = {
    .pf_write              = bsp_st7789_write,
    .pf_write_dma_blocking = bsp_st7789_write_dma_blocking,
    .pf_write_dma_async    = bsp_st7789_write_dma_async,
    .pf_abort_dma          = bsp_st7789_abort_dma,
    .pf_set_command_mode   = bsp_st7789_set_command_mode,
    .pf_delay_ms           = bsp_st7789_delay_ms,
};

static void bsp_display_async_complete_from_isr(st7789_state_t status,
                                                void          *p_context)
{
    ((void)p_context);
    bsp_display_async_complete_cb_t callback = s_display_async_callback;
    s_display_async_callback = NULL;
    if(NULL != callback)
    {
        callback(bsp_st7789_convert_driver_status(status));
    }
}

platform_err_t bsp_display_get_info(bsp_display_info_t *p_info)
{
    if(NULL == p_info)
    {
        return PLATFORM_ERR_PARAM;
    }

    *p_info = s_display_info;
    return PLATFORM_ERR_OK;
}

platform_err_t bsp_display_init(void)
{
    platform_err_t error = plat_gpio_write(BOARD_GPIO_DISPLAY_BACKLIGHT,
                                           BOARD_DISPLAY_BACKLIGHT_OFF_LEVEL);
    if(PLATFORM_ERR_OK != error)
    {
        return error;
    }

    st7789_state_t status = st7789_init(&s_display, &s_display_transport,
                                        BOARD_DISPLAY_WIDTH,
                                        BOARD_DISPLAY_HEIGHT,
                                        s_display_work_buffer,
                                        sizeof(s_display_work_buffer),
                                        BSP_ST7789_TRANSFER_TIMEOUT_MS);
    if(ST7789_OK != status)
    {
        return bsp_st7789_convert_driver_status(status);
    }

    return plat_gpio_write(BOARD_GPIO_DISPLAY_BACKLIGHT,
                           BOARD_DISPLAY_BACKLIGHT_ON_LEVEL);
}

platform_err_t bsp_display_fill_screen(uint16_t color)
{
    return bsp_st7789_convert_driver_status(
        st7789_fill_screen(&s_display, color));
}

platform_err_t bsp_display_fill_rect(
    uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    return bsp_st7789_convert_driver_status(
        st7789_fill_rect(&s_display, x, y, w, h, color));
}

platform_err_t bsp_display_draw_line(
    uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    return bsp_st7789_convert_driver_status(
        st7789_draw_line(&s_display, x0, y0, x1, y1, color));
}

platform_err_t bsp_display_draw_string(const front_def_t *p_font,
                                       uint16_t           x,
                                       uint16_t           y,
                                       const char        *p_str,
                                       uint16_t           f_color,
                                       uint16_t           b_color)
{
    return bsp_st7789_convert_driver_status(
        st7789_draw_string(&s_display, p_font, x, y, p_str, f_color, b_color));
}

platform_err_t bsp_display_draw_image(
    uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *p_pixels)
{
    return bsp_st7789_convert_driver_status(
        st7789_draw_image(&s_display, x, y, w, h, p_pixels));
}

platform_err_t bsp_display_draw_image_async(
    uint16_t                        x,
    uint16_t                        y,
    uint16_t                        w,
    uint16_t                        h,
    const uint8_t                  *p_pixels,
    bsp_display_async_complete_cb_t callback)
{
    if(NULL == callback)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(NULL != s_display_async_callback)
    {
        return PLATFORM_ERR_BUSY;
    }

    s_display_async_callback = callback;
    st7789_state_t status = st7789_draw_image_async(
        &s_display, x, y, w, h, p_pixels, bsp_display_async_complete_from_isr,
        NULL);
    if(ST7789_OK != status)
    {
        s_display_async_callback = NULL;
    }
    return bsp_st7789_convert_driver_status(status);
}

platform_err_t bsp_display_abort_async(void)
{
    st7789_state_t status = st7789_abort_async(&s_display);
    s_display_async_callback = NULL;
    return bsp_st7789_convert_driver_status(status);
}

platform_err_t bsp_display_draw_dec(const front_def_t *p_font,
                                    uint16_t           x,
                                    uint16_t           y,
                                    int32_t            value,
                                    uint16_t           f_color,
                                    uint16_t           b_color)
{
    return bsp_st7789_convert_driver_status(
        st7789_draw_dec(&s_display, p_font, x, y, value, f_color, b_color));
}

platform_err_t bsp_display_draw_hex(const front_def_t *p_font,
                                    uint16_t           x,
                                    uint16_t           y,
                                    uint32_t           value,
                                    uint16_t           f_color,
                                    uint16_t           b_color)
{
    return bsp_st7789_convert_driver_status(
        st7789_draw_hex(&s_display, p_font, x, y, value, f_color, b_color));
}

platform_err_t bsp_display_draw_float(const front_def_t *p_font,
                                      uint16_t           x,
                                      uint16_t           y,
                                      float              value,
                                      uint8_t            decimals,
                                      uint16_t           f_color,
                                      uint16_t           b_color)
{
    return bsp_st7789_convert_driver_status(
        st7789_draw_float(&s_display, p_font, x, y, value, decimals, f_color,
                          b_color));
}

/* end of file --------------------------------------------------------------*/
