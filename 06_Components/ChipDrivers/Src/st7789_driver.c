/**
 ******************************************************************************
 *@file               :   st7789_driver.c
 *@brief              :   Portable ST7789 protocol and drawing implementation.
 *@version            :   V3.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include <string.h> /* string lib header file. */
#include <stddef.h> /* stddef lib header file. */
#include "st7789_driver.h"
#include "front.h" /* font table header file. */

/* define   -----------------------------------------------------------------*/
/* To avoid gcc/g++ warnings */
#define ST7789_NOT_USE_FUNCTION     __attribute__((unused))
#define ST7789_NOT_USE_VARIABLE(x)  ((void)(x))

#define ST7789_CMD_NOP              (0x00U)      // NOP command.
#define ST7789_CMD_SWRESET          (0x01U)      // Software reset.
#define ST7789_CMD_SLPIN            (0x10U)      // Sleep in.
#define ST7789_CMD_SLPOUT           (0x11U)      // Sleep out.
#define ST7789_CMD_NORON            (0x13U)      // Normal display mode on.
#define ST7789_CMD_INVOFF           (0x20U)      // Display inversion off.
#define ST7789_CMD_INVON            (0x21U)      // Display inversion on.
#define ST7789_CMD_DISPOFF          (0x28U)      // Display off.
#define ST7789_CMD_DISPON           (0x29U)      // Display on.
#define ST7789_CMD_CASET            (0x2AU)      // Column address set.
#define ST7789_CMD_RASET            (0x2BU)      // Row address set.
#define ST7789_CMD_RAMWR            (0x2CU)      // Memory write.
#define ST7789_CMD_COLMOD           (0x3AU)      // Interface pixel format.
#define ST7789_SWRESET_DELAY_MS     (150U)       // Delay after SW reset.
#define ST7789_SLPOUT_DELAY_MS      (120U)       // Delay after sleep out.
#define ST7789_DRIVER_IS_INIT       (1U)
#define ST7789_DRIVER_NOT_INIT      (0U)

#define ST7789_FONT_CHAR_OFFSET     (0x20U)      // ASCII offset: space.
#define ST7789_NUM_BUF_SIZE         (20U)        // Scratch buffer for number strings.

/* typedef ------------------------------------------------------------------*/
/* private  functions  ------------------------------------------------------*/
/**
 * @brief            :  [color24bit_to_rgb565
                         24bit color to rgb565]
 * @retval           :  [ uint16_t rgb 565]
 * @param[in]        :  [(uint32_t Color)]
 */
ST7789_NOT_USE_FUNCTION
uint16_t color24bit_to_rgb565(uint32_t color)
{
    uint16_t r = 0, g = 0, b = 0, rgb565 = 0;

    r      = (uint16_t)((color & 0x00F80000) >> 8);
    g      = (uint16_t)((color & 0x0000FC00) >> 5);
    b      = (uint16_t)((color & 0x000000F8) >> 3);
    rgb565 = (uint16_t)(r | g | b);
    return rgb565;
}

/**
  * @brief            :  [st7789_set_forward_color]
                          set forward color
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint8_t cmd]
  */
ST7789_NOT_USE_FUNCTION
static st7789_state_t st7789_set_forward_color(st7789_dev_t *p_drv,
                                               uint32_t      f_color)
{
    if(NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }
    ST7789_NOT_USE_VARIABLE(f_color);
    // p_drv->f_color = color24bit_to_rgb565(f_color);
    return ST7789_OK;
}

/**
  * @brief            :  [st7789_set_back_color]
                          set forward color
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint8_t cmd]
  */
ST7789_NOT_USE_FUNCTION
static st7789_state_t st7789_set_back_color(st7789_dev_t *p_drv,
                                            uint32_t      b_color)
{
    if(NULL == p_drv)
        return ST7789_INVALID_PARAM;
    ST7789_NOT_USE_VARIABLE(b_color);
    // p_drv->b_color = color24bit_to_rgb565(b_color);
    return ST7789_OK;
}

/**
  * @brief            :  [st7789_write_cmd]
                         DC pin -> command mode, then SPI transmit 1 byte.
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint8_t cmd]
  */
static st7789_state_t st7789_write_cmd(st7789_dev_t *p_drv, uint8_t cmd)
{
    st7789_state_t ret = ST7789_OK;

    if(0U != p_drv->async_busy)
    {
        return ST7789_BUSY;
    }

    /* DC pin = command mode */
    ret = p_drv->p_transport->pf_set_command_mode(1U);
    if(ST7789_OK != ret)
    {
        return ret;
    }
    /* SPI transmit command byte */
    ret = p_drv->p_transport->pf_write(&cmd, 1U, p_drv->transfer_timeout_ms);
    return ret;
}

/**
  * @brief            :  [st7789_write_data]
                         DC pin -> data mode, then SPI transmit 1 byte.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint8_t data]
  */
static st7789_state_t st7789_write_data(st7789_dev_t *p_drv, uint8_t data)
{
    st7789_state_t ret = ST7789_OK;

    if(0U != p_drv->async_busy)
    {
        return ST7789_BUSY;
    }

    /* DC pin = data mode */
    ret = p_drv->p_transport->pf_set_command_mode(0U);
    if(ST7789_OK != ret)
    {
        return ret;
    }
    /* SPI transmit data byte */
    ret = p_drv->p_transport->pf_write(&data, 1U, p_drv->transfer_timeout_ms);
    return ret;
}

/**
  * @brief            :  [st7789_write_buf]
                         DC pin 鈫?data mode, then SPI transmit buffer.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint8_t *p_data, uint32_t data_len]
  */
static st7789_state_t
st7789_write_buf(st7789_dev_t *p_drv, uint8_t *p_data, uint32_t data_len)
{
    st7789_state_t ret = ST7789_OK;

    if((NULL == p_drv) || (NULL == p_data) || (0U == data_len))
    {
        return ST7789_INVALID_PARAM;
    }
    if(0U != p_drv->async_busy)
    {
        return ST7789_BUSY;
    }

    /* DC pin = data mode */
    ret = p_drv->p_transport->pf_set_command_mode(0U);
    if(ST7789_OK != ret)
    {
        return ret;
    }
    /* SPI transmit data buffer dma or cpu*/
    while(data_len > 0)
    {
        uint16_t check_len = data_len >= 0xFFFFU ? 0xFFFFU : data_len;
        if(check_len > 16)
        {
            ret = p_drv->p_transport
                      ->pf_write_dma_blocking(p_data, check_len,
                                              p_drv->transfer_timeout_ms);
            if(ST7789_OK != ret)
            {
                return ret;
            }
        }
        else
        {
            ret = p_drv->p_transport->pf_write(p_data, check_len,
                                               p_drv->transfer_timeout_ms);
            if(ST7789_OK != ret)
            {
                return ret;
            }
        }
        p_data += check_len;
        data_len -= check_len;
    }
    return ret;
}

/**
  * @brief            :  [st7789_write_cmd_data]
                         Write a command followed by a single data byte.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint8_t cmd, uint8_t data]
  */
static st7789_state_t
st7789_write_cmd_data(st7789_dev_t *p_drv, uint8_t cmd, uint8_t data)
{
    st7789_state_t ret = ST7789_OK;
    ret                = st7789_write_cmd(p_drv, cmd);
    if(ST7789_OK != ret)
    {
        return ret;
    }
    ret = st7789_write_data(p_drv, data);
    return ret;
}

/**
  * @brief            :  [st7789_write_reg]
                         Write a command followed by a multi-byte data payload.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint8_t cmd , \
                          uint8_t *p_data, uint32_t data_len]
  */
static st7789_state_t st7789_write_reg(st7789_dev_t *p_drv,
                                       uint8_t       cmd,
                                       uint8_t      *p_data,
                                       uint32_t      data_len)
{
    st7789_state_t ret = ST7789_OK;
    if(NULL == p_data || NULL == p_drv || 0U == data_len)
    {
        return ST7789_INVALID_PARAM;
    }
    ret = st7789_write_cmd(p_drv, cmd);
    if(ST7789_OK != ret)
    {
        return ret;
    }
    ret = st7789_write_buf(p_drv, p_data, data_len);
    return ret;
}

/**
  * @brief            :  [st7789_init]
                         Initialize the ST7789 hardware (register sequence).
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv]
  */
static st7789_state_t st7789_apply_init_sequence(st7789_dev_t *p_drv)
{
    st7789_state_t ret = ST7789_OK;
    memset(p_drv->p_work_buffer, 0, p_drv->work_buffer_size);
    /* Step 1: Software reset */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_SWRESET);
    if(ST7789_OK != ret)
    {
        return ret;
    }
    p_drv->p_transport->pf_delay_ms(ST7789_SWRESET_DELAY_MS);

    /* Step 2: Sleep out */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_SLPOUT);
    if(ST7789_OK != ret)
    {
        return ret;
    }
    p_drv->p_transport->pf_delay_ms(ST7789_SLPOUT_DELAY_MS);

    /* Step 3: Interface pixel format 鈥?RGB565 (16-bit) */
    ret = st7789_write_cmd_data(p_drv, ST7789_CMD_COLMOD, 0x05U);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 4: Porch setting */
    {
        uint8_t porch_data[] = {0x0CU, 0x0CU, 0x00U, 0x33U, 0x33U};
        ret = st7789_write_reg(p_drv, 0xB2U, porch_data, sizeof(porch_data));
        if(ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Step 5: Gate control */
    ret = st7789_write_cmd_data(p_drv, 0xB7U, 0x35U);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 6: VCOM setting */
    ret = st7789_write_cmd_data(p_drv, 0xBBU, 0x19U);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 7: LCM control */
    ret = st7789_write_cmd_data(p_drv, 0xC0U, 0x2CU);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 8: VDV and VRH command enable */
    ret = st7789_write_cmd_data(p_drv, 0xC2U, 0x01U);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 9: VRH set */
    ret = st7789_write_cmd_data(p_drv, 0xC3U, 0x12U);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 10: VDV set */
    ret = st7789_write_cmd_data(p_drv, 0xC4U, 0x20U);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 11: Frame rate control */
    ret = st7789_write_cmd_data(p_drv, 0xC6U, 0x0FU);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 12: Power control 1 */
    {
        uint8_t pwr_data[] = {0xA4U, 0xA1U};
        ret = st7789_write_reg(p_drv, 0xD0U, pwr_data, sizeof(pwr_data));
        if(ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Step 13: Positive gamma correction */
    {
        uint8_t gamma_p[] = {0xD0U, 0x04U, 0x0DU, 0x11U, 0x13U, 0x2BU, 0x3FU,
                             0x54U, 0x4CU, 0x18U, 0x0DU, 0x0BU, 0x1FU, 0x23U};
        ret = st7789_write_reg(p_drv, 0xE0U, gamma_p, sizeof(gamma_p));
        if(ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Step 14: Negative gamma correction */
    {
        uint8_t gamma_n[] = {0xD0U, 0x04U, 0x0CU, 0x11U, 0x13U, 0x2CU, 0x3FU,
                             0x44U, 0x51U, 0x2FU, 0x1FU, 0x1FU, 0x20U, 0x23U};
        ret = st7789_write_reg(p_drv, 0xE1U, gamma_n, sizeof(gamma_n));
        if(ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Step 15: Display inversion on */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_INVON);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 16: Sleep out (again, as per datasheet sequence) */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_SLPOUT);
    if(ST7789_OK != ret)
    {
        return ret;
    }
    p_drv->p_transport->pf_delay_ms(ST7789_SLPOUT_DELAY_MS);

    /* Step 17: Display on */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_DISPON);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    return ST7789_OK;
}

/**
  * @brief            :  [st7789_deinit]
                         Deinitialize the ST7789 hardware (register sequence).
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_dev_t *p_drv]
  */
st7789_state_t st7789_deinit(st7789_dev_t *p_drv)
{
    if(NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }

    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }
    if(0U != p_drv->async_busy)
    {
        return ST7789_BUSY;
    }

    p_drv->is_init = ST7789_DRIVER_NOT_INIT;
    return ST7789_OK;
}

/**
  * @brief            :  [st7789_set_window]
                         Set column and row address window.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,
                             ST7789_BUSY            = 0x02U,
                             ST7789_TIMEOUT         = 0x03U,
                             ST7789_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint16_t xs, uint16_t ys, \
                          uint16_t xe, uint16_t ye ]
  */
static st7789_state_t st7789_set_window(
    st7789_dev_t *p_drv, uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
    st7789_state_t ret = ST7789_OK;

    if(NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }

    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    if((xs >= p_drv->width) || (xe >= p_drv->width) || (ys >= p_drv->height) ||
       (ye >= p_drv->height) || (xs > xe) || (ys > ye))
    {
        return ST7789_INVALID_PARAM;
    }

    /* Column address set (0x2A) */
    {
        uint8_t col_data[] = {(uint8_t)(xs >> 8U), (uint8_t)(xs & 0xFFU),
                              (uint8_t)(xe >> 8U), (uint8_t)(xe & 0xFFU)};
        ret                = st7789_write_reg(p_drv, ST7789_CMD_CASET, col_data,
                                              sizeof(col_data));
        if(ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Row address set (0x2B) */
    {
        uint8_t row_data[] = {(uint8_t)(ys >> 8U), (uint8_t)(ys & 0xFFU),
                              (uint8_t)(ye >> 8U), (uint8_t)(ye & 0xFFU)};
        ret                = st7789_write_reg(p_drv, ST7789_CMD_RASET, row_data,
                                              sizeof(row_data));
        if(ST7789_OK != ret)
        {
            return ret;
        }
    }

    return ST7789_OK;
}

/**
  * @brief            :  [st7789_fill_screen]
                         Fill entire screen with a single RGB565 color.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,
                             ST7789_BUSY            = 0x02U,
                             ST7789_TIMEOUT         = 0x03U,
                             ST7789_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [st7789_dev_t *p_drv , \
                          uint16_t color ]
  */
st7789_state_t st7789_fill_screen(st7789_dev_t *p_drv, uint16_t color)
{
    st7789_state_t ret = ST7789_OK;
    uint32_t       i   = 0U;
    uint16_t       row = 0U;

    if(NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }

    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    /* Set window to full screen */
    ret = st7789_set_window(p_drv, 0U, 0U, p_drv->width - 1U,
                            p_drv->height - 1U);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Memory write command */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Fill line buffer with the color (RGB565: high byte first) */
    for(i = 0U; i < p_drv->work_buffer_size; i += 2U)
    {
        p_drv->p_work_buffer[i]      = (uint8_t)(color >> 8U);
        p_drv->p_work_buffer[i + 1U] = (uint8_t)(color & 0x00FFU);
    }
    /* Send line buffer row by row */
    /* DC pin = data mode */
    /* SPI transmit data buffer */
    for(row = 0U; row < p_drv->height; row++)
    {
        ret = st7789_write_buf(p_drv, (uint8_t *)p_drv->p_work_buffer,
                               p_drv->work_buffer_size);
        if(ST7789_OK != ret)
        {
            return ret;
        }
    }
    return ST7789_OK;
}

/**
  * @brief            :  [st7789_clear_screen]
                         Clear entire screen (fill with black 0x0000).
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,
                             ST7789_BUSY            = 0x02U,
                             ST7789_TIMEOUT         = 0x03U,
                             ST7789_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [st7789_dev_t *p_drv]
  */
ST7789_NOT_USE_FUNCTION
static st7789_state_t st7789_clear_screen(st7789_dev_t *p_drv)
{
    return st7789_fill_screen(p_drv, 0x0000U);
}

/**
  * @brief            :  [st7789_fill_rect]
                         Fill a rectangular region with a single RGB565 color.
                         Sends one row at a time via the DMA line buffer,
                         so only the row width (w*2 bytes) is transmitted
                         per row 鈥?much faster than clearing the full screen.
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,
                          ST7789_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [st7789_dev_t *p_drv,
                          uint16_t x, uint16_t y,
                          uint16_t w, uint16_t h,
                          uint16_t color]
  */
st7789_state_t st7789_fill_rect(st7789_dev_t *p_drv,
                                uint16_t      x,
                                uint16_t      y,
                                uint16_t      w,
                                uint16_t      h,
                                uint16_t      color)
{
    st7789_state_t ret       = ST7789_OK;
    uint32_t       row_bytes = 0U;
    uint32_t       i         = 0U;
    uint16_t       row       = 0U;

    if(NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }

    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    if((0U == w) || (0U == h) || ((uint32_t)x + w > p_drv->width) ||
       ((uint32_t)y + h > p_drv->height))
    {
        return ST7789_INVALID_PARAM;
    }

    ret = st7789_set_window(p_drv, x, y, (uint16_t)(x + w - 1U),
                            (uint16_t)(y + h - 1U));
    if(ST7789_OK != ret)
    {
        return ret;
    }

    ret = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Pre-fill one row of pixels into the line buffer */
    row_bytes = (uint32_t)w * 2U;
    for(i = 0U; i < row_bytes; i += 2U)
    {
        p_drv->p_work_buffer[i]      = (uint8_t)(color >> 8U);
        p_drv->p_work_buffer[i + 1U] = (uint8_t)(color & 0x00FFU);
    }

    /* Send the same row buffer h times */
    for(row = 0U; row < h; row++)
    {
        ret = st7789_write_buf(p_drv, p_drv->p_work_buffer, row_bytes);
        if(ST7789_OK != ret)
        {
            return ret;
        }
    }

    return ST7789_OK;
}

/**
  * @brief            :  [st7789_draw_char]
                         Draw a single printable ASCII character at pixel
                         position (x, y). Supports variable-height fonts
                         (e.g. 6x8, 8x16). Pixels rendered row-major in
                         RGB565 big-endian order via the DMA line buffer.
                         Font layout: column-major, each column stored as
                         ceil(char_h/8) bytes (LSB = top pixel per byte).
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,
                          ST7789_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [st7789_dev_t *p_drv, const front_def_t *p_font,
                          uint16_t x, uint16_t y, char ch,
                          uint16_t f_color, uint16_t b_color]
  */
st7789_state_t st7789_draw_char(st7789_dev_t      *p_drv,
                                const front_def_t *p_font,
                                uint16_t           x,
                                uint16_t           y,
                                char               ch,
                                uint16_t           f_color,
                                uint16_t           b_color)
{
    st7789_state_t ret            = ST7789_OK;
    uint8_t        idx            = 0U;
    uint8_t        col            = 0U;
    uint8_t        row            = 0U;
    uint32_t       buf_i          = 0U;
    uint16_t       pixel          = 0U;
    uint8_t        bytes_per_col  = 0U;
    uint8_t        bytes_per_char = 0U;
    const uint8_t *char_data      = NULL;

    if((NULL == p_drv) || (NULL == p_font) || (NULL == p_font->p_data))
    {
        return ST7789_INVALID_PARAM;
    }

    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    /* Printable ASCII range: 0x20 (space) to 0x20 + char_count - 1 */
    if(((uint8_t)ch < ST7789_FONT_CHAR_OFFSET) ||
       ((uint8_t)ch >= (uint8_t)(ST7789_FONT_CHAR_OFFSET + p_font->char_count)))
    {
        return ST7789_INVALID_PARAM;
    }

    /* Character bounding box must fit within screen */
    if(((uint16_t)(x + p_font->char_w - 1U) >= p_drv->width) ||
       ((uint16_t)(y + p_font->char_h - 1U) >= p_drv->height))
    {
        return ST7789_INVALID_PARAM;
    }

    /* bytes_per_col = char_h / 8 (char_h must be a multiple of 8) */
    bytes_per_col  = (uint8_t)(p_font->char_h / 8U);
    bytes_per_char = (uint8_t)(p_font->char_w * bytes_per_col);

    /* Guard: pixel buffer must fit in p_drv->p_work_buffer */
    if(((uint32_t)p_font->char_w * p_font->char_h * 2U) >
       p_drv->work_buffer_size)
    {
        return ST7789_INVALID_PARAM;
    }

    idx       = (uint8_t)ch - (uint8_t)ST7789_FONT_CHAR_OFFSET;
    char_data = &p_font->p_data[(uint16_t)idx * bytes_per_char];

    ret = st7789_set_window(p_drv, x, y, (uint16_t)(x + p_font->char_w - 1U),
                            (uint16_t)(y + p_font->char_h - 1U));
    if(ST7789_OK != ret)
    {
        return ret;
    }

    ret = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Build pixel buffer: row-major, RGB565 big-endian.
       For column c, row r: byte = char_data[(r/8)*char_w + c], bit = r%8. */
    buf_i = 0U;
    for(row = 0U; row < p_font->char_h; row++)
    {
        uint8_t row_byte = row / 8U;
        uint8_t row_bit  = row % 8U;
        for(col = 0U; col < p_font->char_w; col++)
        {
            pixel = ((char_data[(uint8_t)(row_byte * p_font->char_w) + col] >>
                      row_bit) &
                     1U)
                        ? f_color
                        : b_color;
            p_drv->p_work_buffer[buf_i]      = (uint8_t)(pixel >> 8U);
            p_drv->p_work_buffer[buf_i + 1U] = (uint8_t)(pixel & 0x00FFU);
            buf_i += 2U;
        }
    }

    return st7789_write_buf(p_drv, p_drv->p_work_buffer, buf_i);
}

/**
 * @brief            :  [st7789_draw_string]
 *                       Draw a null-terminated printable ASCII string
 *                       starting at (x, y). Characters are placed
 *                       left-to-right; drawing stops when the next
 *                       character would exceed the screen width.
 * @retval           :  [ST7789_OK              = 0x00U,
 *                        ST7789_ERROR           = 0x01U,
 *                        ST7789_INVALID_PARAM   = 0x04U,]
 * @param[in]        :  [st7789_dev_t *p_drv, const front_def_t *p_font,
 *                        uint16_t x, uint16_t y, const char *p_str,
 *                        uint16_t f_color, uint16_t b_color]
 */
st7789_state_t st7789_draw_string(st7789_dev_t      *p_drv,
                                  const front_def_t *p_font,
                                  uint16_t           x,
                                  uint16_t           y,
                                  const char        *p_str,
                                  uint16_t           f_color,
                                  uint16_t           b_color)
{
    st7789_state_t ret = ST7789_OK;
    uint16_t       cx  = x;

    if((NULL == p_drv) || (NULL == p_font) || (NULL == p_str))
    {
        return ST7789_INVALID_PARAM;
    }

    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    while(*p_str)
    {
        if(((uint32_t)cx + p_font->char_w) > p_drv->width)
        {
            break;
        }

        ret = st7789_draw_char(p_drv, p_font, cx, y, *p_str, f_color, b_color);
        if(ST7789_OK != ret)
        {
            return ret;
        }

        cx = (uint16_t)(cx + p_font->char_w);
        p_str++;
    }

    return ST7789_OK;
}

/**
 * @brief            :  [st7789_draw_image]
 *                       Blit a w脳h RGB565 big-endian pixel buffer at (x, y).
 *                       p_pixels must point to w*h*2 bytes in DMA-accessible
 *                       memory (e.g. .ram_dma_buffers or AXI SRAM).
 * @retval           :  [ST7789_OK              = 0x00U,
 *                        ST7789_ERROR           = 0x01U,
 *                        ST7789_INVALID_PARAM   = 0x04U,]
 * @param[in]        :  [st7789_dev_t *p_drv,
 *                        uint16_t x, uint16_t y, uint16_t w, uint16_t h,
 *                        const uint8_t *p_pixels]
 */
st7789_state_t st7789_draw_image(st7789_dev_t  *p_drv,
                                 uint16_t       x,
                                 uint16_t       y,
                                 uint16_t       w,
                                 uint16_t       h,
                                 const uint8_t *p_pixels)
{
    st7789_state_t ret = ST7789_OK;

    if((NULL == p_drv) || (NULL == p_pixels))
    {
        return ST7789_INVALID_PARAM;
    }

    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    if((0U == w) || (0U == h) || ((uint32_t)x + w > p_drv->width) ||
       ((uint32_t)y + h > p_drv->height))
    {
        return ST7789_INVALID_PARAM;
    }

    ret = st7789_set_window(p_drv, x, y, (uint16_t)(x + w - 1U),
                            (uint16_t)(y + h - 1U));
    if(ST7789_OK != ret)
    {
        return ret;
    }

    ret = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    /* Send row by row through p_drv->p_work_buffer so p_pixels can reside
       anywhere (Flash, DTCM, non-DMA SRAM) 鈥?no DMA-accessible requirement on
       caller. */
    {
        uint32_t row_bytes = (uint32_t)w * 2U;
        uint16_t row       = 0U;
        for(row = 0U; row < h; row++)
        {
            memcpy(p_drv->p_work_buffer, p_pixels + (uint32_t)row * row_bytes,
                   row_bytes);
            ret = st7789_write_buf(p_drv, p_drv->p_work_buffer, row_bytes);
            if(ST7789_OK != ret)
            {
                return ret;
            }
        }
    }
    return ST7789_OK;
}

/**
 * @brief Finish an asynchronous image transfer from transport ISR context.
 */
static void st7789_async_transport_complete(st7789_state_t status,
                                            void          *p_context)
{
    st7789_dev_t *p_drv = (st7789_dev_t *)p_context;
    if((NULL == p_drv) || (0U == p_drv->async_busy))
    {
        return;
    }

    st7789_async_complete_cb_t callback = p_drv->async_callback;
    void *p_callback_context = p_drv->p_async_context;
    p_drv->async_callback    = NULL;
    p_drv->p_async_context   = NULL;
    p_drv->async_busy        = 0U;

    if(NULL != callback)
    {
        callback(status, p_callback_context);
    }
}

st7789_state_t
st7789_draw_image_async(st7789_dev_t              *p_drv,
                        uint16_t                   x,
                        uint16_t                   y,
                        uint16_t                   w,
                        uint16_t                   h,
                        const uint8_t             *p_pixels,
                        st7789_async_complete_cb_t callback,
                        void                      *p_context)
{
    if((NULL == p_drv) || (NULL == p_pixels))
    {
        return ST7789_INVALID_PARAM;
    }
    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_NOT_INITIALIZED;
    }
    if(0U != p_drv->async_busy)
    {
        return ST7789_BUSY;
    }
    if((0U == w) || (0U == h) || ((uint32_t)x + w > p_drv->width) ||
       ((uint32_t)y + h > p_drv->height))
    {
        return ST7789_INVALID_PARAM;
    }

    uint64_t transfer_size = (uint64_t)w * (uint64_t)h * 2ULL;
    if(transfer_size > UINT32_MAX)
    {
        return ST7789_INVALID_PARAM;
    }

    st7789_state_t status = st7789_set_window(
        p_drv, x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));
    if(ST7789_OK != status)
    {
        return status;
    }

    status = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if(ST7789_OK != status)
    {
        return status;
    }
    status = p_drv->p_transport->pf_set_command_mode(0U);
    if(ST7789_OK != status)
    {
        return status;
    }

    p_drv->async_callback  = callback;
    p_drv->p_async_context = p_context;
    p_drv->async_busy      = 1U;
    status = p_drv->p_transport->pf_write_dma_async(
        p_pixels, (uint32_t)transfer_size, st7789_async_transport_complete,
        p_drv);
    if(ST7789_OK != status)
    {
        p_drv->async_callback  = NULL;
        p_drv->p_async_context = NULL;
        p_drv->async_busy      = 0U;
    }
    return status;
}

st7789_state_t st7789_abort_async(st7789_dev_t *p_drv)
{
    if(NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }
    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_NOT_INITIALIZED;
    }
    if(0U == p_drv->async_busy)
    {
        return ST7789_OK;
    }

    st7789_state_t status = p_drv->p_transport->pf_abort_dma();
    p_drv->async_callback  = NULL;
    p_drv->p_async_context = NULL;
    p_drv->async_busy      = 0U;
    return status;
}

/**
 * @brief            :  [st7789_dec_to_str]
 *                       Convert int32_t to null-terminated decimal string.
 *                       Uses two's-complement negation 鈥?safe for INT32_MIN.
 * @param[in]        :  [int32_t val, char *buf, uint8_t bufsz]
 */
static void st7789_dec_to_str(int32_t val, char *buf, uint8_t bufsz)
{
    uint8_t  pos = 0U;
    char     tmp[11U];
    uint8_t  ti = 0U;
    uint32_t uval;

    if(bufsz < 2U)
    {
        buf[0U] = '\0';
        return;
    }

    if(val < 0)
    {
        buf[pos++] = '-';
        uval       = (~(uint32_t)val) +
                     1U; /* two's-complement: avoids UB on INT32_MIN */
    }
    else
    {
        uval = (uint32_t)val;
    }

    if(0U == uval)
    {
        tmp[ti++] = '0';
    }
    else
    {
        while((uval > 0U) && (ti < (uint8_t)sizeof(tmp)))
        {
            tmp[ti++] = (char)('0' + (uint8_t)(uval % 10U));
            uval /= 10U;
        }
    }
    for(uint8_t a = 0U, b = (uint8_t)(ti - 1U); a < b; a++, b--)
    {
        char t = tmp[a];
        tmp[a] = tmp[b];
        tmp[b] = t;
    }
    for(uint8_t i = 0U; i < ti && pos < (bufsz - 1U); i++)
    {
        buf[pos++] = tmp[i];
    }
    buf[pos] = '\0';
}

/**
 * @brief            :  [st7789_draw_dec]
 *                       Display a signed decimal integer at (x, y).
 * @retval           :  [ST7789_OK / ST7789_ERROR / ST7789_INVALID_PARAM]
 * @param[in]        :  [st7789_dev_t *p_drv, const front_def_t *p_font,
 *                        uint16_t x, uint16_t y, int32_t value,
 *                        uint16_t f_color, uint16_t b_color]
 */
st7789_state_t st7789_draw_dec(st7789_dev_t      *p_drv,
                               const front_def_t *p_font,
                               uint16_t           x,
                               uint16_t           y,
                               int32_t            value,
                               uint16_t           f_color,
                               uint16_t           b_color)
{
    char buf[ST7789_NUM_BUF_SIZE];

    if((NULL == p_drv) || (NULL == p_font))
    {
        return ST7789_INVALID_PARAM;
    }

    st7789_dec_to_str(value, buf, (uint8_t)sizeof(buf));
    return st7789_draw_string(p_drv, p_font, x, y, buf, f_color, b_color);
}

/**
 * @brief            :  [st7789_hex_to_str]
 *                       Convert uint32_t to uppercase hex string with "0x"
 * prefix. Nibble computed inline 鈥?no lookup table.
 * @param[in]        :  [uint32_t val, char *buf, uint8_t bufsz]
 */
static void st7789_hex_to_str(uint32_t val, char *buf, uint8_t bufsz)
{
    uint8_t pos = 0U;
    char    tmp[8U];
    uint8_t ti = 0U;
    uint8_t nib;

    if(bufsz < 3U)
    {
        buf[0U] = '\0';
        return;
    }

    buf[pos++] = '0';
    if(pos < (bufsz - 1U))
    {
        buf[pos++] = 'x';
    }

    if(0U == val)
    {
        if(pos < (bufsz - 1U))
        {
            buf[pos++] = '0';
        }
    }
    else
    {
        while((val > 0U) && (ti < (uint8_t)sizeof(tmp)))
        {
            nib       = (uint8_t)(val & 0xFU);
            tmp[ti++] = (nib < 10U) ? (char)('0' + nib)
                                    : (char)('A' + nib - 10U);
            val >>= 4U;
        }
        for(uint8_t a = 0U, b = (uint8_t)(ti - 1U); a < b; a++, b--)
        {
            char t = tmp[a];
            tmp[a] = tmp[b];
            tmp[b] = t;
        }
        for(uint8_t i = 0U; i < ti && pos < (bufsz - 1U); i++)
        {
            buf[pos++] = tmp[i];
        }
    }
    buf[pos] = '\0';
}

/**
 * @brief            :  [st7789_draw_hex]
 *                       Display an unsigned 32-bit value in hex at (x, y).
 * @retval           :  [ST7789_OK / ST7789_ERROR / ST7789_INVALID_PARAM]
 * @param[in]        :  [st7789_dev_t *p_drv, const front_def_t *p_font,
 *                        uint16_t x, uint16_t y, uint32_t value,
 *                        uint16_t f_color, uint16_t b_color]
 */
st7789_state_t st7789_draw_hex(st7789_dev_t      *p_drv,
                               const front_def_t *p_font,
                               uint16_t           x,
                               uint16_t           y,
                               uint32_t           value,
                               uint16_t           f_color,
                               uint16_t           b_color)
{
    char buf[ST7789_NUM_BUF_SIZE];

    if((NULL == p_drv) || (NULL == p_font))
    {
        return ST7789_INVALID_PARAM;
    }

    st7789_hex_to_str(value, buf, (uint8_t)sizeof(buf));
    return st7789_draw_string(p_drv, p_font, x, y, buf, f_color, b_color);
}

/**
 * @brief            :  [st7789_float_to_str]
 *                       Manual float鈫抯tring; avoids float-printf dependency.
 * @param[in]        :  [float val, uint8_t dec, char *buf, uint8_t bufsz]
 */
static void
st7789_float_to_str(float val, uint8_t dec, char *buf, uint8_t bufsz)
{
    uint8_t  pos   = 0U;
    uint32_t scale = 1U;
    uint8_t  d;
    char     tmp[12];
    uint8_t  ti = 0U;
    uint32_t total, ipart, fpart;

    if(bufsz < 2U)
    {
        buf[0] = '\0';
        return;
    }

    if(val < 0.0f)
    {
        if(pos < (bufsz - 1U))
        {
            buf[pos++] = '-';
        }
        val = -val;
    }

    for(d = 0U; d < dec; d++)
    {
        scale *= 10U;
    }
    total = (uint32_t)(val * (float)scale + 0.5f);
    ipart = total / scale;
    fpart = total % scale;

    if(0U == ipart)
    {
        tmp[ti++] = '0';
    }
    else
    {
        uint32_t v = ipart;
        while(v > 0U && ti < (uint8_t)sizeof(tmp))
        {
            tmp[ti++] = (char)('0' + (uint8_t)(v % 10U));
            v /= 10U;
        }
        for(uint8_t a = 0U, b = (uint8_t)(ti - 1U); a < b; a++, b--)
        {
            char t = tmp[a];
            tmp[a] = tmp[b];
            tmp[b] = t;
        }
    }
    for(uint8_t i = 0U; i < ti && pos < (bufsz - 1U); i++)
    {
        buf[pos++] = tmp[i];
    }

    if(dec > 0U && pos < (bufsz - 1U))
    {
        buf[pos++] = '.';
        uint32_t s = scale / 10U;
        for(d = 0U; d < dec && pos < (bufsz - 1U); d++)
        {
            buf[pos++] = (char)('0' +
                                (uint8_t)((s > 0U ? fpart / s : fpart) % 10U));
            if(s > 1U)
            {
                s /= 10U;
            }
        }
    }
    buf[pos] = '\0';
}

/**
 * @brief            :  [st7789_draw_float]
 *                       Display a float with `decimals` decimal places at (x,
 *                       y).
 * @retval           :  [ST7789_OK / ST7789_ERROR / ST7789_INVALID_PARAM]
 * @param[in]        :  [st7789_dev_t *p_drv, const front_def_t *p_font,
 *                        uint16_t x, uint16_t y, float value, uint8_t decimals,
 *                        uint16_t f_color, uint16_t b_color]
 */
st7789_state_t st7789_draw_float(st7789_dev_t      *p_drv,
                                 const front_def_t *p_font,
                                 uint16_t           x,
                                 uint16_t           y,
                                 float              value,
                                 uint8_t            decimals,
                                 uint16_t           f_color,
                                 uint16_t           b_color)
{
    char buf[ST7789_NUM_BUF_SIZE];

    if((NULL == p_drv) || (NULL == p_font))
    {
        return ST7789_INVALID_PARAM;
    }

    st7789_float_to_str(value, decimals, buf, (uint8_t)sizeof(buf));
    return st7789_draw_string(p_drv, p_font, x, y, buf, f_color, b_color);
}

/**
 * @brief            :  [st7789_draw_pixel]
 *                       Write a single RGB565 pixel at (x, y).
 *                       Private helper 鈥?caller must have validated p_drv
 *                       and is_init before invoking.
 * @param[in]        :  [st7789_dev_t *p_drv,
 *                        uint16_t x, uint16_t y, uint16_t color]
 */
static st7789_state_t
st7789_draw_pixel(st7789_dev_t *p_drv, uint16_t x, uint16_t y, uint16_t color)
{
    st7789_state_t ret = ST7789_OK;
    uint8_t        pix[2U];

    ret = st7789_set_window(p_drv, x, y, x, y);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    ret = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if(ST7789_OK != ret)
    {
        return ret;
    }

    pix[0U] = (uint8_t)(color >> 8U);
    pix[1U] = (uint8_t)(color & 0x00FFU);
    return st7789_write_buf(p_drv, pix, 2U);
}

/**
 * @brief            :  [st7789_draw_line]
 *                       Draw a straight line from (x0, y0) to (x1, y1)
 *                       in RGB565 color using Bresenham's algorithm.
 *                       Horizontal and vertical lines are routed through
 *                       st7789_fill_rect for better SPI throughput.
 * @retval           :  [ST7789_OK              = 0x00U,
 *                        ST7789_ERROR           = 0x01U,
 *                        ST7789_INVALID_PARAM   = 0x04U,]
 * @param[in]        :  [st7789_dev_t *p_drv,
 *                        uint16_t x0, uint16_t y0,
 *                        uint16_t x1, uint16_t y1,
 *                        uint16_t color]
 */
st7789_state_t st7789_draw_line(st7789_dev_t *p_drv,
                                uint16_t      x0,
                                uint16_t      y0,
                                uint16_t      x1,
                                uint16_t      y1,
                                uint16_t      color)
{
    st7789_state_t ret = ST7789_OK;
    int16_t        dx  = 0;
    int16_t        dy  = 0;
    int16_t        sx  = 0;
    int16_t        sy  = 0;
    int16_t        err = 0;
    int16_t        e2  = 0;
    int16_t        cx  = 0;
    int16_t        cy  = 0;

    if(NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }

    if(ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    if((x0 >= p_drv->width) || (x1 >= p_drv->width) || (y0 >= p_drv->height) ||
       (y1 >= p_drv->height))
    {
        return ST7789_INVALID_PARAM;
    }

    /* Horizontal line fast path */
    if(y0 == y1)
    {
        uint16_t xs = (x0 < x1) ? x0 : x1;
        uint16_t xe = (x0 < x1) ? x1 : x0;
        return st7789_fill_rect(p_drv, xs, y0, (uint16_t)(xe - xs + 1U), 1U,
                                color);
    }

    /* Vertical line fast path */
    if(x0 == x1)
    {
        uint16_t ys = (y0 < y1) ? y0 : y1;
        uint16_t ye = (y0 < y1) ? y1 : y0;
        return st7789_fill_rect(p_drv, x0, ys, 1U, (uint16_t)(ye - ys + 1U),
                                color);
    }

    /* General case: Bresenham's line algorithm */
    dx  = (int16_t)((x1 > x0) ? (x1 - x0) : (x0 - x1));
    dy  = (int16_t)((y1 > y0) ? (y1 - y0) : (y0 - y1));
    sx  = (x0 < x1) ? 1 : -1;
    sy  = (y0 < y1) ? 1 : -1;
    err = (int16_t)(dx - dy);
    cx  = (int16_t)x0;
    cy  = (int16_t)y0;

    for(;;)
    {
        ret = st7789_draw_pixel(p_drv, (uint16_t)cx, (uint16_t)cy, color);
        if(ST7789_OK != ret)
        {
            return ret;
        }

        if((cx == (int16_t)x1) && (cy == (int16_t)y1))
        {
            break;
        }

        e2 = (int16_t)(err * 2);

        if(e2 > -dy)
        {
            err = (int16_t)(err - dy);
            cx  = (int16_t)(cx + sx);
        }

        if(e2 < dx)
        {
            err = (int16_t)(err + dx);
            cy  = (int16_t)(cy + sy);
        }
    }

    return ST7789_OK;
}

st7789_state_t st7789_init(st7789_dev_t             *p_drv,
                           const st7789_transport_t *p_transport,
                           uint16_t                  width,
                           uint16_t                  height,
                           uint8_t                  *p_work_buffer,
                           uint32_t                  work_buffer_size,
                           uint32_t                  transfer_timeout_ms)
{
    if(NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }
    if(ST7789_DRIVER_IS_INIT == p_drv->is_init)
    {
        return ST7789_OK;
    }

    p_drv->is_init = ST7789_DRIVER_NOT_INIT;
    if((NULL == p_transport) || (NULL == p_transport->pf_write) ||
       (NULL == p_transport->pf_write_dma_blocking) ||
       (NULL == p_transport->pf_write_dma_async) ||
       (NULL == p_transport->pf_abort_dma) ||
       (NULL == p_transport->pf_set_command_mode) ||
       (NULL == p_transport->pf_delay_ms) || (0U == width) || (0U == height) ||
       (NULL == p_work_buffer) || (work_buffer_size < ((uint32_t)width * 2U)) ||
       (0U == transfer_timeout_ms) || (UINT32_MAX == transfer_timeout_ms))
    {
        return ST7789_INVALID_PARAM;
    }

    p_drv->p_transport         = p_transport;
    p_drv->p_work_buffer       = p_work_buffer;
    p_drv->work_buffer_size    = work_buffer_size;
    p_drv->transfer_timeout_ms = transfer_timeout_ms;
    p_drv->width               = width;
    p_drv->height              = height;
    p_drv->async_busy          = 0U;
    p_drv->async_callback      = NULL;
    p_drv->p_async_context     = NULL;

    st7789_state_t status = st7789_apply_init_sequence(p_drv);
    if(ST7789_OK != status)
    {
        p_drv->p_transport = NULL;
        return status;
    }

    p_drv->is_init = ST7789_DRIVER_IS_INIT;
    return ST7789_OK;
}

/* end of  file -------------------------------------------------------------*/
