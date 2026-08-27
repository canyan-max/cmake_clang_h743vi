/**
 ******************************************************************************
 *@file               :   bsp_rs485.c
 *@brief              :   Board RS485 ports using DMA circular reception and
 *                        an internal automatic/GPIO direction policy.
 ******************************************************************************
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdatomic.h>
#include "bsp_rs485.h"
#include "board_resources.h"
#include "kfifo.h"
#include "plat_gpio.h"
#include "plat_sys.h"
#include "plat_uart.h"

#define BSP_RS485_RX_BUF_SIZE (2048U)

typedef struct
{
    kfifo_t rx_fifo;
    /* Updated only from the DMA callback; the task never changes dma_pos. */
    uint16_t dma_pos;
    uint8_t initialized;
    atomic_bool tx_busy;
    uint32_t rx_dropped_bytes;
    bsp_rs485_rx_notify_cb_t volatile rx_notify_cb;
} rs485_inst_t;

typedef enum
{
    RS485_DIRECTION_STATE_RX = 0U,
    RS485_DIRECTION_STATE_TX,
} rs485_direction_state_t;

typedef struct
{
    board_rs485_direction_mode_t mode;
    plat_gpio_id_t gpio_id;
    uint8_t tx_level;
    uint8_t rx_level;
} rs485_direction_config_t;

static rs485_inst_t g_rs485_inst[BSP_RS485_NUM];
static const plat_uart_id_t s_uart_resources[BSP_RS485_NUM] = {
    [BSP_RS485_PORT_1] = BOARD_UART_RS485_1,
    [BSP_RS485_PORT_2] = BOARD_UART_RS485_2,
};
static const rs485_direction_config_t
    s_direction_config[BSP_RS485_NUM] = {
        [BSP_RS485_PORT_1] = {
            .mode = BOARD_RS485_1_DIRECTION_MODE,
            .gpio_id = BOARD_RS485_1_DIRECTION_GPIO,
            .tx_level = BOARD_RS485_1_DIRECTION_TX_LEVEL,
            .rx_level = BOARD_RS485_1_DIRECTION_RX_LEVEL,
        },
        [BSP_RS485_PORT_2] = {
            .mode = BOARD_RS485_2_DIRECTION_MODE,
            .gpio_id = BOARD_RS485_2_DIRECTION_GPIO,
            .tx_level = BOARD_RS485_2_DIRECTION_TX_LEVEL,
            .rx_level = BOARD_RS485_2_DIRECTION_RX_LEVEL,
        },
};

/* UART7 and UART8 use DMA1 in the D2 domain. Keeping these buffers separate
 * from bsp_uart's LPUART1/BDMA D3 buffer is required for DMA reachability. */
__attribute__((section(".ram_d2_dma_buffers"), aligned(32))) static uint8_t
    g_rs485_rx_buf[BSP_RS485_NUM][BSP_RS485_RX_BUF_SIZE];

_Static_assert((BSP_RS485_RX_BUF_SIZE & (BSP_RS485_RX_BUF_SIZE - 1U)) == 0U,
               "RS485 RX buffer must be a power of two");
_Static_assert((sizeof(s_uart_resources) / sizeof(s_uart_resources[0])) ==
                   BSP_RS485_NUM,
               "RS485 UART table size mismatch");
_Static_assert((sizeof(s_direction_config) /
                sizeof(s_direction_config[0])) == BSP_RS485_NUM,
               "RS485 direction table size mismatch");

/**
 * @brief Validate one endpoint's direction-control configuration.
 * @note GPIO pin mode is configured by the generated board startup code.
 */
static platform_err_t rs485_direction_init(bsp_rs485_id_t id)
{
    if(id >= BSP_RS485_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    const rs485_direction_config_t *p_config = &s_direction_config[id];
    switch(p_config->mode)
    {
    case BOARD_RS485_DIRECTION_AUTO:
        return PLATFORM_ERR_OK;
    case BOARD_RS485_DIRECTION_GPIO:
        if((p_config->gpio_id >= BOARD_GPIO_RESOURCE_COUNT) ||
           (p_config->tx_level > 1U) || (p_config->rx_level > 1U) ||
           (p_config->tx_level == p_config->rx_level))
        {
            return PLATFORM_ERR_PARAM;
        }
        return PLATFORM_ERR_OK;
    default:
        return PLATFORM_ERR_PARAM;
    }
}

/** @brief Apply the semantic RX/TX state using the Board direction policy. */
static platform_err_t rs485_direction_set(bsp_rs485_id_t id,
                                           rs485_direction_state_t state)
{
    if((id >= BSP_RS485_NUM) || (state > RS485_DIRECTION_STATE_TX))
    {
        return PLATFORM_ERR_PARAM;
    }
    const rs485_direction_config_t *p_config = &s_direction_config[id];
    switch(p_config->mode)
    {
    case BOARD_RS485_DIRECTION_AUTO:
        return PLATFORM_ERR_OK;
    case BOARD_RS485_DIRECTION_GPIO:
        return plat_gpio_write(p_config->gpio_id,
                               (RS485_DIRECTION_STATE_TX == state)
                                   ? p_config->tx_level
                                   : p_config->rx_level);
    default:
        return PLATFORM_ERR_PARAM;
    }
}

static platform_err_t rs485_enter_tx(bsp_rs485_id_t id)
{
    return rs485_direction_set(id, RS485_DIRECTION_STATE_TX);
}

static platform_err_t rs485_enter_rx(bsp_rs485_id_t id)
{
    return rs485_direction_set(id, RS485_DIRECTION_STATE_RX);
}

static bsp_rs485_id_t rs485_find_id(plat_uart_id_t uart_id)
{
    for(uint8_t i = 0U; i < (uint8_t)BSP_RS485_NUM; i++)
    {
        if(s_uart_resources[i] == uart_id)
        {
            return (bsp_rs485_id_t)i;
        }
    }
    return BSP_RS485_NUM;
}

/**
 * @brief Detect DMA overwrite and discard the damaged receive window.
 * @note Task context only. fifo.in remains ISR-owned and fifo.out remains
 *       consumer-owned, including while an RX interrupt occurs here.
 */
static bool rs485_recover_rx_overrun(rs485_inst_t *p_inst)
{
    uint32_t pending = kfifo_len(&p_inst->rx_fifo);
    if(pending <= BSP_RS485_RX_BUF_SIZE)
    {
        return false;
    }
    p_inst->rx_dropped_bytes += kfifo_discard_all(&p_inst->rx_fifo);
    return true;
}

/* HAL reports the current DMA write position in [1, buffer_size]. */
static void rs485_rx_event(plat_uart_id_t uart_id, uint16_t size)
{
    bsp_rs485_id_t id = rs485_find_id(uart_id);
    if((id >= BSP_RS485_NUM) || (size > BSP_RS485_RX_BUF_SIZE))
    {
        return;
    }

    uint16_t new_pos = (size == BSP_RS485_RX_BUF_SIZE) ? 0U : size;
    rs485_inst_t *p_inst = &g_rs485_inst[id];
    uint16_t old_pos = p_inst->dma_pos;
    uint16_t delta = (uint16_t)((new_pos - old_pos) &
                                (BSP_RS485_RX_BUF_SIZE - 1U));
    if(0U == delta)
    {
        return;
    }

    p_inst->dma_pos = new_pos;
    /* Always commit the complete DMA delta so fifo.in stays physically
     * aligned with the DMA ring. The task detects and discards overruns. */
    (void)kfifo_advance_in(&p_inst->rx_fifo, delta);
    bsp_rs485_rx_notify_cb_t notify_cb = p_inst->rx_notify_cb;
    if(NULL != notify_cb)
    {
        notify_cb(id);
    }
}

platform_err_t bsp_rs485_init(bsp_rs485_id_t id)
{
    if(id >= BSP_RS485_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }

    rs485_inst_t *p_inst = &g_rs485_inst[id];
    if(0U != p_inst->initialized)
    {
        return PLATFORM_ERR_OK;
    }
    p_inst->dma_pos = 0U;
    p_inst->rx_dropped_bytes = 0U;
    atomic_init(&p_inst->tx_busy, false);
    if(0U != kfifo_init(&p_inst->rx_fifo, g_rs485_rx_buf[id],
                        BSP_RS485_RX_BUF_SIZE))
    {
        return PLATFORM_ERR_PARAM;
    }

    platform_err_t status = rs485_direction_init(id);
    if(PLATFORM_ERR_OK == status)
    {
        status = rs485_enter_rx(id);
    }
    if(PLATFORM_ERR_OK == status)
    {
        status = plat_uart_set_rx_callback(s_uart_resources[id],
                                            rs485_rx_event);
    }
    if(PLATFORM_ERR_OK == status)
    {
        status = plat_uart_receive_start(s_uart_resources[id],
                                         g_rs485_rx_buf[id],
                                         BSP_RS485_RX_BUF_SIZE);
    }
    if(PLATFORM_ERR_OK == status)
    {
        p_inst->initialized = 1U;
    }
    return status;
}

platform_err_t bsp_rs485_send(bsp_rs485_id_t id,
                              const uint8_t *p_data,
                              uint16_t size,
                              uint32_t timeout_ms)
{
    if((id >= BSP_RS485_NUM) || (NULL == p_data) || (0U == size) ||
       (0U == g_rs485_inst[id].initialized))
    {
        return PLATFORM_ERR_PARAM;
    }

    rs485_inst_t *p_inst = &g_rs485_inst[id];
    bool expected = false;
    if(!atomic_compare_exchange_strong_explicit(
           &p_inst->tx_busy, &expected, true, memory_order_acquire,
           memory_order_relaxed))
    {
        return PLATFORM_ERR_BUSY;
    }

    platform_err_t send_status = rs485_enter_tx(id);
    if(PLATFORM_ERR_OK == send_status)
    {
        /* plat_uart_send() is blocking through UART TC. A manual-DE policy
         * may therefore restore RX immediately after this call. */
        send_status = plat_uart_send(s_uart_resources[id], p_data, size,
                                     timeout_ms);
    }
    /* Always attempt to restore the receiver, including on TX timeout/error. */
    platform_err_t rx_status = rs485_enter_rx(id);
    atomic_store_explicit(&p_inst->tx_busy, false, memory_order_release);
    return (PLATFORM_ERR_OK != send_status) ? send_status : rx_status;
}

uint16_t bsp_rs485_read(bsp_rs485_id_t id, uint8_t *p_buf, uint16_t max_len)
{
    if((id >= BSP_RS485_NUM) || (NULL == p_buf) || (0U == max_len) ||
       (0U == g_rs485_inst[id].initialized))
    {
        return 0U;
    }

    rs485_inst_t *p_inst = &g_rs485_inst[id];
    /* DMA writes bypass the M7 cache. The whole aligned ring is invalidated
     * once per read; this favors correctness over a more complex split-range
     * cache operation and is acceptable for the current short RTU frames. */
    plat_dcache_invalidate(g_rs485_rx_buf[id],
                           (int32_t)BSP_RS485_RX_BUF_SIZE);
    if(rs485_recover_rx_overrun(p_inst))
    {
        return 0U;
    }
    return (uint16_t)kfifo_get(&p_inst->rx_fifo, p_buf, max_len);
}

uint16_t bsp_rs485_available(bsp_rs485_id_t id)
{
    if((id >= BSP_RS485_NUM) || (0U == g_rs485_inst[id].initialized))
    {
        return 0U;
    }
    rs485_inst_t *p_inst = &g_rs485_inst[id];
    if(rs485_recover_rx_overrun(p_inst))
    {
        return 0U;
    }
    return (uint16_t)kfifo_len(&p_inst->rx_fifo);
}

uint32_t bsp_rs485_get_rx_dropped_bytes(bsp_rs485_id_t id)
{
    if((id >= BSP_RS485_NUM) || (0U == g_rs485_inst[id].initialized))
    {
        return 0U;
    }
    return g_rs485_inst[id].rx_dropped_bytes;
}

platform_err_t bsp_rs485_set_rx_notify_cb(bsp_rs485_id_t id,
                                          bsp_rs485_rx_notify_cb_t cb)
{
    if(id >= BSP_RS485_NUM)
    {
        return PLATFORM_ERR_PARAM;
    }
    g_rs485_inst[id].rx_notify_cb = cb;
    return PLATFORM_ERR_OK;
}
