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
#include "service_app.h"
#include "service_osd.h"
#include "device_camera.h"
#include "device_display.h"
#include "device_eeprom.h"
#include "device_indicator.h"

/* exported functions -------------------------------------------------------*/

/**
  * @brief            : [service_app_init] initialize all services in order.
  * @retval           : PLATFORM_ERR_OK / PLATFORM_ERR_HW
  */
platform_err_t service_app_init(void)
{
    device_indicator_init(DEVICE_INDICATOR_1);
    device_indicator_init(DEVICE_INDICATOR_2);
    device_eeprom_init();

    if(PLATFORM_ERR_OK != device_display_init())    { return PLATFORM_ERR_HW; }
    if(PLATFORM_ERR_OK != device_display_fill_screen(0x0000U)) { return PLATFORM_ERR_HW; }
    if(PLATFORM_ERR_OK != device_camera_init())     { return PLATFORM_ERR_HW; }
    if(PLATFORM_ERR_OK != device_camera_start())    { return PLATFORM_ERR_HW; }

    service_osd_init();
    service_osd_set_rec_state(SERVICE_OSD_REC_ACTIVE);
    return PLATFORM_ERR_OK;
}

/**
  * @brief            : [service_on_frame] per-frame business logic.
  *                     Called by RTOS task on each camera frame notification.
  */
void service_on_frame(void)
{
    service_osd_render();
    device_display_draw_image(0U, OSD_BAR_HEIGHT,
                               240U, 240U - OSD_BAR_HEIGHT,
                               device_camera_get_buffer());
    device_indicator_blink(DEVICE_INDICATOR_1);
    device_indicator_blink(DEVICE_INDICATOR_2);
}

/* end of file --------------------------------------------------------------*/
