/**
 ******************************************************************************
 *@file               :   service_uart_test.h
 *@brief              :   Bring-up test: parse proto_simple frames off
 *                        DEVICE_UART_PROTO_1 and echo the payload back.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef SERVICE_UART_TEST_H
#define SERVICE_UART_TEST_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include "plat_error.h"

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
platform_err_t service_uart_test_init(void);
void           service_uart_test_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_UART_TEST_H */
