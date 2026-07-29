/**
 ******************************************************************************
 *@file               :   device_camera.h
 *
 *@brief              :   Camera device — owns OV2640 driver, exposes clean API.
 *
 *@version            :   V2.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef DEVICE_CAMERA_H
#define DEVICE_CAMERA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "platform_error.h"

platform_err_t device_camera_init(void);
platform_err_t device_camera_start(uint32_t *p_buf, uint32_t len_words);
platform_err_t device_camera_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_CAMERA_H */
