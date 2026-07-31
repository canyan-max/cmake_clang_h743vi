/**
 ******************************************************************************
 *@file               :   device_key.h
 *@brief              :   Device key — debounce and press-event abstraction.
 *                        No BSP types exposed to callers.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef DEVICE_KEY_H
#define DEVICE_KEY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "plat_error.h"

/* ---- key id -------------------------------------------------------------- */
typedef enum
{
    DEVICE_KEY_1 = 0,
    DEVICE_KEY_2 = 1,
    DEVICE_KEY_NUM
} device_key_id_t;

/* ---- events (what the caller cares about) -------------------------------- */
typedef enum
{
    DEVICE_KEY_EVT_NONE        = 0x00U,
    DEVICE_KEY_EVT_SHORT_PRESS = 0x01U,
    DEVICE_KEY_EVT_LONG_PRESS  = 0x02U,
} device_key_event_t;

/* ---- API ----------------------------------------------------------------- */
platform_err_t     device_key_init(device_key_id_t id);
device_key_event_t device_key_get_event(device_key_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_KEY_H */
