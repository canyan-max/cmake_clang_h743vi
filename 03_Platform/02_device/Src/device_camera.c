/**
 ******************************************************************************
 *@file               :   device_camera.c
 *@brief              :   Provide the device of camera
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "device_camera.h"
#include "bsp_ov2640.h"
#include "plat_sys.h"

/* define   -----------------------------------------------------------------*/
#define DEVICE_CAM_BUF_SIZE  (OV2640_OUT_W * OV2640_OUT_H * 2U)

/* variables ----------------------------------------------------------------*/
__attribute__((section(".ram_d2_dma_buffers"), aligned(32)))
static uint8_t g_cam_buf[DEVICE_CAM_BUF_SIZE];

static ov2640_dev_t g_cam;

/* exported functions -------------------------------------------------------*/

platform_err_t device_camera_init(void)
{
    ov2640_state_t ret = bsp_ov2640_init(&g_cam, OV2640_MODE_SVGA);
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_camera_start(void)
{
    if(OV2640_DRIVER_IS_INIT != g_cam.is_init)
    {
        return PLATFORM_ERR_HW;
    }
    ov2640_state_t ret = ov2640_start(&g_cam, (uint32_t *)g_cam_buf,
                                       DEVICE_CAM_BUF_SIZE / sizeof(uint32_t));
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_camera_stop(void)
{
    if(OV2640_DRIVER_IS_INIT != g_cam.is_init)
    {
        return PLATFORM_ERR_HW;
    }
    ov2640_state_t ret = ov2640_stop(&g_cam);
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

uint8_t *device_camera_get_buffer(void)
{
    return g_cam_buf;
}

uint32_t device_camera_get_buffer_size(void)
{
    return DEVICE_CAM_BUF_SIZE;
}

void device_camera_frame_isr(void)
{
    plat_dcache_invalidate(g_cam_buf, (int32_t)DEVICE_CAM_BUF_SIZE);
}

/* end of  file -------------------------------------------------------------*/
