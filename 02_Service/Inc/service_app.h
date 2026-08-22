/**
 ******************************************************************************
 *@file               :   service_app.h
 *@brief              :   Top-level application service: init sequence and
 *                        per-frame business logic. No RTOS primitives here.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef SERVICE_APP_H
#define SERVICE_APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "plat_error.h"

/**
 * @brief Camera-frame notification invoked from interrupt context.
 * @note The callback must only use ISR-safe operations and must not block.
 */
typedef void (*service_app_frame_ready_cb_t)(void);

/**
 * @brief Display-frame completion invoked from interrupt context.
 * @note The callback must only use ISR-safe operations and must not block.
 */
typedef void (*service_app_display_complete_cb_t)(platform_err_t status);

/**
 * @brief Register the application camera-frame notification callback.
 * @param callback Callback invoked from ISR context; NULL unregisters it.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t
service_app_set_frame_ready_callback(service_app_frame_ready_cb_t callback);

/**
 * @brief Register the application display-completion callback.
 * @param callback Callback invoked from ISR context; NULL unregisters it.
 */
platform_err_t service_app_set_display_complete_callback(
    service_app_display_complete_cb_t callback);

platform_err_t service_app_init(void);

/**
 * @brief Render the OSD and start non-blocking display of the latest frame.
 * @retval PLATFORM_ERR_OK when accepted; otherwise a stable Platform error.
 */
platform_err_t service_on_frame_start(void);

/**
 * @brief Abort the active asynchronous frame display.
 * @note No display-completion callback is made for an explicit abort.
 */
platform_err_t service_app_abort_frame_display(void);

/**
 * @brief Release the camera frame after display DMA has completed or stopped.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t service_app_finish_frame_display(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_APP_H */
