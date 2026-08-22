/**
 ******************************************************************************
 *@file               :   plat_spi.h
 *@brief              :   Portable bounded SPI transmit contract.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef PLAT_SPI_H
#define PLAT_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "plat_error.h"

typedef enum PLAT_SPI_ID_T
{
    PLAT_SPI_ID_0 = 0U,
    PLAT_SPI_ID_NUM,
} plat_spi_id_t;

/**
 * @brief SPI DMA transmit completion callback.
 * @param id Logical SPI controller that completed or failed.
 * @param status PLATFORM_ERR_OK on completion; otherwise a transfer error.
 * @param p_context Opaque caller context supplied when starting the transfer.
 * @note Invoked from interrupt context; it must not block or call non-ISR-safe
 *       services.
 */
typedef void (*plat_spi_tx_complete_cb_t)(plat_spi_id_t id,
                                          platform_err_t status,
                                          void          *p_context);

platform_err_t plat_spi_write(plat_spi_id_t  id,
                              const uint8_t *p_data,
                              uint32_t       size,
                              uint32_t       timeout_ms);

/**
 * @brief Start a non-blocking DMA transmit.
 * @param id Logical SPI controller ID.
 * @param p_data DMA-accessible source buffer.
 * @param size Number of bytes to transmit; may exceed one HAL DMA transfer.
 * @param callback Optional completion/error callback invoked from ISR context.
 * @param p_context Opaque value forwarded to callback.
 * @note The source must remain valid and unchanged until callback execution or
 *       an explicit plat_spi_abort_dma() call. D-Cache is cleaned before each
 *       DMA chunk. The logical transfer remains exclusively owned, but hardware
 *       chip select may toggle between internal chunks. The caller owns the
 *       overall operation timeout and may abort a stalled transfer.
 * @retval PLATFORM_ERR_OK when accepted; otherwise parameter, busy or hardware
 *         error. No callback is made when the request is rejected.
 */
platform_err_t
plat_spi_write_dma_async(plat_spi_id_t              id,
                         const uint8_t             *p_data,
                         uint32_t                   size,
                         plat_spi_tx_complete_cb_t callback,
                         void                      *p_context);

/**
 * @brief Abort an active asynchronous DMA transmit in caller context.
 * @note No completion callback is invoked for an explicit abort.
 * @retval PLATFORM_ERR_OK when stopped or already idle; otherwise an error.
 */
platform_err_t plat_spi_abort_dma(plat_spi_id_t id);

/**
 * @brief Query whether an asynchronous DMA transmit owns the controller.
 * @param id Logical SPI controller ID.
 * @param p_busy Destination: 1U when busy, otherwise 0U.
 * @retval PLATFORM_ERR_OK on success; PLATFORM_ERR_PARAM for invalid input.
 */
platform_err_t plat_spi_is_busy(plat_spi_id_t id, uint8_t *p_busy);

/**
 * @brief Transmit bytes with DMA and wait for bounded completion.
 * @note The source must be DMA-accessible and remain unchanged until return.
 *       This compatibility API uses the asynchronous DMA state machine and
 *       waits with one total timeout across all chunks.
 */
platform_err_t plat_spi_write_dma_blocking(plat_spi_id_t  id,
                                           const uint8_t *p_data,
                                           uint32_t       size,
                                           uint32_t       timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_SPI_H */
