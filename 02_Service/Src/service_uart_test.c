/**
 ******************************************************************************
 *@file               :   service_uart_test.c
 *@brief              :   Bring-up test: parse proto_simple frames off
 *                        DEVICE_UART_PROTO_1 and echo the payload back.
 *                        Anything not recognized as a proto_simple frame
 *                        (wrong SOF) is echoed through raw, unparsed.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "service_uart_test.h"
#include "device_uart.h"
#include "proto_simple.h"

/* define   -----------------------------------------------------------------*/
#define SERVICE_UART_TEST_POLL_BUF_SIZE  (64U)

/* variables ----------------------------------------------------------------*/
static proto_simple_parser_t g_parser;

/* exported functions -------------------------------------------------------*/

platform_err_t service_uart_test_init(void)
{
    proto_simple_init(&g_parser);
    return device_uart_init(DEVICE_UART_PROTO_1);
}

void service_uart_test_poll(void)
{
    uint8_t buf[SERVICE_UART_TEST_POLL_BUF_SIZE];
    uint16_t n = device_uart_read(DEVICE_UART_PROTO_1, buf, sizeof(buf));

    for(uint16_t i = 0U; i < n; i++)
    {
        /* idle and not our SOF: not a proto_simple frame, pass it through */
        if((PROTO_SIMPLE_ST_SOF == g_parser.state) &&
           (PROTO_SIMPLE_SOF != buf[i]))
        {
            (void)device_uart_send(DEVICE_UART_PROTO_1, &buf[i], 1U);
            continue;
        }

        proto_simple_frame_t frame;
        if(1U == proto_simple_feed(&g_parser, buf[i], &frame))
        {
            (void)device_uart_send(DEVICE_UART_PROTO_1, frame.payload,
                                    frame.len);
        }
    }
}

/* end of file --------------------------------------------------------------*/
