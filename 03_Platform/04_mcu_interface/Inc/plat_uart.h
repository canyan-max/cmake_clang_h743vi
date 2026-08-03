/**
 ******************************************************************************
 *@file               :   plat_uart.h
 *@brief              :   MCU UART abstraction — send / start DMA+IDLE receive /
 *                        register RX event callback, addressed by logical id.
 *                        Board wiring (id -> HAL handle) is resolved inside
 *                        the implementation.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef PLAT_UART_H
#define PLAT_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef enum
{
    PLAT_UART_PROTO_1 = 0x00U,
    PLAT_UART_NUM,
} plat_uart_id_t;

typedef void (*plat_uart_rx_cb_t)(plat_uart_id_t id, uint16_t size);

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
void plat_uart_send(plat_uart_id_t id, const uint8_t *p_data, uint16_t size);
void plat_uart_receive_start(plat_uart_id_t id, uint8_t *p_buf,
                              uint16_t buf_size);
void plat_uart_set_rx_callback(plat_uart_id_t id, plat_uart_rx_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_UART_H */
