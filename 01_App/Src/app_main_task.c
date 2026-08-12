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

    if(PLATFORM_ERR_OK != service_app_init())
    {
        logError("service_app_init failed");
    }

    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        service_on_frame();
        vTaskDelay(pdMS_TO_TICKS(5U));
    }
}

/* end of file --------------------------------------------------------------*/
