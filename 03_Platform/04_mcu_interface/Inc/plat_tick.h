/**
 ******************************************************************************
 *@file               :   plat_tick.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef PLAT_TICK_H
#define PLAT_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */
#include <stddef.h>                               /* stdint lib header file. */
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
uint32_t plat_tick_get_ms(void);
void plat_delay_ms(uint32_t ms);
#ifdef __cplusplus
}
#endif

#endif /* PLAT_TICK_H */

