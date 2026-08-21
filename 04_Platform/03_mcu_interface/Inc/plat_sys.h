/**
 ******************************************************************************
 *@file               :   plat_sys.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef PLAT_SYS_H
#define PLAT_SYS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include "plat_error.h"
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
 * @brief Initialize the platform high-resolution time source.
 * @retval PLATFORM_ERR_OK on success; otherwise a hardware error.
 */
platform_err_t plat_time_init(void);

/**
 * @brief Get the current high-resolution timestamp in microseconds.
 * @note Call plat_time_init() before using this timestamp.
 * @retval Current timestamp in microseconds, with platform-defined wraparound.
 */
uint32_t plat_time_get_us(void);

/**
 * @brief Perform a blocking microsecond delay using the high-resolution timer.
 * @param us Delay duration in microseconds; zero returns immediately.
 */
void plat_delay_us(uint32_t us);

uint32_t plat_tick_get_ms(void);
void     plat_delay_ms(uint32_t ms);
void     plat_dcache_invalidate(void *p_addr, int32_t size);
#ifdef __cplusplus
}
#endif

#endif /* PLAT_SYS_H */
