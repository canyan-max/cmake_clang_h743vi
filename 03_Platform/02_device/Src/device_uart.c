/**
 ******************************************************************************
 *@file               :   device_uart.c
 *@brief              :   Device UART — thin id-based forward to bsp_uart.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "device_uart.h"
#include "bsp_uart.h"

/* exported functions -------------------------------------------------------*/

platform_err_t device_uart_init(device_uart_id_t id)
{
    if(id >= DEVICE_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    bsp_uart_init((plat_uart_id_t)id);
    return PLATFORM_ERR_OK;
}

platform_err_t device_uart_send(device_uart_id_t id, const uint8_t *p_data,
                                 uint16_t size)
{
    if(id >= DEVICE_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    bsp_uart_send((plat_uart_id_t)id, p_data, size);
    return PLATFORM_ERR_OK;
}

uint16_t device_uart_read(device_uart_id_t id, uint8_t *p_buf,
                           uint16_t max_len)
{
    if(id >= DEVICE_UART_NUM)
    {
        return 0U;
    }
    return bsp_uart_read((plat_uart_id_t)id, p_buf, max_len);
}

uint16_t device_uart_available(device_uart_id_t id)
{
    if(id >= DEVICE_UART_NUM)
    {
        return 0U;
    }
    return bsp_uart_available((plat_uart_id_t)id);
}

/* end of file --------------------------------------------------------------*/
