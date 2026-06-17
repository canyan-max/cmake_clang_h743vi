/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   st_lcd_spi.c
 *
 *@pardependencies    :
 *                        st_lcd_spi.c
 *                        spi.h
 *                        bsp_drv_st7789.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the ST7789 SPI platform ops implementation.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>                               /* stddef lib header file. */
#include "st_lcd_spi.h"                       /* st_lcd_spi lib header file. */
#include "bsp_drv_st7789.h"            /* bsp_drv_st7789 lib header file. */
#include "spi.h"                                     /* spi lib header file. */
#include "gpio.h"                                   /* gpio lib header file. */

/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static st7789_state_t st_spi_transmit (uint8_t  *p_data,
                                        uint32_t  lenth);
static st7789_state_t st_backlight_pin(uint8_t   on);
static st7789_state_t st_dataorcmd_pin (uint8_t   on);
static void           st_delay_ms      (uint32_t  ms);

const st7789_spi_ops_t g_st7789_spi_ops =
{
    .pf_spi_transmit    = st_spi_transmit,
    .pf_backlight_pin   = st_backlight_pin,
    .pf_dataorcmd_pin   = st_dataorcmd_pin,
    .pf_delay_ms        = st_delay_ms,
};

/* Private  functions  ------------------------------------------------------*/
/**
  * @brief            :  [st_spi_transmit]
                         Pure SPI transmit via SPI4, no DC pin control.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [uint8_t *p_data, uint32_t lenth]
  */
static st7789_state_t st_spi_transmit(uint8_t  *p_data,
                                       uint32_t  lenth)
{
    HAL_StatusTypeDef hal_ret = HAL_OK;

    if ((NULL == p_data) || (0U == lenth))
    {
        return ST7789_INVALID_PARAM;
    }

    hal_ret = HAL_SPI_Transmit(&hspi4, p_data, (uint16_t)lenth, 5000U);
    if (HAL_OK != hal_ret)
    {
        return ST7789_ERROR;
    }
    return ST7789_OK;
}

/**
  * @brief            :  [st_backlight_pin]
                         Set backlight pin: 1 = on (HIGH), 0 = off (LOW).
  * @retval           :  [   ST7789_OK              = 0x00U,]
  * @param[in]        :  [uint8_t on]
  */
static st7789_state_t st_backlight_pin(uint8_t on)
{
    GPIO_PinState pin_state = (0U != on) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(BACK_LIGHT_GPIO_Port, BACK_LIGHT_Pin, pin_state);
    return ST7789_OK;
}

/**
  * @brief            :  [st_dataorcmd_pin]
                         Set DC pin: 1 = command mode, 0 = data mode.
  * @retval           :  [   ST7789_OK              = 0x00U,]
  * @param[in]        :  [uint8_t on]
  */
static st7789_state_t st_dataorcmd_pin(uint8_t on)
{
    GPIO_PinState pin_state = (0U != on) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(SPI4_DC_GPIO_Port, SPI4_DC_Pin, pin_state);
    return ST7789_OK;
}

/**
  * @brief            :  [st_delay_ms]
                         Millisecond delay wrapper.
  * @retval           :  [none]
  * @param[in]        :  [uint32_t ms]
  */
static void st_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

/* end of  file -------------------------------------------------------------*/
