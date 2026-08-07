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
/* If no new byte arrives for this long while proto_files is mid-frame (a
 * frame started but then went silent), service_uart_test_poll() forces the
 * parser back to IDLE instead of waiting forever. Only applies mid-frame
 * (idx > 0): normal gaps between frames of a multi-pack session are exempt,
 * so a slow sender/ACKer doesn't get its transfer cut off.
 * Also used as the RX-wait timeout in freertos.c so the poll task actually
 * wakes up to check it. */
#define SERVICE_UART_TEST_IDLE_TIMEOUT_MS   (1000U)

/* typedef ------------------------------------------------------------------*/
/* Called from ISR context whenever new RX bytes land. Lets the caller supply
 * whatever "wake up and go call service_uart_test_poll()" mechanism fits its
 * environment (RTOS task notify, a flag polled in a bare-metal main loop,
 * ...) without this module knowing or caring which. */
typedef void (*service_uart_test_wake_cb_t)(void);

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
  * @brief            : [service_uart_test_init] init the parsers and start
  *                     listening on DEVICE_UART_PROTO_1.
  * @param[in]        : [wake_cb] fired from ISR context on new RX data; may
  *                     be NULL to disable the wake notification (poll must
  *                     then be driven some other way, e.g. a fixed period).
  */
platform_err_t service_uart_test_init(service_uart_test_wake_cb_t wake_cb);
void           service_uart_test_poll(void);

/* Debug probes (read-only): inspect the proto_files parser state without
 * exposing the parser struct. state is the low byte of proto_files_state_t. */
const uint8_t *service_uart_test_files_buf(void);
uint16_t       service_uart_test_files_idx(void);
uint8_t        service_uart_test_files_state(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_UART_TEST_H */
