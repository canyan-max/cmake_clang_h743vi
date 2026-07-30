/**
 ******************************************************************************
 *@file               :   service_indicator.h
 *@brief              :   Provide the service indicator APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef SERVICE_INDICATOR_H
#define SERVICE_INDICATOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include "device_indicator.h"
#include "platform_error.h"

/* functions ----------------------------------------------------------------*/
platform_err_t service_indicator_init(void);
platform_err_t service_indicator_on(device_indicator_id_t id);
platform_err_t service_indicator_off(device_indicator_id_t id);
platform_err_t service_indicator_blink(device_indicator_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_INDICATOR_H */
