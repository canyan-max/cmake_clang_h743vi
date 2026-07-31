/**
 ******************************************************************************
 *@file               :   device_camera.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef DEVICE_CAMERA_H
#define DEVICE_CAMERA_H

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

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
platform_err_t device_camera_init(void);
platform_err_t device_camera_start(void);
platform_err_t device_camera_stop(void);
uint8_t       *device_camera_get_buffer(void);
uint32_t       device_camera_get_buffer_size(void);
void           device_camera_frame_isr(void);
#ifdef __cplusplus
}
#endif

#endif /* DEVICE_CAMERA_H */
