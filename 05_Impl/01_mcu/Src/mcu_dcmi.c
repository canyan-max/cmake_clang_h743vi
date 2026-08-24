/**
 ******************************************************************************
 *@file               :   mcu_dcmi.c
 *@brief              :   STM32 HAL implementation of the portable DCMI API.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include "plat_dcmi.h"
#include "board_stm32h743_binding.h"
#include "dcmi.h"

/* define   -----------------------------------------------------------------*/
#define PLAT_DCMI_DMA_ALIGNMENT_BYTES  (4U)

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static plat_dcmi_event_cb_t volatile s_dcmi_event_callback;

/* Private  functions  ------------------------------------------------------*/
static platform_err_t plat_dcmi_convert_hal_status(HAL_StatusTypeDef status)
{
    switch(status)
    {
        case HAL_OK:
            return PLATFORM_ERR_OK;

        case HAL_BUSY:
            return PLATFORM_ERR_BUSY;

        case HAL_TIMEOUT:
            return PLATFORM_ERR_TIMEOUT;

        case HAL_ERROR:
        default:
            return PLATFORM_ERR_HW;
    }
}

static uint8_t plat_dcmi_get_hal_mode(plat_dcmi_mode_t mode,
                                      uint32_t        *p_hal_mode)
{
    switch(mode)
    {
        case PLAT_DCMI_MODE_CONTINUOUS:
            *p_hal_mode = DCMI_MODE_CONTINUOUS;
            return 1U;

        case PLAT_DCMI_MODE_SNAPSHOT:
            *p_hal_mode = DCMI_MODE_SNAPSHOT;
            return 1U;

        case PLAT_DCMI_MODE_NUM:
        default:
            return 0U;
    }
}

static void plat_dcmi_notify_event_from_isr(plat_dcmi_event_t event)
{
    plat_dcmi_event_cb_t callback = s_dcmi_event_callback;
    if(NULL != callback)
    {
        callback(event);
    }
}

/* Exported functions -------------------------------------------------------*/
platform_err_t plat_dcmi_set_event_callback(plat_dcmi_event_cb_t callback)
{
    if(HAL_DCMI_STATE_BUSY == HAL_DCMI_GetState(&BOARD_CAM_DCMI_HANDLE))
    {
        return PLATFORM_ERR_BUSY;
    }

    s_dcmi_event_callback = callback;
    return PLATFORM_ERR_OK;
}

platform_err_t plat_dcmi_config_crop(const plat_dcmi_crop_t *p_crop)
{
    if((NULL == p_crop) || (0U == p_crop->capture_width_bytes) ||
       (0U == p_crop->capture_height_lines))
    {
        return PLATFORM_ERR_PARAM;
    }

    uint32_t width_count  = (uint32_t)p_crop->capture_width_bytes - 1U;
    uint32_t height_count = (uint32_t)p_crop->capture_height_lines - 1U;
    if(((uint32_t)p_crop->horizontal_offset_bytes > DCMI_WINDOW_COORDINATE) ||
       ((uint32_t)p_crop->vertical_offset_lines > DCMI_WINDOW_HEIGHT) ||
       (width_count > DCMI_WINDOW_COORDINATE) ||
       (height_count > DCMI_WINDOW_COORDINATE))
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_StatusTypeDef
        status = HAL_DCMI_ConfigCrop(&BOARD_CAM_DCMI_HANDLE,
                                     p_crop->horizontal_offset_bytes,
                                     p_crop->vertical_offset_lines, width_count,
                                     height_count);
    if(HAL_OK != status)
    {
        return plat_dcmi_convert_hal_status(status);
    }

    status = HAL_DCMI_EnableCrop(&BOARD_CAM_DCMI_HANDLE);
    return plat_dcmi_convert_hal_status(status);
}

platform_err_t plat_dcmi_start_dma(uint8_t         *p_buffer,
                                   uint32_t         size_bytes,
                                   plat_dcmi_mode_t mode)
{
    uint32_t hal_mode;
    if((NULL == p_buffer) || (0U == size_bytes) ||
       (0U != (size_bytes % PLAT_DCMI_DMA_ALIGNMENT_BYTES)) ||
       (0U != ((uintptr_t)p_buffer % PLAT_DCMI_DMA_ALIGNMENT_BYTES)) ||
       (0U == plat_dcmi_get_hal_mode(mode, &hal_mode)))
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_StatusTypeDef
        status = HAL_DCMI_Start_DMA(&BOARD_CAM_DCMI_HANDLE, hal_mode,
                                    (uint32_t)(uintptr_t)p_buffer,
                                    size_bytes / PLAT_DCMI_DMA_ALIGNMENT_BYTES);
    if(HAL_OK == status)
    {
        __HAL_DCMI_ENABLE_IT(&BOARD_CAM_DCMI_HANDLE,
                             DCMI_IT_FRAME | DCMI_IT_OVR | DCMI_IT_ERR);
    }
    return plat_dcmi_convert_hal_status(status);
}

platform_err_t plat_dcmi_stop(void)
{
    return plat_dcmi_convert_hal_status(HAL_DCMI_Stop(&BOARD_CAM_DCMI_HANDLE));
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *p_hdcmi)
{
    if(p_hdcmi == &BOARD_CAM_DCMI_HANDLE)
    {
        plat_dcmi_notify_event_from_isr(PLAT_DCMI_EVENT_FRAME_READY);
    }
}

void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *p_hdcmi)
{
    if(p_hdcmi != &BOARD_CAM_DCMI_HANDLE)
    {
        return;
    }

    if(0U != (HAL_DCMI_GetError(p_hdcmi) & HAL_DCMI_ERROR_OVR))
    {
        plat_dcmi_notify_event_from_isr(PLAT_DCMI_EVENT_OVERRUN);
    }
    else
    {
        plat_dcmi_notify_event_from_isr(PLAT_DCMI_EVENT_ERROR);
    }
}

/* end of file --------------------------------------------------------------*/
