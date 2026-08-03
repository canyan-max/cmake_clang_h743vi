/**
 ******************************************************************************
 *@file               :   mcu_uart.c
 *@brief              :   STM32 implementation of plat_uart (DMA + IDLE line).
 *                        Board wiring (id -> HAL handle) is fixed at compile
 *                        time via board_config.h.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "plat_uart.h"
#include "board_config.h"
/* define   -----------------------------------------------------------------*/
#define MCU_UART_TX_TIMEOUT_MS  (100U)

/* typedef  ---------------------------------------------------------------- */

/* variables ----------------------------------------------------------------*/
static UART_HandleTypeDef *const s_uart_handle_table[PLAT_UART_NUM] = {
    &BOARD_UART_PROTO1_HANDLE,
};
static plat_uart_rx_cb_t s_rx_cb_table[PLAT_UART_NUM];
static uint16_t          s_buf_size_table[PLAT_UART_NUM];

/* private  functions  ------------------------------------------------------*/

static plat_uart_id_t uart_find_id(const UART_HandleTypeDef *p_huart)
{
    for(uint8_t i = 0U; i < (uint8_t)PLAT_UART_NUM; i++)
    {
        if(s_uart_handle_table[i] == p_huart)
        {
            return (plat_uart_id_t)i;
        }
    }
    return PLAT_UART_NUM;
}

/* exported functions -------------------------------------------------------*/

platform_err_t plat_uart_send(plat_uart_id_t id, const uint8_t *p_data, uint16_t size)
{
    if((id >= PLAT_UART_NUM) || (NULL == p_data))
    {
        return PLATFORM_ERR_PARAM ;
    }
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_UART_Transmit(s_uart_handle_table[id], (uint8_t *)p_data, size,
                             MCU_UART_TX_TIMEOUT_MS);
    if(HAL_OK != ret)
    {
        return PLATFORM_ERR_HW;
    }
    return PLATFORM_ERR_OK;
}

platform_err_t plat_uart_receive_start(plat_uart_id_t id, uint8_t *p_buf,
                              uint16_t buf_size)
{
    if((id >= PLAT_UART_NUM) || (NULL == p_buf))
    {
        return PLATFORM_ERR_PARAM;
    }
    s_buf_size_table[id] = buf_size;
    HAL_StatusTypeDef ret = HAL_UARTEx_ReceiveToIdle_DMA(s_uart_handle_table[id], p_buf,
                                                        buf_size);
    if(HAL_OK != ret)
    {
        return PLATFORM_ERR_HW;
    }
    return PLATFORM_ERR_OK;
}

platform_err_t  plat_uart_set_rx_callback(plat_uart_id_t id, plat_uart_rx_cb_t cb)
{
    if(id >= PLAT_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    s_rx_cb_table[id] = cb;
    return PLATFORM_ERR_OK;
}

/* ---- HAL callback: dispatch to the matching instance ---------------------*/
/* HAL's own Size argument is derived from huart->RxXferCount, which is
 * written from two independent IRQ sources (DMA HT/TC on BDMA_Channel0_IRQn,
 * IDLE line on LPUART1_IRQn) racing near the circular buffer's wrap point.
 * Recompute the position ourselves, directly off the DMA counter, instead
 * of trusting whichever of the two paths happened to run last. */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // ((void)Size);

    plat_uart_id_t id = uart_find_id(huart);
    if(PLAT_UART_NUM == id)
    {
        return;
    }

    // uint16_t remaining   = (uint16_t)__HAL_DMA_GET_COUNTER(huart->hdmarx);
    // uint16_t actual_size = s_buf_size_table[id] - remaining;

    if(NULL != s_rx_cb_table[id])
    {
        s_rx_cb_table[id](id, Size);
    }
}

/* end of file --------------------------------------------------------------*/
