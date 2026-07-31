/**
 ******************************************************************************
 *@file               :   service_camera.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef SERVICE_CAMERA_H
#define SERVICE_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"
/* functions ----------------------------------------------------------------*/
platform_err_t service_camera_init(void);
platform_err_t service_camera_start(void);
uint8_t       *service_camera_get_buffer(void);
uint32_t       service_camera_get_buffer_size(void);
void           service_camera_frame_isr(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_CAMERA_H */
