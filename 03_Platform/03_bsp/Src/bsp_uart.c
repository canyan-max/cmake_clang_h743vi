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
#include "kfifo.h"

/* define   -----------------------------------------------------------------*/
#define BSP_UART_RX_BUF_SIZE  (256U)  /* power of 2 required (kfifo mask) */

/* typedef  -------------------------------------------------------------- */
typedef struct
{
    kfifo_t  rx_fifo;
    uint32_t last_pos; /* last DMA write position handled (0..BUF_SIZE-1) */
} uart_inst_t;

/* variables ----------------------------------------------------------------*/
static uart_inst_t g_uart_inst[PLAT_UART_NUM];

/* DMA writes straight into this buffer; it doubles as the kfifo's buffer.
 * PLAT_UART_PROTO_1 = LPUART1, whose RX DMA is BDMA_Channel0 — BDMA can only
 * reach RAM_D3 (SRAM4 @ 0x38000000), not RAM_D2. If a future instance uses
 * a D2-domain peripheral on DMA1/DMA2 instead, its buffer needs to move to
 * RAM_D2/RAM — this single shared array assumes one DMA domain for all
 * instances and will need revisiting then. */
__attribute__((section(".ram_d3_dma_buffers"), aligned(32))) static uint8_t
    g_rx_buf[PLAT_UART_NUM][BSP_UART_RX_BUF_SIZE];

/* private  functions  ------------------------------------------------------*/

/* size is the circular buffer's current absolute write position, as
 * reported by HAL on every HT / TC / IDLE event — not a byte count. */
static void uart_rx_event_handler(plat_uart_id_t id, uint16_t size)
{
    uart_inst_t *p_inst = &g_uart_inst[id];
    // uint32_t     new_bytes = (size >= p_inst->last_pos)
    //                              ? ((uint32_t)size - p_inst->last_pos)
    //                              : (BSP_UART_RX_BUF_SIZE - p_inst->last_pos +
    //                                 (uint32_t)size);
    uint16_t delta   = (uint16_t)((size - p_inst->last_pos) &
                                  (BSP_UART_RX_BUF_SIZE - 1U));
    p_inst->last_pos = size;
    /* RAM_D2/D3 is cacheable on this MCU config; DMA writes bypass the
     * cache, so the CPU must invalidate before reading what DMA wrote. */
    plat_dcache_invalidate(g_rx_buf[id], (int32_t)BSP_UART_RX_BUF_SIZE);
    if(delta > 0U)
    {
        kfifo_advance_in(&p_inst->rx_fifo, delta);
        p_inst->last_pos = size & (BSP_UART_RX_BUF_SIZE - 1U);
    }
}

/* exported functions -------------------------------------------------------*/

platform_err_t bsp_uart_init(plat_uart_id_t id)
{
    if(id >= PLAT_UART_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }

    uart_inst_t *p_inst = &g_uart_inst[id];
    p_inst->last_pos    = 0U;
    if(0U != kfifo_init(&p_inst->rx_fifo, g_rx_buf[id], BSP_UART_RX_BUF_SIZE))
    {
        return PLATFORM_ERR_PARAM;
    }

    platform_err_t ret = plat_uart_set_rx_callback(id, uart_rx_event_handler);
    if(PLATFORM_ERR_OK != ret)
    {
        return ret;
    }
    return plat_uart_receive_start(id, g_rx_buf[id], BSP_UART_RX_BUF_SIZE);
}

platform_err_t
bsp_uart_send(plat_uart_id_t id, const uint8_t *p_data, uint16_t size)
{
    if((id >= PLAT_UART_NUM) || (NULL == p_data))
    {
        return PLATFORM_ERR_PARAM;
    }
    return plat_uart_send(id, p_data, size);
}

uint16_t bsp_uart_read(plat_uart_id_t id, uint8_t *p_buf, uint16_t max_len)
{
    if((id >= PLAT_UART_NUM) || (NULL == p_buf))
    {
        return 0U;
    }
    return (uint16_t)kfifo_get(&g_uart_inst[id].rx_fifo, p_buf, max_len);
}

uint16_t bsp_uart_available(plat_uart_id_t id)
{
    if(id >= PLAT_UART_NUM)
    {
        return 0U;
    }
    return (uint16_t)kfifo_len(&g_uart_inst[id].rx_fifo);
}

/* end of file --------------------------------------------------------------*/
