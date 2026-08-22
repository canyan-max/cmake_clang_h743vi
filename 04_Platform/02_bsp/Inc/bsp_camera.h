/**
 ******************************************************************************
 *@file               :   bsp_camera.h
 *@brief              :   Board camera capability contract.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_CAMERA_H
#define BSP_CAMERA_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"

/* define -------------------------------------------------------------------*/
#define BSP_CAMERA_FRAME_WIDTH       (240U)
#define BSP_CAMERA_FRAME_HEIGHT      (240U)
#define BSP_CAMERA_BYTES_PER_PIXEL   (2U)
#define BSP_CAMERA_FRAME_SIZE_BYTES  \
    (BSP_CAMERA_FRAME_WIDTH * BSP_CAMERA_FRAME_HEIGHT * \
     BSP_CAMERA_BYTES_PER_PIXEL)

/* typedef ------------------------------------------------------------------*/
typedef enum BSP_CAMERA_EVENT_T
{
    BSP_CAMERA_EVENT_FRAME_READY = 0U,
    BSP_CAMERA_EVENT_OVERRUN,
    BSP_CAMERA_EVENT_ERROR,
    BSP_CAMERA_EVENT_NUM,
} bsp_camera_event_t;

/**
 * @brief Board-camera event callback invoked from interrupt context.
 * @note Implementations must not block or call non-ISR-safe services.
 */
typedef void (*bsp_camera_event_cb_t)(bsp_camera_event_t event);

/* variables ----------------------------------------------------------------*/

/* function  ----------------------------------------------------------------*/
/**
 * @brief Register the board-camera interrupt-event callback.
 * @param callback Callback to invoke from ISR context; NULL unregisters it.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_camera_set_event_callback(bsp_camera_event_cb_t callback);

/**
 * @brief Initialize the fitted board camera and DCMI receive geometry.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_camera_init(void);

/**
 * @brief Start the first snapshot into a free BSP-owned frame buffer.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_camera_start(void);

/**
 * @brief Start one snapshot into the next free BSP-owned frame buffer.
 * @note A buffer acquired by the caller is never selected for capture.
 * @retval PLATFORM_ERR_OK on success; PLATFORM_ERR_BUSY when no buffer is free.
 */
platform_err_t bsp_camera_capture_next(void);

/**
 * @brief Stop the active board-camera capture.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_camera_stop(void);

/**
 * @brief Acquire the completed frame and prepare it for CPU/DMA reading.
 * @param pp_frame Receives the BSP-owned frame address on success.
 * @note The acquired buffer is protected from subsequent camera capture until
 *       bsp_camera_release_frame() is called.
 * @retval PLATFORM_ERR_OK on success; PLATFORM_ERR_BUSY if no frame is ready.
 */
platform_err_t bsp_camera_acquire_frame(const uint8_t **pp_frame);

/**
 * @brief Release a previously acquired frame buffer back to the camera BSP.
 * @param p_frame Frame address returned by bsp_camera_acquire_frame().
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_camera_release_frame(const uint8_t *p_frame);

/** @brief Return the BSP-owned frame-buffer capacity in bytes. */
uint32_t bsp_camera_get_frame_buffer_size(void);

/** @brief Return the number of DCMI overruns since initialization. */
uint32_t bsp_camera_get_overrun_count(void);

/** @brief Return the number of other DCMI errors since initialization. */
uint32_t bsp_camera_get_error_count(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_CAMERA_H */

/* end of file --------------------------------------------------------------*/
