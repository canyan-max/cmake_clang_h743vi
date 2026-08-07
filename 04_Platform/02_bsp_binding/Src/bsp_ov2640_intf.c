/**
 ******************************************************************************
 *@file               :   bsp_ov2640_intf.c
 *@brief              :   OV2640 implementation of camera_intf.
 *                        Swap this file when replacing camera chip.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "bsp_ov2640.h"
#include "camera_intf.h"

/* variables ----------------------------------------------------------------*/
static ov2640_dev_t g_ov2640_cam;

/* Private functions --------------------------------------------------------*/
static platform_err_t camera_bsp_init(void)
{
    ov2640_state_t ret = bsp_ov2640_init(&g_ov2640_cam, OV2640_MODE_CIF);
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t camera_bsp_start(uint32_t *p_buf, uint32_t len_words)
{
    if(OV2640_DRIVER_IS_INIT != g_ov2640_cam.is_init)
    {
        return PLATFORM_ERR_HW;
    }
    ov2640_state_t ret = ov2640_start(&g_ov2640_cam, p_buf, len_words);
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t camera_bsp_stop(void)
{
    if(OV2640_DRIVER_IS_INIT != g_ov2640_cam.is_init)
    {
        return PLATFORM_ERR_HW;
    }
    ov2640_state_t ret = ov2640_stop(&g_ov2640_cam);
    return (OV2640_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

/* Exported variables -------------------------------------------------------*/
const bsp_camera_ops_t g_camera_bsp_ops = {
    .pf_init  = camera_bsp_init,
    .pf_start = camera_bsp_start,
    .pf_stop  = camera_bsp_stop,
};

/* end of file --------------------------------------------------------------*/
