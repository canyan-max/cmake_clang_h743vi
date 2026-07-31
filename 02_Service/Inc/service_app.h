/**
 ******************************************************************************
 *@file               :   service_app.h
 *@brief              :   Top-level application service: init sequence and
 *                        per-frame business logic. No RTOS primitives here.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef SERVICE_APP_H
#define SERVICE_APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "plat_error.h"

platform_err_t service_app_init(void);
void           service_on_frame(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_APP_H */
