/**
 ******************************************************************************
 *@file               :   at24.h
 *@brief              :   Portable AT24 EEPROM driver contract.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef AT24_H
#define AT24_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>

/* define -------------------------------------------------------------------*/
#define AT24_MAX_PAGE_SIZE_BYTES  (8U)
#define AT24_MAX_WORD_ADDR_BYTES  (1U)

/* typedef ------------------------------------------------------------------*/
typedef enum AT24_STATUS_T
{
    AT24_STATUS_OK = 0U,
    AT24_STATUS_PARAM,
    AT24_STATUS_NOT_INITIALIZED,
    AT24_STATUS_RANGE,
    AT24_STATUS_BUSY,
    AT24_STATUS_TIMEOUT,
    AT24_STATUS_TRANSPORT,
} at24_status_t;

typedef enum AT24_MODEL_T
{
    AT24_MODEL_C02 = 0U,
    AT24_MODEL_NUM,
} at24_model_t;

typedef struct AT24_TRANSPORT_T
{
    at24_status_t (*pf_write)(void *p_context,
                              uint8_t address_7b,
                              const uint8_t *p_data,
                              uint16_t size,
                              uint32_t timeout_ms);
    at24_status_t (*pf_write_read)(void *p_context,
                                   uint8_t address_7b,
                                   const uint8_t *p_tx_data,
                                   uint16_t tx_size,
                                   uint8_t *p_rx_data,
                                   uint16_t rx_size,
                                   uint32_t timeout_ms);
    at24_status_t (*pf_wait_ready)(void *p_context,
                                   uint8_t address_7b,
                                   uint32_t timeout_ms);
} at24_transport_t;

typedef struct AT24_T
{
    const at24_transport_t *p_transport;
    void                   *p_transport_context;
    at24_model_t            model;
    uint8_t                 address_7b;
    uint8_t                 initialized;
    uint8_t work_buffer[AT24_MAX_WORD_ADDR_BYTES +
                        AT24_MAX_PAGE_SIZE_BYTES];
} at24_t;

/* function  ----------------------------------------------------------------*/
/**
 * @brief Initialize one AT24 instance from its model and transport dependency.
 * @param p_dev Instance storage owned by the caller.
 * @param model Supported AT24 model; model specifications remain internal.
 * @param p_transport Immutable host transport implementation.
 * @param p_transport_context Opaque context returned to transport calls.
 * @param address_7b Unshifted 7-bit I2C address.
 * @retval AT24_STATUS_OK on success; otherwise a parameter or transport error.
 */
at24_status_t at24_init(at24_t *p_dev,
                        at24_model_t model,
                        const at24_transport_t *p_transport,
                        void *p_transport_context,
                        uint8_t address_7b);

/**
 * @brief Read a bounded byte range from an initialized AT24 instance.
 * @param p_dev Initialized AT24 instance.
 * @param memory_address First byte address.
 * @param p_data Destination buffer.
 * @param size Number of bytes to read.
 * @param timeout_ms Finite timeout for each underlying bus transaction.
 * @retval AT24_STATUS_OK on success; otherwise a validation, timeout, or
 *         transport error.
 */
at24_status_t at24_read(at24_t *p_dev,
                        uint32_t memory_address,
                        uint8_t *p_data,
                        uint32_t size,
                        uint32_t timeout_ms);

/**
 * @brief Write a bounded byte range, splitting it at page boundaries and
 *        waiting for every page-write cycle to complete.
 * @param p_dev Initialized AT24 instance.
 * @param memory_address First byte address.
 * @param p_data Source buffer.
 * @param size Number of bytes to write.
 * @param timeout_ms Finite timeout for each underlying bus transaction.
 * @retval AT24_STATUS_OK on success; otherwise a validation, timeout, or
 *         transport error.
 */
at24_status_t at24_write(at24_t *p_dev,
                         uint32_t memory_address,
                         const uint8_t *p_data,
                         uint32_t size,
                         uint32_t timeout_ms);

/**
 * @brief Wait a bounded time for an initialized AT24 instance to acknowledge.
 * @param p_dev Initialized AT24 instance.
 * @retval AT24_STATUS_OK when ready; otherwise the translated chip status.
 */
at24_status_t at24_is_ready(at24_t *p_dev);

#ifdef __cplusplus
}
#endif

#endif /* AT24_H */
