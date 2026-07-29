/**
 ******************************************************************************
 *@file               :   device_camera.c
 *
 *@brief              :   Camera device implementation — OV2640 via DCMI.
 *
 *@version            :   V2.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#include <stddef.h>
#include "device_camera.h"
#include "bsp_drv_ov2640.h"
#include "st_ov2640.h"

/* ---- local variables ---------------------------------------------------- */

static ov2640_driver_t g_cam;

/* ---- public functions --------------------------------------------------- */

platform_err_t device_camera_init(void)
{
    ov2640_state_t ret = ov2640_driver_instruct(&g_cam,
                                                 &g_ov2640_hw_ops,
                                                 OV2640_MODE_SVGA);
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_camera_start(uint32_t *p_buf, uint32_t len_words)
{
    if (NULL == p_buf) { return PLATFORM_ERR_PARAM; }
    if (OV2640_DRIVER_IS_INIT != g_cam.is_init) { return PLATFORM_ERR_HW; }
    ov2640_state_t ret = g_cam.pf_start(&g_cam, p_buf, len_words);
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_camera_stop(void)
{
    if (OV2640_DRIVER_IS_INIT != g_cam.is_init) { return PLATFORM_ERR_HW; }
    ov2640_state_t ret = g_cam.pf_stop(&g_cam);
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}
