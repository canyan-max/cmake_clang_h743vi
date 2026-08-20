/**
 ******************************************************************************
 *@file               :   at24cxx_driver.h
 *@brief              :   Portable AT24Cxx EEPROM driver contract.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef AT24CXX_DRIVER_H
#define AT24CXX_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef enum AT24CXX_STATUS_T
{
    AT24CXX_STATUS_OK = 0U,
    AT24CXX_STATUS_PARAM,
    AT24CXX_STATUS_NOT_INITIALIZED,
    AT24CXX_STATUS_RANGE,
    AT24CXX_STATUS_BUSY,
    AT24CXX_STATUS_TIMEOUT,
    AT24CXX_STATUS_TRANSPORT,
} at24cxx_status_t;

typedef enum AT24CXX_MODEL_T
{
    AT24CXX_MODEL_C02 = 0U,
    AT24CXX_MODEL_NUM,
} at24cxx_model_t;

typedef struct AT24CXX_TRANSPORT_T
{
    at24cxx_status_t (*pf_write)(uint8_t        address_7b,
                                 uint16_t       word_address,
                                 uint8_t        word_address_size_bytes,
                                 const uint8_t *p_data,
                                 uint16_t       size,
                                 uint32_t       timeout_ms);
    at24cxx_status_t (*pf_read)(uint8_t  address_7b,
                                uint16_t word_address,
                                uint8_t  word_address_size_bytes,
                                uint8_t *p_data,
                                uint16_t size,
                                uint32_t timeout_ms);
    at24cxx_status_t (*pf_wait_ready)(uint8_t address_7b, uint32_t timeout_ms);
} at24cxx_transport_t;

typedef struct AT24CXX_DRIVER_T
{
    const at24cxx_transport_t *p_transport;
    uint32_t                   capacity_bytes;
    uint16_t                   page_size_bytes;
    at24cxx_model_t            model;
    uint8_t                    address_7b;
    uint8_t                    initialized;
} at24cxx_driver_t;

/* variables ----------------------------------------------------------------*/

/* function  ----------------------------------------------------------------*/
/**
 * @brief Initialize one AT24Cxx instance with chip and board properties.
 * @param p_driver Instance storage owned by the caller.
 * @param p_transport Immutable host transport implementation.
 * @param model AT24Cxx model used to select its addressing rule.
 * @param address_7b Unshifted base 7-bit I2C address.
 * @param capacity_bytes Accessible EEPROM capacity in bytes.
 * @param page_size_bytes Page-write size in bytes.
 * @param timeout_ms Finite timeout for the initial ready check.
 * @retval AT24CXX_STATUS_OK on success; otherwise an AT24Cxx status.
 */
at24cxx_status_t at24cxx_init(at24cxx_driver_t          *p_driver,
                              const at24cxx_transport_t *p_transport,
                              at24cxx_model_t            model,
                              uint8_t                    address_7b,
                              uint32_t                   capacity_bytes,
                              uint16_t                   page_size_bytes,
                              uint32_t                   timeout_ms);

/**
 * @brief Read a bounded byte range from an initialized AT24Cxx instance.
 * @param p_driver Initialized AT24Cxx instance.
 * @param memory_address First byte address.
 * @param p_data Destination buffer.
 * @param size Number of bytes to read.
 * @param timeout_ms Finite timeout for each underlying transaction.
 * @retval AT24CXX_STATUS_OK on success; otherwise an AT24Cxx status.
 */
at24cxx_status_t at24cxx_read(at24cxx_driver_t *p_driver,
                              uint32_t          memory_address,
                              uint8_t          *p_data,
                              uint32_t          size,
                              uint32_t          timeout_ms);

/**
 * @brief Write a bounded byte range and split it at page boundaries.
 * @param p_driver Initialized AT24Cxx instance.
 * @param memory_address First byte address.
 * @param p_data Source buffer.
 * @param size Number of bytes to write.
 * @param timeout_ms Finite timeout for each transaction and ready wait.
 * @retval AT24CXX_STATUS_OK on success; otherwise an AT24Cxx status.
 */
at24cxx_status_t at24cxx_write(at24cxx_driver_t *p_driver,
                               uint32_t          memory_address,
                               const uint8_t    *p_data,
                               uint32_t          size,
                               uint32_t          timeout_ms);

/**
 * @brief Wait a bounded time for an initialized AT24Cxx to acknowledge.
 * @param p_driver Initialized AT24Cxx instance.
 * @param timeout_ms Finite ready-wait timeout.
 * @retval AT24CXX_STATUS_OK when ready; otherwise an AT24Cxx status.
 */
at24cxx_status_t at24cxx_is_ready(at24cxx_driver_t *p_driver,
                                  uint32_t          timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* AT24CXX_DRIVER_H */

/* end of file --------------------------------------------------------------*/
