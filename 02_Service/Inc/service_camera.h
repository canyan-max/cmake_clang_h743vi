/**
 ******************************************************************************
 *@file               :   service_camera.h
 *
 *@brief              :   Camera service — owns OV2640 handle, DMA buffer, and
 *                        provides application-level capture interface.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef SERVICE_CAMERA_H
#define SERVICE_CAMERA_H

#include <stdint.h>

#define SERVICE_CAMERA_BUF_SIZE  (115200U)

void         service_camera_init(void);
void         service_camera_start(void);
uint8_t     *service_camera_get_buffer(void);
uint32_t     service_camera_get_buffer_len(void);

#endif /* SERVICE_CAMERA_H */
