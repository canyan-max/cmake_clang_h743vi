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
#include "platform_error.h"
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
platform_err_t device_camera_init(void);
platform_err_t device_camera_start(uint32_t *p_buf, uint32_t len_words);
platform_err_t device_camera_stop(void);
platform_err_t device_camera_register_frame_cb(void (*cb)(void));
#ifdef __cplusplus
}
#endif

#endif /* DEVICE_CAMERA_H */
