/**
 ******************************************************************************
 *@file               :   bsp_uart.c
 *@brief              :   UART BSP — circular DMA + IDLE receive, written
 *                        directly into each instance's kfifo buffer (zero
 *                        copy: no memcpy in the RX event path). Board wiring
 *                        (id -> HAL handle) lives in mcu_uart.c, not here.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "bsp_uart.h"
#include "plat_uart.h"
#include "plat_sys.h"
#include "board_resources.h"
#include "kfifo.h"

/* define   -----------------------------------------------------------------*/
#define BSP_UART_RX_BUF_SIZE  (2048U)  /* power of 2 required (kfifo mask) */

/* typedef  -------------------------------------------------------------- */
typedef struct
{
    kfifo_t rx_fifo;
    /* last DMA write position handled (0..BUF_SIZE-1) */
    uint32_t                last_pos;
    bsp_uart_rx_notify_cb_t rx_notify_cb;
} uart_inst_t;

/* variables ----------------------------------------------------------------*/
static uart_inst_t          g_uart_inst[BSP_UART_NUM];
static const plat_uart_id_t s_uart_resources[BSP_UART_NUM] = {
    [BSP_UART_PROTO_1] = BOARD_UART_PROTOCOL_1,
};

/* DMA writes straight into this buffer; it doubles as the kfifo's buffer.
 * BOARD_UART_PROTOCOL_1 is bound to LPUART1, whose RX DMA is BDMA_Channel0.
 * BDMA can only reach RAM_D3 (SRAM4 @ 0x38000000), not RAM_D2. If a future
 * instance uses
 * a D2-domain peripheral on DMA1/DMA2 instead, its buffer needs to move to
 * RAM_D2/RAM — this single shared array assumes one DMA domain for all
 * instances and will need revisiting then. */
__attribute__((section(".ram_d3_dma_buffers"), aligned(32))) static uint8_t
    g_rx_buf[BSP_UART_NUM][BSP_UART_RX_BUF_SIZE];

/* private  functions  ------------------------------------------------------*/

/* size is the circular buffer's current absolute write position, as
 * reported by HAL on every HT / TC / IDLE event — not a byte count. */
static void uart_rx_event_handler(plat_uart_id_t id, uint16_t size)
{
    bsp_uart_id_t bsp_id;
    for(bsp_id = BSP_UART_PROTO_1; bsp_id < BSP_UART_NUM; bsp_id++)
    {
        if(s_uart_resources[bsp_id] == id)
        {
            break;
        }
    }
    if(bsp_id >= BSP_UART_NUM)
    {
        return;
    }

    uart_inst_t *p_inst = &g_uart_inst[bsp_id];
    // uint32_t     new_bytes = (size >= p_inst->last_pos)
    //                              ? ((uint32_t)size - p_inst->last_pos)
    //                              : (BSP_UART_RX_BUF_SIZE - p_inst->last_pos +
    //                                 (uint32_t)size);
    uint16_t delta   = (uint16_t)((size - p_inst->last_pos) &
                                  (BSP_UART_RX_BUF_SIZE - 1U));
    p_inst->last_pos = size;
    /* RAM_D2/D3 is cacheable on this MCU config; DMA writes bypass the
     * cache, so the CPU must invalidate before reading what DMA wrote. */
    plat_dcache_invalidate(g_rx_buf[bsp_id], (int32_t)BSP_UART_RX_BUF_SIZE);
    if(delta > 0U)
    {
        kfifo_advance_in(&p_inst->rx_fifo, delta);
        p_inst->last_pos = size & (BSP_UART_RX_BUF_SIZE - 1U);
        if(NULL != p_inst->rx_notify_cb)
        {
            p_inst->rx_notify_cb(bsp_id);
        }
    }
}

/* exported functions -------------------------------------------------------*/

platform_err_t bsp_uart_init(bsp_uart_id_t id)
{
    if(id >= BSP_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }

    uart_inst_t *p_inst = &g_uart_inst[id];
    p_inst->last_pos    = 0U;
    if(0U != kfifo_init(&p_inst->rx_fifo, g_rx_buf[id], BSP_UART_RX_BUF_SIZE))
    {
        return PLATFORM_ERR_PARAM;
    }

    platform_err_t ret = plat_uart_set_rx_callback(s_uart_resources[id],
                                                   uart_rx_event_handler);
    if(PLATFORM_ERR_OK != ret)
    {
        return ret;
    }
    return plat_uart_receive_start(s_uart_resources[id], g_rx_buf[id],
                                   BSP_UART_RX_BUF_SIZE);
}

platform_err_t bsp_uart_send(bsp_uart_id_t  id,
                             const uint8_t *p_data,
                             uint16_t       size,
                             uint32_t       timeout_ms)
{
    if((id >= BSP_UART_NUM) || (NULL == p_data))
    {
        return PLATFORM_ERR_PARAM;
    }
    return plat_uart_send(s_uart_resources[id], p_data, size, timeout_ms);
}

uint16_t bsp_uart_read(bsp_uart_id_t id, uint8_t *p_buf, uint16_t max_len)
{
    if((id >= BSP_UART_NUM) || (NULL == p_buf))
    {
        return 0U;
    }
    return (uint16_t)kfifo_get(&g_uart_inst[id].rx_fifo, p_buf, max_len);
}

uint16_t bsp_uart_available(bsp_uart_id_t id)
{
    if(id >= BSP_UART_NUM)
    {
        return 0U;
    }
    return (uint16_t)kfifo_len(&g_uart_inst[id].rx_fifo);
}

platform_err_t bsp_uart_set_rx_notify_cb(bsp_uart_id_t           id,
                                         bsp_uart_rx_notify_cb_t cb)
{
    if(id >= BSP_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    g_uart_inst[id].rx_notify_cb = cb;
    return PLATFORM_ERR_OK;
}

/* end of file --------------------------------------------------------------*/
