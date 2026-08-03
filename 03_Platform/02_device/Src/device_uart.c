/**
 ******************************************************************************
 *@file               :   device_uart.c
 *@brief              :   Device UART — thin id-based forward to bsp_uart.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "device_uart.h"
#include "bsp_uart.h"

/* variables ----------------------------------------------------------------*/
static device_uart_rx_notify_cb_t s_rx_notify_cb[DEVICE_UART_NUM];

/* private  functions  ------------------------------------------------------*/

/* Trampoline: bsp_uart's callback is typed plat_uart_id_t, ours is
 * device_uart_id_t — the two enums share the same values, so this just
 * re-dispatches to the registered device-level callback. Runs in ISR
 * context, same as bsp_uart's notify. */
static void device_uart_rx_notify_trampoline(plat_uart_id_t id)
{
    device_uart_id_t dev_id = (device_uart_id_t)id;
    if((dev_id < DEVICE_UART_NUM) && (NULL != s_rx_notify_cb[dev_id]))
    {
        s_rx_notify_cb[dev_id](dev_id);
    }
}

/* exported functions -------------------------------------------------------*/

platform_err_t device_uart_init(device_uart_id_t id)
{
    if(id >= DEVICE_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    return bsp_uart_init((plat_uart_id_t)id);
}

platform_err_t device_uart_send(device_uart_id_t id, const uint8_t *p_data,
                                 uint16_t size)
{
    if(id >= DEVICE_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    return bsp_uart_send((plat_uart_id_t)id, p_data, size);
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

platform_err_t device_uart_set_rx_notify_cb(device_uart_id_t          id,
                                             device_uart_rx_notify_cb_t cb)
{
    if(id >= DEVICE_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    s_rx_notify_cb[id] = cb;
    return bsp_uart_set_rx_notify_cb((plat_uart_id_t)id,
                                      device_uart_rx_notify_trampoline);
}

/* end of file --------------------------------------------------------------*/
