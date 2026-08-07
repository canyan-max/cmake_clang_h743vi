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
#include "camera_intf.h"
#include "plat_sys.h"

/* define   -----------------------------------------------------------------*/
#define DEVICE_CAM_BUF_SIZE  BSP_CAMERA_BUF_SIZE

/* variables ----------------------------------------------------------------*/
__attribute__((section(".ram_d2_dma_buffers"), aligned(32)))
static uint8_t g_cam_buf[DEVICE_CAM_BUF_SIZE];

/* exported functions -------------------------------------------------------*/

platform_err_t device_camera_init(void)
{
    return g_camera_bsp_ops.pf_init();
}

platform_err_t device_camera_start(void)
{
    return g_camera_bsp_ops.pf_start((uint32_t *)g_cam_buf,
                                      DEVICE_CAM_BUF_SIZE / sizeof(uint32_t));
}

platform_err_t device_camera_stop(void)
{
    return g_camera_bsp_ops.pf_stop();
}

uint8_t *device_camera_get_buffer(void)
{
    return g_cam_buf ;
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
