/**
 ******************************************************************************
 *@file               :   bsp_rs485.h
 *@brief              :   Board-level RS485 byte-stream capabilities with an
 *                        internal automatic/manual direction policy.
 ******************************************************************************
 */
#ifndef BSP_RS485_H
#define BSP_RS485_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "plat_error.h"

typedef enum BSP_RS485_ID_T
{
    BSP_RS485_PORT_1 = 0U,
    BSP_RS485_PORT_2,
    BSP_RS485_NUM,
} bsp_rs485_id_t;

/** @brief Called from the UART ISR after the receive position is advanced. */
typedef void (*bsp_rs485_rx_notify_cb_t)(bsp_rs485_id_t id);

/** @brief Start circular DMA reception for one board RS485 endpoint. */
platform_err_t bsp_rs485_init(bsp_rs485_id_t id);

/**
 * @brief Enter TX state, send the complete byte sequence, then restore RX.
 * @note Direction switching is an internal board policy and may be a no-op
 *       for an automatically-directed transceiver.
 */
platform_err_t bsp_rs485_send(bsp_rs485_id_t  id,
                              const uint8_t *p_data,
                              uint16_t       size,
                              uint32_t       timeout_ms);

/**
 * @brief Copy available received bytes into a caller-owned buffer.
 * @note Task context only. Returns zero after discarding an overrun window.
 */
uint16_t bsp_rs485_read(bsp_rs485_id_t id, uint8_t *p_buf, uint16_t max_len);

/**
 * @brief Return bytes currently available from task context.
 * @note Returns zero after discarding an overrun window.
 */
uint16_t bsp_rs485_available(bsp_rs485_id_t id);

/** @brief Return cumulative RX bytes discarded after DMA ring overruns. */
uint32_t bsp_rs485_get_rx_dropped_bytes(bsp_rs485_id_t id);

/** @brief Register an ISR-safe receive notification callback. */
platform_err_t bsp_rs485_set_rx_notify_cb(bsp_rs485_id_t id,
                                          bsp_rs485_rx_notify_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* BSP_RS485_H */
