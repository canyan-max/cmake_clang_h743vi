/**
 ******************************************************************************
 *@file               :   bsp_key.h
 *@brief              :   Key BSP — raw press state by board index.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_KEY_H
#define BSP_KEY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"

/* functions ----------------------------------------------------------------*/
/**
 * @brief Read the normalized pressed state of one fitted board key.
 * @param[in] id Board key index in the range returned by bsp_key_count().
 * @param[out] p_pressed Receives 1 when pressed, otherwise 0.
 * @retval PLATFORM_ERR_OK on success; PLATFORM_ERR_PARAM for invalid input;
 *         otherwise the GPIO read error.
 */
platform_err_t bsp_key_is_pressed(uint8_t id, uint8_t *p_pressed);

/**
 * @brief Return the number of fitted board keys.
 * @retval Number of keys addressable by bsp_key_is_pressed().
 */
uint8_t bsp_key_count(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_KEY_H */
