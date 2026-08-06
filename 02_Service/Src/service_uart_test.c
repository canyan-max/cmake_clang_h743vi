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
#include "proto_files.h"
/* define   -----------------------------------------------------------------*/
#define SERVICE_UART_TEST_POLL_BUF_SIZE     (64U)
#define SERVICE_UART_TEST_SEND_TIMEOUT_MS   (100U)

/* variables ----------------------------------------------------------------*/
static proto_simple_parser_t g_parser;
static TaskHandle_t          s_poll_task_handle;
proto_files_parser_t         g_files_parser;
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
#ifdef USE_DEBUG_LOG
    plat_log_i("uart_test", "service uart init");
#endif
    proto_simple_init(&g_parser);
    proto_files_init(&g_files_parser);
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
    uint8_t  buf[SERVICE_UART_TEST_POLL_BUF_SIZE];
    uint16_t n;

    /* One wake-up can lag behind an arbitrary amount of already-arrived
     * data (ulTaskNotifyTake(pdTRUE, ...) coalesces multiple ISR notifies
     * into a single wake). Drain the fifo down to empty here instead of
     * reading one fixed-size chunk, or leftover bytes never get another
     * chance to be read once the sender goes quiet. */
    while((n = device_uart_read(DEVICE_UART_PROTO_1, buf,
                                SERVICE_UART_TEST_POLL_BUF_SIZE)) > 0U)
    {
#ifdef USE_DEBUG_LOG
        plat_log_i("uart_test", "rec data=%d", n);
#endif
        for(uint16_t i = 0U; i < n; i++)
        {
#ifdef USE_DEBUG_LOG
            proto_files_state_t prev_state = g_files_parser.state;
            uint8_t             prev_pack  = g_files_parser.package_number;
#endif
            proto_files_ret_t ret = proto_files_feed(&g_files_parser, buf[i]);

#ifdef USE_DEBUG_LOG
            if(PROTO_FILE_RET_OK != ret)
            {
                /* proto_files_feed already reset the parser back to IDLE by
                 * this point, so report the state it broke in, not the
                 * post-reset one. */
                plat_log_w("uart_test",
                           "proto_files err=%d in state=%d (reset)", ret,
                           prev_state);
                break;
            }
            else if(g_files_parser.state != prev_state)
            {
                plat_log_i("uart_test", "proto_files state %d -> %d",
                           prev_state, g_files_parser.state);
            }
            else if((PROTO_FILE_RECEIVE_DATA_INFO == prev_state) &&
                    (g_files_parser.package_number != prev_pack))
            {
                plat_log_i("uart_test", "proto_files data pack #%d ok",
                           g_files_parser.package_number);
            }
#endif
        }
    }
    // device_uart_send(DEVICE_UART_PROTO_1, buf, 7U,
    //                  SERVICE_UART_TEST_SEND_TIMEOUT_MS);
    device_uart_send(DEVICE_UART_PROTO_1, g_files_parser.buf, 7U,
                     SERVICE_UART_TEST_SEND_TIMEOUT_MS);
    device_uart_send(DEVICE_UART_PROTO_1, (uint8_t*)&g_files_parser.idx, 1U,
                     SERVICE_UART_TEST_SEND_TIMEOUT_MS);
        device_uart_send(DEVICE_UART_PROTO_1, (uint8_t*)&g_files_parser.state, 1U,
                     SERVICE_UART_TEST_SEND_TIMEOUT_MS);

    // for(uint16_t i = 0U; i < n; i++)
    // {
    //     /* idle and not our SOF: not a proto_simple frame, pass it through */
    //     if((PROTO_SIMPLE_ST_SOF == g_parser.state) &&
    //        (PROTO_SIMPLE_SOF != buf[i]))
    //     {
    //         if(PLATFORM_ERR_OK != device_uart_send(DEVICE_UART_PROTO_1,
    //         &buf[i], 1U,
    //                                                 SERVICE_UART_TEST_SEND_TIMEOUT_MS))
    //         {
    //             plat_log_w("uart_test", "raw echo send failed");
    //         }
    //         continue;
    //     }

    //     proto_simple_frame_t frame;
    //     if(1U == proto_simple_feed(&g_parser, buf[i], &frame))
    //     {
    //         if(PLATFORM_ERR_OK != device_uart_send(DEVICE_UART_PROTO_1,
    //         frame.payload,
    //                                                 frame.len,
    //                                                 SERVICE_UART_TEST_SEND_TIMEOUT_MS))
    //         {
    //             plat_log_w("uart_test", "frame reply send failed");
    //         }
    //     }
    // }
}

/* end of file --------------------------------------------------------------*/
