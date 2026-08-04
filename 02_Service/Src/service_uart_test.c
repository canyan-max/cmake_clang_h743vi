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
#include "plat_log.h"
#include "FreeRTOS.h"
#include "task.h"

/* define   -----------------------------------------------------------------*/
#define SERVICE_UART_TEST_POLL_BUF_SIZE     (64U)
#define SERVICE_UART_TEST_SEND_TIMEOUT_MS   (100U)

/* variables ----------------------------------------------------------------*/
static proto_simple_parser_t g_parser;
static TaskHandle_t          s_poll_task_handle;

/* private  functions  ------------------------------------------------------*/

/* Fired from ISR context (bsp_uart's DMA/IDLE handler) whenever new bytes
 * land in the RX fifo — wakes service_uart_test_poll()'s task instead of it
 * polling on a fixed delay. */
static void uart_rx_notify_cb(device_uart_id_t id)
{
    ((void)id);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(DEVICE_UART_PROTO_1 == id)
    {
        vTaskNotifyGiveFromISR(s_poll_task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }


}

/* exported functions -------------------------------------------------------*/

platform_err_t service_uart_test_init(void)
{
    proto_simple_init(&g_parser);
    s_poll_task_handle = xTaskGetCurrentTaskHandle();
    platform_err_t ret = device_uart_init(DEVICE_UART_PROTO_1);
    if(PLATFORM_ERR_OK != ret)
    {
        return ret;
    }
    return device_uart_set_rx_notify_cb(DEVICE_UART_PROTO_1, uart_rx_notify_cb);
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
            if(PLATFORM_ERR_OK != device_uart_send(DEVICE_UART_PROTO_1, &buf[i], 1U,
                                                    SERVICE_UART_TEST_SEND_TIMEOUT_MS))
            {
                plat_log_w("uart_test", "raw echo send failed");
            }
            continue;
        }

        proto_simple_frame_t frame;
        if(1U == proto_simple_feed(&g_parser, buf[i], &frame))
        {
            if(PLATFORM_ERR_OK != device_uart_send(DEVICE_UART_PROTO_1, frame.payload,
                                                    frame.len,
                                                    SERVICE_UART_TEST_SEND_TIMEOUT_MS))
            {
                plat_log_w("uart_test", "frame reply send failed");
            }
        }
    }
}

/* end of file --------------------------------------------------------------*/
