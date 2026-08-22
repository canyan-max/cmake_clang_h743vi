/**
 ******************************************************************************
 *@file               :   app_main_task.c
 *@brief              :   Default application task implementation.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "app_main_task.h"
#include "service_app.h"
#include "log.h"

/* define   -----------------------------------------------------------------*/
#define APP_CAMERA_NOTIFY_INDEX       (0U)
#define APP_DISPLAY_NOTIFY_INDEX      (1U)
#define APP_DISPLAY_TIMEOUT_MS        (100U)

/* variables ----------------------------------------------------------------*/
static TaskHandle_t s_app_task_handle;
static volatile platform_err_t s_display_status;

/* Private functions --------------------------------------------------------*/
static void app_camera_frame_ready_from_isr(void)
{
    BaseType_t   higher_priority_task_woken = pdFALSE;
    TaskHandle_t task_handle                = s_app_task_handle;

    if(NULL != task_handle)
    {
        vTaskNotifyGiveIndexedFromISR(task_handle, APP_CAMERA_NOTIFY_INDEX,
                                      &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }
}

static void app_display_complete_from_isr(platform_err_t status)
{
    BaseType_t   higher_priority_task_woken = pdFALSE;
    TaskHandle_t task_handle                = s_app_task_handle;

    s_display_status = status;
    if(NULL != task_handle)
    {
        vTaskNotifyGiveIndexedFromISR(task_handle, APP_DISPLAY_NOTIFY_INDEX,
                                      &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }
}

/* exported functions -------------------------------------------------------*/

/**
 * @brief            : [app_main_task] Initialize application services and
 *                     process each camera-frame notification.
 * @param[in]        : p_argument RTOS task argument; currently unused.
 */
void app_main_task(void *p_argument)
{
    portTASK_USES_FLOATING_POINT();
    ((void)p_argument);
    s_app_task_handle = xTaskGetCurrentTaskHandle();

    if(PLATFORM_ERR_OK !=
       service_app_set_frame_ready_callback(app_camera_frame_ready_from_isr))
    {
        logError("camera frame callback registration failed");
    }
    if(PLATFORM_ERR_OK != service_app_set_display_complete_callback(
                              app_display_complete_from_isr))
    {
        logError("display callback registration failed");
    }

    if(PLATFORM_ERR_OK != service_app_init())
    {
        logError("service_app_init failed");
    }

    for(;;)
    {
        (void)ulTaskNotifyTakeIndexed(APP_CAMERA_NOTIFY_INDEX, pdTRUE,
                                      portMAX_DELAY);
        (void)ulTaskNotifyTakeIndexed(APP_DISPLAY_NOTIFY_INDEX, pdTRUE, 0U);

        platform_err_t status = service_on_frame_start();
        if(PLATFORM_ERR_OK == status)
        {
            uint32_t completed = ulTaskNotifyTakeIndexed(
                APP_DISPLAY_NOTIFY_INDEX, pdTRUE,
                pdMS_TO_TICKS(APP_DISPLAY_TIMEOUT_MS));
            if(0U == completed)
            {
                platform_err_t abort_status =
                    service_app_abort_frame_display();
                if(PLATFORM_ERR_OK == abort_status)
                {
                    (void)service_app_finish_frame_display();
                }
                else
                {
                    logError("display DMA abort failed: %d",
                             (int)abort_status);
                }
                logError("display DMA timeout");
            }
            else
            {
                if(PLATFORM_ERR_OK != s_display_status)
                {
                    logError("display DMA failed: %d", (int)s_display_status);
                }
                if(PLATFORM_ERR_OK != service_app_finish_frame_display())
                {
                    logError("camera frame release failed");
                }
            }
        }
        else
        {
            logError("display frame start failed: %d", (int)status);
        }
    }
}

/* end of file --------------------------------------------------------------*/
