/**
 ******************************************************************************
 *@file               :   app_main_task.c
 *@brief              :   Default application task implementation.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "app_main_task.h"
#include "service_app.h"
#include "service_record.h"
#include "log.h"

/* define   -----------------------------------------------------------------*/
#define APP_CAMERA_NOTIFY_INDEX       (0U)
#define APP_DISPLAY_NOTIFY_INDEX      (1U)
#define APP_NOTIFY_FRAME_READY_BIT    (1UL << 0U)
#define APP_NOTIFY_KEY_EVENT_BIT      (1UL << 1U)
#define APP_DISPLAY_TIMEOUT_MS        (100U)
#define APP_KEY_EVENT_QUEUE_LENGTH    (4U)

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t             key_id;
    service_key_event_t event;
} app_key_event_t;

/* variables ----------------------------------------------------------------*/
static TaskHandle_t s_app_task_handle;
static volatile platform_err_t s_display_status;
static QueueHandle_t s_key_event_queue;
static StaticQueue_t s_key_event_queue_control;
static uint8_t s_key_event_queue_storage[APP_KEY_EVENT_QUEUE_LENGTH *
                                         sizeof(app_key_event_t)];

/* Private functions --------------------------------------------------------*/
static void app_camera_frame_ready_from_isr(void)
{
    BaseType_t   higher_priority_task_woken = pdFALSE;
    TaskHandle_t task_handle                = s_app_task_handle;

    if(NULL != task_handle)
    {
        (void)xTaskNotifyIndexedFromISR(task_handle, APP_CAMERA_NOTIFY_INDEX,
                                        APP_NOTIFY_FRAME_READY_BIT, eSetBits,
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

static void app_process_key_events(void)
{
    app_key_event_t key_event;

    while(pdPASS == xQueueReceive(s_key_event_queue, &key_event, 0U))
    {
        service_record_on_key(key_event.key_id, key_event.event);
    }
}

/* exported functions -------------------------------------------------------*/

app_main_post_status_t app_main_post_key_event(uint8_t             key_id,
                                               service_key_event_t event)
{
    switch(event)
    {
        case SERVICE_KEY_EVENT_CLICK:
        case SERVICE_KEY_EVENT_DOUBLE_CLICK:
        case SERVICE_KEY_EVENT_LONG_PRESS:
            break;

        default:
            return APP_MAIN_POST_PARAM;
    }

    QueueHandle_t queue_handle = s_key_event_queue;
    TaskHandle_t task_handle   = s_app_task_handle;
    if((NULL == queue_handle) || (NULL == task_handle))
    {
        return APP_MAIN_POST_UNAVAILABLE;
    }

    const app_key_event_t key_event = {
        .key_id = key_id,
        .event  = event,
    };
    if(pdPASS != xQueueSend(queue_handle, &key_event, 0U))
    {
        return APP_MAIN_POST_UNAVAILABLE;
    }

    (void)xTaskNotifyIndexed(task_handle, APP_CAMERA_NOTIFY_INDEX,
                             APP_NOTIFY_KEY_EVENT_BIT, eSetBits);
    return APP_MAIN_POST_OK;
}

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
    s_key_event_queue = xQueueCreateStatic(
        APP_KEY_EVENT_QUEUE_LENGTH, sizeof(app_key_event_t),
        s_key_event_queue_storage, &s_key_event_queue_control);
    if(NULL == s_key_event_queue)
    {
        logError("key event queue creation failed");
        vTaskDelete(NULL);
        return;
    }

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
        uint32_t app_events = 0U;
        (void)xTaskNotifyWaitIndexed(APP_CAMERA_NOTIFY_INDEX, 0U, UINT32_MAX,
                                     &app_events, portMAX_DELAY);

        if(0U != (app_events & APP_NOTIFY_KEY_EVENT_BIT))
        {
            app_process_key_events();
        }
        if(0U == (app_events & APP_NOTIFY_FRAME_READY_BIT))
        {
            continue;
        }

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
