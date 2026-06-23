/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_drv_st7789.c
 *
 *@pardependencies    :
 *                        bsp_drv_st7789.c
 *                        bsp_drv_st7789.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of ST7789 LCD driver.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <string.h>                               /* string lib header file. */
#include <stddef.h>                               /* stddef lib header file. */
#include "bsp_drv_st7789.h"               /* bsp_drv_st7789 lib header file. */
#include "front.h"                                 /* font table header file. */

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

#define ST7789_LINE_BUF_SIZE        (ST7789_SCREEN_WIDTH * 2U)

#define ST7789_FONT_CHAR_HEIGHT     (8U)         // Pixel rows per glyph.
#define ST7789_FONT_CHAR_OFFSET     (0x20U)      // ASCII offset: space.

/* typedef ------------------------------------------------------------------*/
/* variables ----------------------------------------------------------------*/
// line buffer the ram_dma_buffers from .ld file
__attribute__((section(".ram_dma_buffers"))) \
uint8_t sg_line_buf[ST7789_LINE_BUF_SIZE];
/* private  functions  ------------------------------------------------------*/
/**
 * @brief            :  [color24bit_to_rgb565
                         24bit color to rgb565]
 * @retval           :  [ uint16_t rgb 565]
 * @param[in]        :  [(uint32_t Color)]
 */
ST7789_NOT_USE_FUNCTION \
uint16_t color24bit_to_rgb565(uint32_t color)
{
    uint16_t r = 0, g = 0, b = 0,rgb565=0; 

    r = (uint16_t)((color & 0x00F80000) >> 8);
    g = (uint16_t)((color & 0x0000FC00) >> 5);
    b = (uint16_t)((color & 0x000000F8) >> 3);
    rgb565 = (uint16_t)(r | g | b);
    return rgb565;
}

/**
  * @brief            :  [st7789_set_forward_color]
                          set forward color
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint8_t cmd]
  */
ST7789_NOT_USE_FUNCTION \
static st7789_state_t st7789_set_forward_color(st7789_driver_t *p_drv, \
                                        uint32_t f_color)
{
    if(NULL == p_drv) return ST7789_INVALID_PARAM; 
    ST7789_NOT_USE_VARIABLE(f_color);
    // p_drv->f_color = color24bit_to_rgb565(f_color);
    return ST7789_OK;
}

/**
  * @brief            :  [st7789_set_back_color]
                          set forward color
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint8_t cmd]
  */
ST7789_NOT_USE_FUNCTION \
static st7789_state_t st7789_set_back_color(st7789_driver_t *p_drv, \
                                            uint32_t b_color)
{
    if(NULL == p_drv) return ST7789_INVALID_PARAM; 
    ST7789_NOT_USE_VARIABLE(b_color);
    // p_drv->b_color = color24bit_to_rgb565(b_color);
    return ST7789_OK;
}

/**
  * @brief            :  [st7789_write_cmd]
                         DC pin -> command mode, then SPI transmit 1 byte.
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint8_t cmd]
  */
static st7789_state_t st7789_write_cmd(st7789_driver_t *p_drv,
                                        uint8_t          cmd)
{
    st7789_state_t ret = ST7789_OK;
   
    /* DC pin = command mode */
    ret = p_drv->p_spi_ops->pf_dc_pin(1U);
    if (ST7789_OK != ret)
    {
        return ret;
    }
    /* SPI transmit command byte */
    ret = p_drv->p_spi_ops->pf_spi_transmit(&cmd, 1U);
    return ret;
}

/**
  * @brief            :  [st7789_write_data]
                         DC pin -> data mode, then SPI transmit 1 byte.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint8_t data]
  */
static st7789_state_t st7789_write_data(st7789_driver_t *p_drv,
                                        uint8_t          data)
{
    st7789_state_t ret = ST7789_OK;

    /* DC pin = data mode */
    ret = p_drv->p_spi_ops->pf_dc_pin(0U);
    if (ST7789_OK != ret)
    {
        return ret;
    }
    /* SPI transmit data byte */
    ret = p_drv->p_spi_ops->pf_spi_transmit(&data, \
                                             1U);
    return ret;
}

/**
  * @brief            :  [st7789_write_buf]
                         DC pin → data mode, then SPI transmit buffer.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint8_t *p_data, uint32_t data_len]
  */
static st7789_state_t st7789_write_buf(st7789_driver_t *p_drv,
                                       uint8_t         *p_data,
                                       uint32_t         data_len)
{
    st7789_state_t ret = ST7789_OK;

    if ((NULL == p_data) || (0U == data_len))
    {
        return ST7789_INVALID_PARAM;
    }

    /* DC pin = data mode */
    ret = p_drv->p_spi_ops->pf_dc_pin(0U);
    if (ST7789_OK != ret)
    {
        return ret;
    }
    /* SPI transmit data buffer dma or cpu*/
    while(data_len>0)
    {
        uint16_t check_len = data_len>=0xFFFFU?0xFFFFU:data_len;
        if(check_len>16)
        {
            ret = p_drv->p_spi_ops->pf_spi_transmit_with_dma(p_data, \
                                                    check_len);
            if (ST7789_OK != ret)
            {
                return ret;
            }
        }
        else
        {
            ret = p_drv->p_spi_ops->pf_spi_transmit(p_data, check_len);
            if (ST7789_OK != ret)
            {
                return ret;
            }
        }
        p_data+=check_len;
        data_len-=check_len;
    }
    return ret;
}

/**
  * @brief            :  [st7789_write_cmd_data]
                         Write a command followed by a single data byte.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint8_t cmd, uint8_t data]
  */
static st7789_state_t st7789_write_cmd_data(st7789_driver_t *p_drv,
                                            uint8_t          cmd,
                                            uint8_t          data)
{
    st7789_state_t ret = ST7789_OK;
    ret = st7789_write_cmd(p_drv, cmd);
    if (ST7789_OK != ret)
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
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint8_t cmd , \
                          uint8_t *p_data, uint32_t data_len]
  */
static st7789_state_t st7789_write_reg(st7789_driver_t *p_drv,
                                        uint8_t         cmd,
                                        uint8_t         *p_data,
                                        uint32_t        data_len)
{
    st7789_state_t ret = ST7789_OK;
    if(NULL == p_data || NULL == p_drv || 0U == data_len)
    {
        return ST7789_INVALID_PARAM;
    }
    ret = st7789_write_cmd(p_drv, cmd);
    if (ST7789_OK != ret)
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
  * @param[in]        :  [st7789_driver_t *p_drv]
  */
static st7789_state_t st7789_init(st7789_driver_t *p_drv)
{
    st7789_state_t ret = ST7789_OK;
    memset(sg_line_buf, 0,sizeof(sg_line_buf));
    /* Step 1: Software reset */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_SWRESET);
    if (ST7789_OK != ret)
    {
        return ret;
    }
    p_drv->p_spi_ops->pf_delay_ms(ST7789_SWRESET_DELAY_MS);

    /* Step 2: Sleep out */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_SLPOUT);
    if (ST7789_OK != ret)
    {
        return ret;
    }
    p_drv->p_spi_ops->pf_delay_ms(ST7789_SLPOUT_DELAY_MS);

    /* Step 3: Interface pixel format — RGB565 (16-bit) */
    ret = st7789_write_cmd_data(p_drv, ST7789_CMD_COLMOD, 0x05U);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 4: Porch setting */
    {
        uint8_t porch_data[] = {0x0CU, 0x0CU, 0x00U, 0x33U, 0x33U};
        ret = st7789_write_reg(p_drv, 0xB2U,
                                porch_data, sizeof(porch_data));
        if (ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Step 5: Gate control */
    ret = st7789_write_cmd_data(p_drv, 0xB7U, 0x35U);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 6: VCOM setting */
    ret = st7789_write_cmd_data(p_drv, 0xBBU, 0x19U);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 7: LCM control */
    ret = st7789_write_cmd_data(p_drv, 0xC0U, 0x2CU);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 8: VDV and VRH command enable */
    ret = st7789_write_cmd_data(p_drv, 0xC2U, 0x01U);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 9: VRH set */
    ret = st7789_write_cmd_data(p_drv, 0xC3U, 0x12U);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 10: VDV set */
    ret = st7789_write_cmd_data(p_drv, 0xC4U, 0x20U);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 11: Frame rate control */
    ret = st7789_write_cmd_data(p_drv, 0xC6U, 0x0FU);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 12: Power control 1 */
    {
        uint8_t pwr_data[] = {0xA4U, 0xA1U};
        ret = st7789_write_reg(p_drv, 0xD0U,
                                pwr_data, sizeof(pwr_data));
        if (ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Step 13: Positive gamma correction */
    {
        uint8_t gamma_p[] = {
            0xD0U, 0x04U, 0x0DU, 0x11U, 0x13U, 0x2BU, 0x3FU,
            0x54U, 0x4CU, 0x18U, 0x0DU, 0x0BU, 0x1FU, 0x23U
        };
        ret = st7789_write_reg(p_drv, 0xE0U,
                                gamma_p, sizeof(gamma_p));
        if (ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Step 14: Negative gamma correction */
    {
        uint8_t gamma_n[] = {
            0xD0U, 0x04U, 0x0CU, 
            0x11U, 0x13U, 0x2CU, 
            0x3FU, 0x44U, 0x51U, 
            0x2FU, 0x1FU, 0x1FU, 
            0x20U, 0x23U
        };
        ret = st7789_write_reg(p_drv, 0xE1U,
                                gamma_n, sizeof(gamma_n));
        if (ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Step 15: Display inversion on */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_INVON);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 16: Sleep out (again, as per datasheet sequence) */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_SLPOUT);
    if (ST7789_OK != ret)
    {
        return ret;
    }
    p_drv->p_spi_ops->pf_delay_ms(ST7789_SLPOUT_DELAY_MS);

    /* Step 17: Display on */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_DISPON);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Step 18: Backlight on */
    ret = p_drv->p_spi_ops->pf_backlight_pin(1U);
    if (ST7789_OK != ret)
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
  * @param[in]        :  [st7789_driver_t *p_drv]
  */
static st7789_state_t st7789_deinit(st7789_driver_t *p_drv)
{
    if (NULL == p_drv) 
    {
        return ST7789_INVALID_PARAM; 
    }

    if( ST7789_DRIVER_NOT_INIT == p_drv->is_init )
    {
        return  ST7789_ERROR;
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
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint16_t xs, uint16_t ys, \
                          uint16_t xe, uint16_t ye ]
  */
static st7789_state_t st7789_set_window(st7789_driver_t *p_drv,
                                         uint16_t         xs,
                                         uint16_t         ys,
                                         uint16_t         xe,
                                         uint16_t         ye)
{
    st7789_state_t ret = ST7789_OK;

    if (NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }

    if (ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    if ((xs >= ST7789_SCREEN_WIDTH)  ||
        (xe >= ST7789_SCREEN_WIDTH)  ||
        (ys >= ST7789_SCREEN_HEIGHT) ||
        (ye >= ST7789_SCREEN_HEIGHT) ||
        (xs > xe) || (ys > ye))
    {
        return ST7789_INVALID_PARAM;
    }

    /* Column address set (0x2A) */
    {
        uint8_t col_data[] = {
            (uint8_t)(xs >> 8U),
            (uint8_t)(xs & 0xFFU),
            (uint8_t)(xe >> 8U),
            (uint8_t)(xe & 0xFFU)
        };
        ret = st7789_write_reg(p_drv, ST7789_CMD_CASET,
                                col_data, sizeof(col_data));
        if (ST7789_OK != ret)
        {
            return ret;
        }
    }

    /* Row address set (0x2B) */
    {
        uint8_t row_data[] = {
            (uint8_t)(ys >> 8U),
            (uint8_t)(ys & 0xFFU),
            (uint8_t)(ye >> 8U),
            (uint8_t)(ye & 0xFFU)
        };
        ret = st7789_write_reg(p_drv, ST7789_CMD_RASET,
                                row_data, sizeof(row_data));
        if (ST7789_OK != ret)
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
  * @param[in]        :  [st7789_driver_t *p_drv , \
                          uint16_t color ]
  */
static st7789_state_t st7789_fill_screen(st7789_driver_t *p_drv,
                                          uint16_t        color)
{
    st7789_state_t ret = ST7789_OK;
    uint32_t       i   = 0U;
    uint16_t       row = 0U;

    if (NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }

    if (ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    /* Set window to full screen */
    ret = st7789_set_window(p_drv,
                         0U,
                         0U,
                         ST7789_SCREEN_WIDTH  - 1U,
                         ST7789_SCREEN_HEIGHT - 1U);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Memory write command */
    ret = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Fill line buffer with the color (RGB565: high byte first) */
    for (i = 0U; i < ST7789_LINE_BUF_SIZE; i+=2U)
    {
      sg_line_buf[i]       = (uint8_t)(color >> 8U);
      sg_line_buf[i + 1U]  = (uint8_t)(color & 0x00FFU);
    }
    /* Send line buffer row by row */
    /* DC pin = data mode */
    /* SPI transmit data buffer */
    for(row =0U;row<ST7789_SCREEN_HEIGHT;row++)
    {
        ret = st7789_write_buf(p_drv, (uint8_t*)sg_line_buf, \
                              ST7789_LINE_BUF_SIZE);
        // ret = p_drv->p_spi_ops->pf_spi_transmit((uint8_t*)sg_line_buf, \
        //                       ST7789_LINE_BUF_SIZE);
        // ret =  p_drv->p_spi_ops->pf_spi_transmit_with_dma((uint8_t*)sg_line_buf, \
        //                       ST7789_LINE_BUF_SIZE);
        if(ST7789_OK!=ret)
        {
            return ret ;
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
  * @param[in]        :  [st7789_driver_t *p_drv]
  */
static st7789_state_t st7789_clear_screen(st7789_driver_t *p_drv)
{
    return st7789_fill_screen(p_drv, 0x0000U);
}

/**
  * @brief            :  [st7789_fill_rect]
                         Fill a rectangular region with a single RGB565 color.
                         Sends one row at a time via the DMA line buffer,
                         so only the row width (w*2 bytes) is transmitted
                         per row — much faster than clearing the full screen.
  * @retval           :  [ST7789_OK              = 0x00U,
                          ST7789_ERROR           = 0x01U,
                          ST7789_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [st7789_driver_t *p_drv,
                          uint16_t x, uint16_t y,
                          uint16_t w, uint16_t h,
                          uint16_t color]
  */
static st7789_state_t st7789_fill_rect(st7789_driver_t *p_drv,
                                       uint16_t         x,
                                       uint16_t         y,
                                       uint16_t         w,
                                       uint16_t         h,
                                       uint16_t         color)
{
    st7789_state_t ret       = ST7789_OK;
    uint32_t       row_bytes = 0U;
    uint32_t       i         = 0U;
    uint16_t       row       = 0U;

    if (NULL == p_drv)
    {
        return ST7789_INVALID_PARAM;
    }

    if (ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    if ((0U == w) || (0U == h) ||
        ((uint32_t)x + w > ST7789_SCREEN_WIDTH) ||
        ((uint32_t)y + h > ST7789_SCREEN_HEIGHT))
    {
        return ST7789_INVALID_PARAM;
    }

    ret = st7789_set_window(p_drv, x, y,
                            (uint16_t)(x + w - 1U),
                            (uint16_t)(y + h - 1U));
    if (ST7789_OK != ret)
    {
        return ret;
    }

    ret = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Pre-fill one row of pixels into the line buffer */
    row_bytes = (uint32_t)w * 2U;
    for (i = 0U; i < row_bytes; i += 2U)
    {
        sg_line_buf[i]      = (uint8_t)(color >> 8U);
        sg_line_buf[i + 1U] = (uint8_t)(color & 0x00FFU);
    }

    /* Send the same row buffer h times */
    for (row = 0U; row < h; row++)
    {
        ret = st7789_write_buf(p_drv, sg_line_buf, row_bytes);
        if (ST7789_OK != ret)
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
  * @param[in]        :  [st7789_driver_t *p_drv, const front_def_t *p_font,
                          uint16_t x, uint16_t y, char ch,
                          uint16_t f_color, uint16_t b_color]
  */
static st7789_state_t st7789_draw_char(st7789_driver_t   *p_drv,
                                       const front_def_t *p_font,
                                       uint16_t           x,
                                       uint16_t           y,
                                       char               ch,
                                       uint16_t           f_color,
                                       uint16_t           b_color)
{
    st7789_state_t    ret             = ST7789_OK;
    uint8_t           idx             = 0U;
    uint8_t           col             = 0U;
    uint8_t           row             = 0U;
    uint32_t          buf_i           = 0U;
    uint16_t          pixel           = 0U;
    uint8_t           bytes_per_col   = 0U;
    uint8_t           bytes_per_char  = 0U;
    const uint8_t    *char_data       = NULL;

    if ((NULL == p_drv) || (NULL == p_font) || (NULL == p_font->p_data))
    {
        return ST7789_INVALID_PARAM;
    }

    if (ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    /* Printable ASCII range: 0x20 (space) to 0x20 + char_count - 1 */
    if (((uint8_t)ch < ST7789_FONT_CHAR_OFFSET) ||
        ((uint8_t)ch >= (uint8_t)(ST7789_FONT_CHAR_OFFSET + p_font->char_count)))
    {
        return ST7789_INVALID_PARAM;
    }

    /* Character bounding box must fit within screen */
    if (((uint16_t)(x + p_font->char_w - 1U) >= ST7789_SCREEN_WIDTH) ||
        ((uint16_t)(y + p_font->char_h - 1U)  >= ST7789_SCREEN_HEIGHT))
    {
        return ST7789_INVALID_PARAM;
    }

    /* bytes_per_col = char_h / 8 (char_h must be a multiple of 8) */
    bytes_per_col  = (uint8_t)(p_font->char_h / 8U);
    bytes_per_char = (uint8_t)(p_font->char_w * bytes_per_col);

    /* Guard: pixel buffer must fit in sg_line_buf */
    if (((uint32_t)p_font->char_w * p_font->char_h * 2U) > ST7789_LINE_BUF_SIZE)
    {
        return ST7789_INVALID_PARAM;
    }

    idx       = (uint8_t)ch - (uint8_t)ST7789_FONT_CHAR_OFFSET;
    char_data = &p_font->p_data[(uint16_t)idx * bytes_per_char];

    ret = st7789_set_window(p_drv, x, y,
                            (uint16_t)(x + p_font->char_w - 1U),
                            (uint16_t)(y + p_font->char_h - 1U));
    if (ST7789_OK != ret)
    {
        return ret;
    }

    ret = st7789_write_cmd(p_drv, ST7789_CMD_RAMWR);
    if (ST7789_OK != ret)
    {
        return ret;
    }

    /* Build pixel buffer: row-major, RGB565 big-endian.
       For column c, row r: byte = char_data[(r/8)*char_w + c], bit = r%8. */
    buf_i = 0U;
    for (row = 0U; row < p_font->char_h; row++)
    {
        uint8_t row_byte = row / 8U;
        uint8_t row_bit  = row % 8U;
        for (col = 0U; col < p_font->char_w; col++)
        {
            pixel = ((char_data[(uint8_t)(row_byte * p_font->char_w) + col] >> row_bit) & 1U) ?
                    f_color : b_color;
            sg_line_buf[buf_i]      = (uint8_t)(pixel >> 8U);
            sg_line_buf[buf_i + 1U] = (uint8_t)(pixel & 0x00FFU);
            buf_i += 2U;
        }
    }

    return st7789_write_buf(p_drv, sg_line_buf, buf_i);
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
  * @param[in]        :  [st7789_driver_t *p_drv, const front_def_t *p_font,
  *                        uint16_t x, uint16_t y, const char *p_str,
  *                        uint16_t f_color, uint16_t b_color]
  */
static st7789_state_t st7789_draw_string(st7789_driver_t   *p_drv,
                                          const front_def_t *p_font,
                                          uint16_t           x,
                                          uint16_t           y,
                                          const char        *p_str,
                                          uint16_t           f_color,
                                          uint16_t           b_color)
{
    st7789_state_t ret = ST7789_OK;
    uint16_t       cx  = x;

    if ((NULL == p_drv) || (NULL == p_font) || (NULL == p_str))
    {
        return ST7789_INVALID_PARAM;
    }

    if (ST7789_DRIVER_NOT_INIT == p_drv->is_init)
    {
        return ST7789_ERROR;
    }

    while (*p_str)
    {
        if (((uint32_t)cx + p_font->char_w) > ST7789_SCREEN_WIDTH)
        {
            break;
        }

        ret = st7789_draw_char(p_drv, p_font, cx, y, *p_str, f_color, b_color);
        if (ST7789_OK != ret)
        {
            return ret;
        }

        cx = (uint16_t)(cx + p_font->char_w);
        p_str++;
    }

    return ST7789_OK;
}

/* exported functions -------------------------------------------------------*/
/**
  * @brief            :  [st7789_driver_instruct]
                         Initialize the ST7789 driver with SPI ops.
                         Binds all function pointers to the driver instance
                         and executes the hardware init sequence.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,
                             ST7789_BUSY            = 0x02U,
                             ST7789_TIMEOUT         = 0x03U,
                             ST7789_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [st7789_driver_t        *p_drv,
                            const st7789_spi_ops_t *p_ops,
                            uint32_t f_color             ,
                            uint32_t b_color              ]
  */
st7789_state_t st7789_driver_instruct(st7789_driver_t        *p_drv,
                                      const st7789_spi_ops_t *p_ops)
{
    st7789_state_t ret = ST7789_OK;

    if ((NULL == p_drv) || (NULL == p_ops))
    {
        return ST7789_INVALID_PARAM;
    }

    if (ST7789_DRIVER_IS_INIT == p_drv->is_init)
    {
        return ST7789_OK;
    }

    /* Bind SPI platform ops */
    p_drv->p_spi_ops = p_ops;

    /* Bind driver function pointers */
    p_drv->pf_deinit       = st7789_deinit;
    p_drv->pf_fill_screen  = st7789_fill_screen;
    p_drv->pf_clear_screen = st7789_clear_screen;
    p_drv->pf_fill_rect    = st7789_fill_rect;
    p_drv->pf_draw_char    = st7789_draw_char;
    p_drv->pf_draw_string  = st7789_draw_string;

    /* Execute hardware init sequence */
    ret = st7789_init(p_drv);
    if (ST7789_OK != ret)
    {
        p_drv->p_spi_ops       = NULL;
        p_drv->pf_fill_screen  = NULL;
        p_drv->pf_clear_screen = NULL;
        p_drv->pf_fill_rect    = NULL;
        p_drv->pf_draw_char    = NULL;
        p_drv->pf_draw_string  = NULL;
        return ret;
    }
    p_drv->is_init = ST7789_DRIVER_IS_INIT;
    return ST7789_OK;
}

/* end of  file -------------------------------------------------------------*/
