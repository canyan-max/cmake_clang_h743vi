/**
 ******************************************************************************
 *@file               :   service_camera.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "service_camera.h"
#include "device_camera.h"

/* exported functions -------------------------------------------------------*/
platform_err_t service_camera_init(void)
{
    return device_camera_init();
}

platform_err_t service_camera_start(void)
{
    return device_camera_start();
}

uint8_t *service_camera_get_buffer(void)
{
    return device_camera_get_buffer();
}

uint32_t service_camera_get_buffer_size(void)
{
    return device_camera_get_buffer_size();
}

void service_camera_frame_isr(void)
{
    device_camera_frame_isr();
}

/* end of  file -------------------------------------------------------------*/
