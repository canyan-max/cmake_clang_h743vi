/**
 ******************************************************************************
 *@file               :   service_uart_test.c
 *@brief              :   Bring-up test: two protocols (proto_simple,
 *                        proto_files) share BSP_UART_PROTO_1. Whichever
 *                        parser is already mid-frame gets each byte
 *                        exclusively; while both are idle, every byte is
 *                        offered to both idle-scanners in parallel (safe
 *                        because neither has a side effect on mismatch),
 *                        and whichever one's start marker it matches locks
 *                        the stream until that frame finishes or errors.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h> /* NULL */
#include <stdint.h> /* UINT32_MAX for the idle-timeout wraparound calc */
#include "service_uart_test.h"
#include "bsp_uart.h"
#include "proto_simple.h"
#include "plat_log.h"
#include "plat_sys.h" /* plat_tick_get_ms: RTOS-independent tick, keeps the
                        * idle-timeout watchdog portable to bare metal */
#include "proto_files.h"
/* define   -----------------------------------------------------------------*/
#define SERVICE_UART_TEST_POLL_BUF_SIZE     (64U)
#define SERVICE_UART_TEST_SEND_TIMEOUT_MS   (100U)

/* variables ----------------------------------------------------------------*/
static proto_simple_parser_t g_parser;
static proto_files_parser_t  g_files_parser;
static uint32_t
    s_last_byte_tick;                         /* plat_tick_get_ms() snapshot,
                                                   for the mid-frame idle-timeout watchdog */
static service_uart_test_wake_cb_t s_wake_cb; /* caller-supplied, fired from
                                                  ISR context on new RX data */
/* private  functions  ------------------------------------------------------*/

/* Fired from ISR context (bsp_uart's DMA/IDLE handler) whenever new bytes
 * land in the RX fifo — just forwards to whatever wake mechanism the caller
 * wired up (RTOS task notify, bare-metal flag, ...); this module doesn't
 * know or care which. */
static void uart_rx_notify_cb(bsp_uart_id_t id)
{
    if((BSP_UART_PROTO_1 == id) && (NULL != s_wake_cb))
    {
        s_wake_cb();
    }
}

/* exported functions -------------------------------------------------------*/

platform_err_t service_uart_test_init(service_uart_test_wake_cb_t wake_cb)
{
#ifdef USE_DEBUG_LOG
    plat_log_i("uart_test", "service uart init");
#endif
    proto_simple_init(&g_parser);
    proto_files_init(&g_files_parser);
    s_wake_cb          = wake_cb;
    platform_err_t ret = bsp_uart_init(BSP_UART_PROTO_1);
    if(PLATFORM_ERR_OK != ret)
    {
        return ret;
    }
    return bsp_uart_set_rx_notify_cb(BSP_UART_PROTO_1, uart_rx_notify_cb);
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
    while((n = bsp_uart_read(BSP_UART_PROTO_1, buf,
                             SERVICE_UART_TEST_POLL_BUF_SIZE)) > 0U)
    {
        s_last_byte_tick = plat_tick_get_ms();
#ifdef USE_DEBUG_LOG
        plat_log_i("uart_test", "rec data=%d", n);
#endif
        for(uint16_t i = 0U; i < n; i++)
        {
            uint8_t byte          = buf[i];
            uint8_t simple_locked = (PROTO_SIMPLE_ST_SOF != g_parser.state);
            uint8_t files_locked  = (PROTO_FILE_IDLE != g_files_parser.state);

            /* proto_files gets this byte unless proto_simple already has
             * exclusive ownership of the stream. */
            if(!simple_locked)
            {
#ifdef USE_DEBUG_LOG
                proto_files_state_t prev_state = g_files_parser.state;
                uint8_t             prev_pack  = g_files_parser.package_number;
#endif
                proto_files_ret_t ret = proto_files_feed(&g_files_parser, byte);

#ifdef USE_DEBUG_LOG
                if(PROTO_FILE_RET_OK != ret)
                {
                    /* proto_files_feed already reset the parser back to IDLE
                     * by this point, so report the state it broke in, not
                     * the post-reset one. */
                    plat_log_w("uart_test",
                               "proto_files err=%d in state=%d (reset)", ret,
                               prev_state);
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

            /* proto_simple gets this byte unless proto_files already has
             * exclusive ownership of the stream. */
            if(!files_locked)
            {
                proto_simple_frame_t frame;
                if(1U == proto_simple_feed(&g_parser, byte, &frame))
                {
                    if(PLATFORM_ERR_OK !=
                       bsp_uart_send(BSP_UART_PROTO_1, frame.payload, frame.len,
                                     SERVICE_UART_TEST_SEND_TIMEOUT_MS))
                    {
#ifdef USE_DEBUG_LOG
                        plat_log_w("uart_test",
                                   "proto_simple reply send failed");
#endif
                    }
                }
            }
        }
    }

    /* Mid-frame idle-timeout watchdog: proto_files_feed only resets on a
     * content mismatch, never on "no more bytes ever arrived". If the sender
     * stalls or disconnects partway through a frame, the parser would
     * otherwise sit in a non-IDLE state forever. Force it back to IDLE once
     * too much silence has passed since the last accepted byte.
     *
     * Only fires while idx > 0 (truly mid-frame) or while matching the
     * "ESC" end-session marker. A multi-pack session legitimately pauses
     * between frames (sender computing/ACKing the next pack), and during
     * those pauses idx stays 0 waiting for the next pack header — that gap
     * must NOT be treated as a stall, or normal interaction gets cut off
     * mid-transfer. So frame-to-frame gaps are exempt; only a frame that
     * started (or an ESC that began) and then went silent times out.
     * plat_tick_get_ms() (not an RTOS tick) so this stays portable if the
     * service layer is ever moved to bare metal. */
    uint32_t now     = plat_tick_get_ms();
    uint32_t elapsed = (now >= s_last_byte_tick)
                           ? (now - s_last_byte_tick)
                           : (UINT32_MAX - s_last_byte_tick + now + 1U);
    if((PROTO_FILE_IDLE != g_files_parser.state) &&
       ((g_files_parser.idx > 0U) || /* mid-frame only; frame gaps are exempt */
        (PROTO_FILE_RECEIVE_END_SESSION == g_files_parser.state)) &&
       (elapsed > SERVICE_UART_TEST_IDLE_TIMEOUT_MS))
    {
#ifdef USE_DEBUG_LOG
        plat_log_w("uart_test", "proto_files idle timeout in state=%d (reset)",
                   g_files_parser.state);
#endif
        proto_files_init(&g_files_parser);
    }
}

/* Debug probes (read-only): expose the proto_files parser state so the
 * bring-up loopback echo can inspect buf/idx/state without handing out the
 * parser struct itself. */

const uint8_t *service_uart_test_files_buf(void)
{
    return g_files_parser.buf;
}

uint16_t service_uart_test_files_idx(void)
{
    return g_files_parser.idx;
}

uint8_t service_uart_test_files_state(void)
{
    return (uint8_t)g_files_parser.state;
}

/* end of file --------------------------------------------------------------*/
