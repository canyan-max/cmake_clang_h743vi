/**
 ******************************************************************************
 *@file               :   ov2640_adapter.c
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 * 
 *@pardependencies    :   bsp_drv_ov2640.h
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "ov2640_adapter.h"
#include "bsp_drv_ov2640.h"

/* Exported functions -------------------------------------------------------*/

/**
  * @brief            : [drv_cam_init]
  */
uint8_t drv_cam_init(void *p_drv, void *p_hw_ops,uint8_t sensor_mode)
{
    if(NULL == p_drv || NULL == p_hw_ops)
    {
        return 1U;
    }
    ov2640_state_t ret = ov2640_driver_instruct((ov2640_driver_t *)p_drv,
                                                (const ov2640_hw_ops_t *)p_hw_ops,
                                                (ov2640_sensor_mode_t)sensor_mode);
    return (ret == OV2640_OK) ? 0U : 2U;
}

/**
  * @brief            : [drv_cam_start]
  */
uint8_t drv_cam_start(void *p_drv, uint32_t *p_buf, uint32_t len_words)
{
    if(NULL == p_drv || NULL == p_buf)
    {
        return 1U;
    }
    ov2640_driver_t *p = (ov2640_driver_t *)p_drv;
    if(OV2640_DRIVER_IS_INIT != p->is_init)
    {
        return 3U;
    }
    ov2640_state_t ret = p->pf_start(p, p_buf, len_words);
    return (ret == OV2640_OK) ? 0U : 2U;
}

/**
  * @brief            : [drv_cam_stop]
  */
uint8_t drv_cam_stop(void *p_drv)
{
    if(NULL == p_drv)
    {
        return 1U;
    }
    ov2640_driver_t *p = (ov2640_driver_t *)p_drv;
    if(OV2640_DRIVER_IS_INIT != p->is_init)
    {
        return 3U;
    }
    ov2640_state_t ret = p->pf_stop(p);
    return (ret == OV2640_OK) ? 0U : 2U;
}

/* end of file --------------------------------------------------------------*/
