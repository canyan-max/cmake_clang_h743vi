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

/* variables ----------------------------------------------------------------*/

/* function  ----------------------------------------------------------------*/
/**
 * @brief Initialize the fitted board camera and DCMI receive geometry.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_camera_init(void);

/**
 * @brief Start continuous capture into a caller-owned frame buffer.
 * @param p_buffer DMA destination buffer.
 * @param size_bytes Available destination capacity in bytes.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_camera_start(uint8_t *p_buffer, uint32_t size_bytes);

/**
 * @brief Stop the active board-camera capture.
 * @retval PLATFORM_ERR_OK on success; otherwise a stable Platform error.
 */
platform_err_t bsp_camera_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_CAMERA_H */

/* end of file --------------------------------------------------------------*/

