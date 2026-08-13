/**
 ******************************************************************************
 *@file               :   plat_i2c.h
 *@brief              :   Portable blocking I2C master transaction contract.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef PLAT_I2C_H
#define PLAT_I2C_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"

/* define -------------------------------------------------------------------*/
#define PLAT_I2C_WRITE_READ_PREFIX_MAX_SIZE  (2U)

/* typedef ------------------------------------------------------------------*/
typedef enum PLAT_I2C_ID_T
{
    PLAT_I2C_ID_0 = 0U,
    PLAT_I2C_ID_NUM,
} plat_i2c_id_t;

/* function  ----------------------------------------------------------------*/
/**
 * @brief Send bytes to a 7-bit-addressed I2C device in blocking mode.
 * @param id Logical I2C controller ID.
 * @param address_7b Unshifted 7-bit device address.
 * @param p_data Bytes to send.
 * @param size Number of bytes to send; must be non-zero.
 * @param timeout_ms Finite transaction timeout in milliseconds.
 * @retval PLATFORM_ERR_OK on success; otherwise a parameter, busy, timeout,
 *         or hardware error.
 */
platform_err_t plat_i2c_write(plat_i2c_id_t id,
                              uint8_t address_7b,
                              const uint8_t *p_data,
                              uint16_t size,
                              uint32_t timeout_ms);

/**
 * @brief Read bytes from a 7-bit-addressed I2C device in blocking mode.
 * @param id Logical I2C controller ID.
 * @param address_7b Unshifted 7-bit device address.
 * @param p_data Destination buffer.
 * @param size Number of bytes to read; must be non-zero.
 * @param timeout_ms Finite transaction timeout in milliseconds.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t plat_i2c_read(plat_i2c_id_t id,
                             uint8_t address_7b,
                             uint8_t *p_data,
                             uint16_t size,
                             uint32_t timeout_ms);

/**
 * @brief Atomically write a one- or two-byte prefix, issue repeated START,
 *        then read bytes from the same 7-bit-addressed device.
 * @note The write prefix is transferred most-significant byte first. No other
 *       I2C transaction may be inserted between the write and read phases.
 * @param id Logical I2C controller ID.
 * @param address_7b Unshifted 7-bit device address.
 * @param p_tx_data One- or two-byte prefix.
 * @param tx_size Prefix length in bytes.
 * @param p_rx_data Destination buffer.
 * @param rx_size Number of bytes to read; must be non-zero.
 * @param timeout_ms Finite timeout for the complete transaction.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t plat_i2c_write_read(plat_i2c_id_t id,
                                   uint8_t address_7b,
                                   const uint8_t *p_tx_data,
                                   uint16_t tx_size,
                                   uint8_t *p_rx_data,
                                   uint16_t rx_size,
                                   uint32_t timeout_ms);

/**
 * @brief Perform one bounded address-acknowledge probe.
 * @param id Logical I2C controller ID.
 * @param address_7b Unshifted 7-bit device address.
 * @param timeout_ms Finite probe timeout in milliseconds.
 * @retval PLATFORM_ERR_OK when acknowledged, PLATFORM_ERR_BUSY on NACK, or
 *         another stable Platform error.
 */
platform_err_t plat_i2c_is_ready(plat_i2c_id_t id,
                                 uint8_t address_7b,
                                 uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_I2C_H */
