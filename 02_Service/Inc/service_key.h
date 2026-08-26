/**
 ******************************************************************************
 *@file               :   service_key.h
 *@brief              :   Non-blocking key service — debounce + single /
 *                        double / long press detection driven by tick
 *                        timestamps. Never blocks: call service_key_poll()
 *                        periodically (e.g. every 10 ms from a task).
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef SERVICE_KEY_H
#define SERVICE_KEY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"

/* define -------------------------------------------------------------------*/
/* Recommended poll period. Key detection tolerates jitter, so any period in
 * the 5..50 ms range works; the freertos.c test task uses this constant. */
#define SERVICE_KEY_POLL_PERIOD_MS   (10U)

/* typedef ------------------------------------------------------------------*/
/**
 * @brief Key gesture event reported through the registered callback.
 */
typedef enum
{
    SERVICE_KEY_EVENT_CLICK        = 0U, /**< Single click (release, no second press). */
    SERVICE_KEY_EVENT_DOUBLE_CLICK = 1U, /**< Two presses within the double-click window. */
    SERVICE_KEY_EVENT_LONG_PRESS   = 2U, /**< Held longer than the long-press threshold. */
} service_key_event_t;

/**
 * @brief Key event callback, invoked from the task that calls
 *        service_key_poll() — normal task context, never ISR.
 * @param[in] key_id Board key index (0..bsp_key_count()-1).
 * @param[in] event  Detected gesture.
 */
typedef void (*service_key_event_cb_t)(uint8_t key_id, service_key_event_t event);

/* functions ----------------------------------------------------------------*/
/**
 * @brief            [service_key_init] Capture the current key levels and arm
 *                     the gesture state machines for every board key.
 * @param[in]        [cb] Event callback; may be NULL to run detection without
 *                     notifications (e.g. query-only bring-up).
 * @retval           PLATFORM_ERR_OK on success; PLATFORM_ERR_PARAM when the
 *                   fitted key count exceeds the static service capacity;
 *                   otherwise the BSP read error.
 */
platform_err_t service_key_init(service_key_event_cb_t cb);

/**
 * @brief            [service_key_poll] Sample all keys once and advance each
 *                     gesture state machine. Non-blocking; call periodically.
 * @retval           PLATFORM_ERR_OK on success; otherwise the BSP read error.
 */
platform_err_t service_key_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_KEY_H */
