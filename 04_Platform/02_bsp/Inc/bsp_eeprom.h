/**
 ******************************************************************************
 *@file               :   bsp_eeprom.h
 *@brief              :   Board EEPROM capability backed by the fitted chip.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_EEPROM_H
#define BSP_EEPROM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"

/* function  ----------------------------------------------------------------*/
/**
 * @brief Initialize and probe the board EEPROM instance.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_eeprom_init(void);

/**
 * @brief Read bytes from the board EEPROM.
 * @param memory_address First EEPROM byte address.
 * @param p_data Destination buffer.
 * @param size Number of bytes to read.
 * @param timeout_ms Finite timeout for each I2C transaction.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_eeprom_read(uint16_t memory_address,
                               uint8_t *p_data,
                               uint16_t size,
                               uint32_t timeout_ms);

/**
 * @brief Write bytes to the board EEPROM, including cross-page writes.
 * @param memory_address First EEPROM byte address.
 * @param p_data Source buffer.
 * @param size Number of bytes to write.
 * @param timeout_ms Finite timeout for each I2C transaction.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_eeprom_write(uint16_t memory_address,
                                const uint8_t *p_data,
                                uint16_t size,
                                uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* BSP_EEPROM_H */
