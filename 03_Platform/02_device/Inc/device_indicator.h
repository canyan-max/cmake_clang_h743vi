/**
 ******************************************************************************
 *@file               :   device_indicator.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef DEVICE_INDICATOR_H
#define DEVICE_INDICATOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stdint lib header file. */
#include "plat_error.h"
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef enum
{
    DEVICE_INDICATOR_1 = 0,
    DEVICE_INDICATOR_2 = 1,
    DEVICE_INDICATOR_NUM
} device_indicator_id_t;

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
platform_err_t device_indicator_init(device_indicator_id_t id);
platform_err_t device_indicator_on(device_indicator_id_t id);
platform_err_t device_indicator_off(device_indicator_id_t id);
platform_err_t device_indicator_blink(device_indicator_id_t id);
#ifdef __cplusplus
}
#endif

#endif /* DEVICE_INDICATOR_H */
