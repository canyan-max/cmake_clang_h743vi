/**
 ******************************************************************************
 *@file               :   service_app.c
 *@brief              :   Top-level application service.
 *                        Owns the init sequence and per-frame business logic.
 *                        No RTOS primitives — RTOS task calls into here.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "service_app.h"
#include "service_osd.h"
#include "bsp_camera.h"
#include "bsp_display.h"
#include "bsp_eeprom.h"
#include "bsp_led.h"

/* variables ----------------------------------------------------------------*/
static service_app_frame_ready_cb_t volatile s_frame_ready_callback;
static service_app_display_complete_cb_t volatile s_display_complete_callback;
static bsp_display_info_t s_display_info;
static uint16_t           s_camera_draw_x;
static uint16_t           s_camera_draw_y;
static uint16_t           s_camera_draw_width;
static uint16_t           s_camera_draw_height;
static const uint8_t     *s_active_camera_frame;

/* Private functions --------------------------------------------------------*/
static void service_camera_event_from_isr(bsp_camera_event_t event)
{
    service_app_frame_ready_cb_t callback = s_frame_ready_callback;
    if((BSP_CAMERA_EVENT_FRAME_READY == event) && (NULL != callback))
    {
        callback();
    }
}

static void service_display_complete_from_isr(platform_err_t status)
{
    service_app_display_complete_cb_t callback = s_display_complete_callback;
    if(NULL != callback)
    {
        callback(status);
    }
}

/* exported functions -------------------------------------------------------*/

platform_err_t
service_app_set_frame_ready_callback(service_app_frame_ready_cb_t callback)
{
    s_frame_ready_callback = callback;
    return bsp_camera_set_event_callback(
        (NULL != callback) ? service_camera_event_from_isr : NULL);
}

platform_err_t service_app_set_display_complete_callback(
    service_app_display_complete_cb_t callback)
{
    s_display_complete_callback = callback;
    return PLATFORM_ERR_OK;
}

/**
 * @brief            : [service_app_init] initialize all services in order.
 * @retval           : PLATFORM_ERR_OK / PLATFORM_ERR_HW
 */
platform_err_t service_app_init(void)
{
    s_active_camera_frame = NULL;
    (void)bsp_eeprom_init();

    if(PLATFORM_ERR_OK != bsp_display_get_info(&s_display_info))
    {
        return PLATFORM_ERR_HW;
    }
    if((BSP_DISPLAY_PIXEL_FORMAT_RGB565 != s_display_info.input_format) ||
       (s_display_info.canvas_width < BSP_CAMERA_FRAME_WIDTH) ||
       (s_display_info.canvas_height <= OSD_BAR_HEIGHT))
    {
        return PLATFORM_ERR_PARAM;
    }

    s_camera_draw_width  = BSP_CAMERA_FRAME_WIDTH;
    s_camera_draw_height = s_display_info.canvas_height - OSD_BAR_HEIGHT;
    if(s_camera_draw_height > BSP_CAMERA_FRAME_HEIGHT)
    {
        s_camera_draw_height = BSP_CAMERA_FRAME_HEIGHT;
    }
    s_camera_draw_x = (s_display_info.canvas_width - s_camera_draw_width) / 2U;
    s_camera_draw_y = OSD_BAR_HEIGHT +
                      ((s_display_info.canvas_height - OSD_BAR_HEIGHT -
                        s_camera_draw_height) /
                       2U);

    if(PLATFORM_ERR_OK != bsp_display_init())
    {
        return PLATFORM_ERR_HW;
    }
    if(PLATFORM_ERR_OK != bsp_display_fill_screen(0x0000U))
    {
        return PLATFORM_ERR_HW;
    }
    if(PLATFORM_ERR_OK != bsp_camera_init())
    {
        return PLATFORM_ERR_HW;
    }
    if(PLATFORM_ERR_OK != bsp_camera_start())
    {
        return PLATFORM_ERR_HW;
    }

    if(PLATFORM_ERR_OK != service_osd_init(s_display_info.canvas_width))
    {
        return PLATFORM_ERR_PARAM;
    }
    service_osd_set_rec_state(SERVICE_OSD_REC_ACTIVE);
    return PLATFORM_ERR_OK;
}

/**
 * @brief            : [service_on_frame_start] render OSD and start the latest
 *                     camera frame transfer from the RTOS task.
 */
platform_err_t service_on_frame_start(void)
{
    if(NULL != s_active_camera_frame)
    {
        return PLATFORM_ERR_BUSY;
    }

    const uint8_t *p_frame = NULL;
    platform_err_t status = bsp_camera_acquire_frame(&p_frame);
    if(PLATFORM_ERR_OK != status)
    {
        return status;
    }

    status = bsp_camera_capture_next();
    if(PLATFORM_ERR_OK != status)
    {
        (void)bsp_camera_release_frame(p_frame);
        return status;
    }

    status = service_osd_render();
    if(PLATFORM_ERR_OK != status)
    {
        (void)bsp_camera_release_frame(p_frame);
        return status;
    }

    s_active_camera_frame = p_frame;
    status = bsp_display_draw_image_async(
        s_camera_draw_x, s_camera_draw_y, s_camera_draw_width,
        s_camera_draw_height, p_frame,
        service_display_complete_from_isr);
    if(PLATFORM_ERR_OK != status)
    {
        s_active_camera_frame = NULL;
        (void)bsp_camera_release_frame(p_frame);
    }
    else
    {
        bsp_led_toggle(BSP_LED_1);
        bsp_led_toggle(BSP_LED_2);
    }
    return status;
}

platform_err_t service_app_abort_frame_display(void)
{
    return bsp_display_abort_async();
}

platform_err_t service_app_finish_frame_display(void)
{
    const uint8_t *p_frame = s_active_camera_frame;
    if(NULL == p_frame)
    {
        return PLATFORM_ERR_PARAM;
    }

    platform_err_t status = bsp_camera_release_frame(p_frame);
    if(PLATFORM_ERR_OK == status)
    {
        s_active_camera_frame = NULL;
    }
    return status;
}

/* end of file --------------------------------------------------------------*/
