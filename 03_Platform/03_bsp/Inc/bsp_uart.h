/**
 ******************************************************************************
 *@file               :   bsp_uart.h
 *@brief              :   UART BSP — instance-independent byte stream service.
 *                        Owns an RX ring buffer per instance; no protocol
 *                        awareness (framing/parsing happens above this layer).
 *                        Reuses plat_uart_id_t — board wiring lives in
 *                        mcu_uart.c, not here.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef BSP_UART_H
#define BSP_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>     /* stdint lib header file. */
#include "plat_error.h" /* platform error code header file. */
#include "plat_uart.h"  /* plat_uart_id_t */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
/* Fired from ISR context (uart_rx_event_handler) right after new bytes are
 * advanced into the RX fifo. Must be ISR-safe: no blocking calls. */
typedef void (*bsp_uart_rx_notify_cb_t)(plat_uart_id_t id);

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
platform_err_t bsp_uart_init(plat_uart_id_t id);
platform_err_t bsp_uart_send(plat_uart_id_t id, const uint8_t *p_data,
                              uint16_t size);
uint16_t bsp_uart_read(plat_uart_id_t id, uint8_t *p_buf, uint16_t max_len);
uint16_t bsp_uart_available(plat_uart_id_t id);
platform_err_t bsp_uart_set_rx_notify_cb(plat_uart_id_t id,
                                          bsp_uart_rx_notify_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */
