/**
 ******************************************************************************
 *@file               :   mcu_led.h
 *@brief              :   STM32 LED HAL ops instances declaration.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef ST_LED_H
#define ST_LED_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct LED_OPERATION_T led_operation_t;

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
extern const led_operation_t g_led1_ops;
extern const led_operation_t g_led2_ops;

/* functions ----------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ST_LED_H */
