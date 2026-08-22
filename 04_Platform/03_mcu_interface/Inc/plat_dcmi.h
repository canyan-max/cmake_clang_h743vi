/**
 ******************************************************************************
 *@file               :   plat_dcmi.h
 *@brief              :   Portable DCMI capture contract.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef PLAT_DCMI_H
#define PLAT_DCMI_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef enum PLAT_DCMI_MODE_T
{
    PLAT_DCMI_MODE_CONTINUOUS = 0U,
    PLAT_DCMI_MODE_SNAPSHOT,
    PLAT_DCMI_MODE_NUM,
} plat_dcmi_mode_t;

typedef enum PLAT_DCMI_EVENT_T
{
    PLAT_DCMI_EVENT_FRAME_READY = 0U,
    PLAT_DCMI_EVENT_OVERRUN,
    PLAT_DCMI_EVENT_ERROR,
    PLAT_DCMI_EVENT_NUM,
} plat_dcmi_event_t;

/**
 * @brief DCMI event callback invoked from interrupt context.
 * @note Implementations must not block or call non-ISR-safe services.
 */
typedef void (*plat_dcmi_event_cb_t)(plat_dcmi_event_t event);

typedef struct PLAT_DCMI_CROP_T
{
    uint16_t horizontal_offset_bytes;
    uint16_t vertical_offset_lines;
    uint16_t capture_width_bytes;
    uint16_t capture_height_lines;
} plat_dcmi_crop_t;

/* variables ----------------------------------------------------------------*/

/* function  ----------------------------------------------------------------*/
/**
 * @brief Register the DCMI interrupt-event callback.
 * @param callback Callback to invoke from ISR context; NULL unregisters it.
 * @note Registration is only allowed while capture is stopped.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t plat_dcmi_set_event_callback(plat_dcmi_event_cb_t callback);

/**
 * @brief Configure and enable the DCMI receive crop window.
 * @param p_crop Crop geometry expressed in received bytes and image lines.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t plat_dcmi_config_crop(const plat_dcmi_crop_t *p_crop);

/**
 * @brief Start DCMI capture into a caller-owned DMA buffer.
 * @param p_buffer Destination buffer; must be at least 4-byte aligned.
 * @param size_bytes Capture size in bytes; must be non-zero and divisible by 4.
 * @param mode Continuous or single-frame capture mode.
 * @note The caller owns DMA memory placement, lifetime, and cache maintenance.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t plat_dcmi_start_dma(uint8_t         *p_buffer,
                                   uint32_t         size_bytes,
                                   plat_dcmi_mode_t mode);

/**
 * @brief Stop the active DCMI capture.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t plat_dcmi_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_DCMI_H */

/* end of file --------------------------------------------------------------*/
