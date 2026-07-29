/**
 ******************************************************************************
 *@file               :   service_camera.c
 *
 *@brief              :   Camera service implementation.
 *
 *@version            :   V2.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#include "service_camera.h"
#include "device_camera.h"

/* ---- local variables ---------------------------------------------------- */

__attribute__((section(".ram_d2_dma_buffers"),
               aligned(32))) static uint8_t g_camera_buffer[SERVICE_CAMERA_BUF_SIZE];

/* ---- public functions --------------------------------------------------- */

void service_camera_init(void)
{
    device_camera_init();
}

void service_camera_start(void)
{
    device_camera_start((uint32_t *)g_camera_buffer,
                        SERVICE_CAMERA_BUF_SIZE / sizeof(uint32_t));
}

uint8_t *service_camera_get_buffer(void)
{
    return g_camera_buffer;
}

uint32_t service_camera_get_buffer_len(void)
{
    return SERVICE_CAMERA_BUF_SIZE;
}
