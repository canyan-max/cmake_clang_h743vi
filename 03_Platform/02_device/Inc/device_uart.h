/**
 ******************************************************************************
 *@file               :   device_uart.h
 *@brief              :   Device UART — id-based byte stream API over bsp_uart.
 *                        No protocol awareness; framing/parsing lives above
 *                        this layer.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef DEVICE_UART_H
#define DEVICE_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include "plat_error.h"

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef enum
{
    DEVICE_UART_PROTO_1 = 0x00U,
    DEVICE_UART_NUM,
} device_uart_id_t;

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
platform_err_t device_uart_init(device_uart_id_t id);
platform_err_t device_uart_send(device_uart_id_t id, const uint8_t *p_data,
                                 uint16_t size);
uint16_t       device_uart_read(device_uart_id_t id, uint8_t *p_buf,
                                 uint16_t max_len);
uint16_t       device_uart_available(device_uart_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_UART_H */
