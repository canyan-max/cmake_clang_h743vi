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
#include "plat_resource.h"

/* define -------------------------------------------------------------------*/
/* typedef ------------------------------------------------------------------*/
/** @brief Opaque I2C resource identifier assigned by the active Board. */
typedef plat_resource_id_t plat_i2c_id_t;

typedef enum PLAT_I2C_MEMORY_ADDRESS_SIZE_T
{
    PLAT_I2C_MEMORY_ADDRESS_SIZE_8BIT  = 1U,
    PLAT_I2C_MEMORY_ADDRESS_SIZE_16BIT = 2U,
} plat_i2c_memory_address_size_t;

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
platform_err_t plat_i2c_write(plat_i2c_id_t  id,
                              uint8_t        address_7b,
                              const uint8_t *p_data,
                              uint16_t       size,
                              uint32_t       timeout_ms);

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
                             uint8_t       address_7b,
                             uint8_t      *p_data,
                             uint16_t      size,
                             uint32_t      timeout_ms);

/**
 * @brief Write bytes to a device memory address in blocking mode.
 * @param id Logical I2C controller ID.
 * @param address_7b Unshifted 7-bit device address.
 * @param memory_address Device-internal memory address.
 * @param memory_address_size Address width transferred before the data.
 * @param p_data Bytes to write.
 * @param size Number of bytes to write; must be non-zero.
 * @param timeout_ms Finite transaction timeout in milliseconds.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t
plat_i2c_memory_write(plat_i2c_id_t                  id,
                      uint8_t                        address_7b,
                      uint16_t                       memory_address,
                      plat_i2c_memory_address_size_t memory_address_size,
                      const uint8_t                 *p_data,
                      uint16_t                       size,
                      uint32_t                       timeout_ms);

/**
 * @brief Read bytes from a device memory address in blocking mode.
 * @param id Logical I2C controller ID.
 * @param address_7b Unshifted 7-bit device address.
 * @param memory_address Device-internal memory address.
 * @param memory_address_size Address width transferred before the data.
 * @param p_data Destination buffer.
 * @param size Number of bytes to read; must be non-zero.
 * @param timeout_ms Finite transaction timeout in milliseconds.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t
plat_i2c_memory_read(plat_i2c_id_t                  id,
                     uint8_t                        address_7b,
                     uint16_t                       memory_address,
                     plat_i2c_memory_address_size_t memory_address_size,
                     uint8_t                       *p_data,
                     uint16_t                       size,
                     uint32_t                       timeout_ms);

/**
 * @brief Perform one bounded address-acknowledge probe.
 * @param id Logical I2C controller ID.
 * @param address_7b Unshifted 7-bit device address.
 * @param timeout_ms Finite probe timeout in milliseconds.
 * @retval PLATFORM_ERR_OK when acknowledged, PLATFORM_ERR_BUSY on NACK, or
 *         another stable Platform error.
 */
platform_err_t
plat_i2c_is_ready(plat_i2c_id_t id, uint8_t address_7b, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_I2C_H */
