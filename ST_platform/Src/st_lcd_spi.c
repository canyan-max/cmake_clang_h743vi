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
#include <stdint.h>
#include "st_lcd_spi.h"                       /* st_lcd_spi lib header file. */
#include "bsp_drv_st7789.h"            /* bsp_drv_st7789 lib header file. */
#include "spi.h"                                     /* spi lib header file. */
#include "log.h"
#include "core_dwt.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static st7789_state_t st_spi_transmit (uint8_t  *p_data,
                                        uint32_t  lenth);
static st7789_state_t st_spi_transmit_with_dma(uint8_t  *p_data,
                                       uint32_t  lenth);                                        
static st7789_state_t st_backlight_pin(uint8_t   on);
static st7789_state_t st_dc_pin (uint8_t   on);
static void           st_delay_ms      (uint32_t  ms);

const st7789_spi_ops_t g_st7789_spi_ops =
{
    .pf_spi_transmit          = st_spi_transmit,
    .pf_spi_transmit_with_dma = st_spi_transmit_with_dma,
    .pf_backlight_pin         = st_backlight_pin,
    .pf_dc_pin                = st_dc_pin,
    .pf_delay_ms              = st_delay_ms,
};
uint8_t g_dma_spi_tx_finish = 0;
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
  * @brief            :  [st_spi_transmit]
                         Pure SPI transmit via SPI4, no DC pin control.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,]
  * @param[in]        :  [uint8_t *p_data, uint32_t lenth]
  */
static st7789_state_t st_spi_transmit_with_dma(uint8_t  *p_data,
                                       uint32_t  lenth)
{
    HAL_StatusTypeDef hal_ret = HAL_OK;

    if ((NULL == p_data) || (0U == lenth))
    {
        return ST7789_INVALID_PARAM;
    }
    SCB_CleanDCache_by_Addr(p_data,lenth);
    // while(HAL_SPI_STATE_BUSY_TX == HAL_SPI_GetState(&hspi4) ) 
    // {
    // }
    while(HAL_SPI_STATE_READY != HAL_SPI_GetState(&hspi4) ) 
    {
    }
    hal_ret = HAL_SPI_Transmit_DMA(&hspi4, p_data, lenth);
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
  * @brief            :  [st_dc_pin]
                         Set DC pin: 1 = command mode, 0 = data mode.
  * @retval           :  [   ST7789_OK              = 0x00U,]
  * @param[in]        :  [uint8_t on]
  */
static st7789_state_t st_dc_pin(uint8_t on)
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
uint8_t TX_CODE ;
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi4)
    {
        TX_CODE+=1;
        g_dma_spi_tx_finish = 0;
    }
}
uint8_t ERR_CODE ;
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi4)
    {
        ERR_CODE+=1;
    }
}
/* end of  file -------------------------------------------------------------*/
