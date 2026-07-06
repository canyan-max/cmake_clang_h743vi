/**
 ******************************************************************************
 *@file               :   camera_handle.c
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 * 
 *@pardependencies    :   camera_handle.h
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "camera_handle.h"

/* Exported functions -------------------------------------------------------*/

/**
 * @brief            : [camera_handle_instruct]
 */
uint8_t camera_handle_instruct(camera_handle_t           *p_handle,
                               const camera_handle_ops_t *p_ops,
                               void                      *p_drv,
                               void                      *p_hw_ops,
                               uint8_t                    sensor_mode)
{
    if(NULL == p_handle || NULL == p_ops || NULL == p_drv || NULL == p_hw_ops)
    {
        return 1U;
    }
    p_handle->p_ops = p_ops;
    p_handle->p_drv = p_drv;
    return p_ops->pf_drv_init(p_drv, p_hw_ops, sensor_mode);
}

/**
 * @brief            : [camera_handle_start]
 */
uint8_t camera_handle_start(camera_handle_t *p_handle,
                            uint32_t        *p_buf,
                            uint32_t         len_words)
{
    if(NULL == p_handle || NULL == p_handle->p_ops || NULL == p_buf)
    {
        return 1U;
    }
    return p_handle->p_ops->pf_start(p_handle->p_drv, p_buf, len_words);
}

/**
 * @brief            : [camera_handle_stop]
 */
uint8_t camera_handle_stop(camera_handle_t *p_handle)
{
    if(NULL == p_handle || NULL == p_handle->p_ops)
    {
        return 1U;
    }
    return p_handle->p_ops->pf_stop(p_handle->p_drv);
}

/* end of file --------------------------------------------------------------*/
