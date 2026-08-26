/**
 ******************************************************************************
 *@file               :   app_main_task.h
 *@brief              :   Default application task entry.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef APP_MAIN_TASK_H
#define APP_MAIN_TASK_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "service_key.h"

/* typedef ------------------------------------------------------------------*/
typedef enum
{
    APP_MAIN_POST_OK          = 0U,
    APP_MAIN_POST_PARAM       = 1U,
    APP_MAIN_POST_UNAVAILABLE = 2U,
} app_main_post_status_t;

/**
 * @brief Queue a key gesture for processing by the main application task.
 * @param key_id Board key index reported by service_key.
 * @param event Detected key gesture.
 * @retval APP_MAIN_POST_OK when queued; APP_MAIN_POST_UNAVAILABLE when the
 *         application queue is unavailable or full; APP_MAIN_POST_PARAM for
 *         an invalid event.
 * @note Call from normal task context only. This function never blocks.
 */
app_main_post_status_t app_main_post_key_event(uint8_t             key_id,
                                               service_key_event_t event);

/**
  * @brief            : [app_main_task] Run the default application task.
  * @param[in]        : p_argument RTOS task argument; currently unused.
  */
void app_main_task(void *p_argument);

#ifdef __cplusplus
}
#endif

#endif /* APP_MAIN_TASK_H */
