/**
 ******************************************************************************
 *@file               :   device_indicator.h
 *
 *@brief              :   Indicator device — owns LED drivers, index-based multi-instance.
 *
 *@version            :   V2.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef DEVICE_INDICATOR_H
#define DEVICE_INDICATOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "platform_error.h"

/** @brief LED instance identifiers (board has two LEDs) */
typedef enum
{
    DEVICE_INDICATOR_1 = 0,
    DEVICE_INDICATOR_2 = 1,
    DEVICE_INDICATOR_NUM
} device_indicator_id_t;

platform_err_t device_indicator_init(device_indicator_id_t id, const void *p_led_ops);
platform_err_t device_indicator_on(device_indicator_id_t id);
platform_err_t device_indicator_off(device_indicator_id_t id);
platform_err_t device_indicator_blink(device_indicator_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_INDICATOR_H */
