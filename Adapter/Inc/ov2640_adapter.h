/**
 ******************************************************************************
 *@file               :   ov2640_adapter.h
 *
 *@brief              :   Provide the HAL APIs of description.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef OV2640_ADAPTER_H
#define OV2640_ADAPTER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>

/* functions ----------------------------------------------------------------*/
/**
 * @brief            : [drv_cam_init] Init ov2640_driver_t via ov2640_hw_ops_t.
 * @retval           : [0 OK, 1 invalid param, 2 driver error]
 * @param[in]        : [p_drv → ov2640_driver_t*, p_hw_ops → ov2640_hw_ops_t*]
 */
uint8_t drv_cam_init(void *p_drv, void *p_hw_ops, uint8_t sensor_mode);

/**
 * @brief            : [drv_cam_start] Start DCMI DMA capture.
 * @retval           : [0 OK, 1 invalid param, 2 driver error, 3 not inited]
 * @param[in]        : [p_drv, p_buf — DMA buffer, len_words — buffer size in
 * 32-bit words]
 */
uint8_t drv_cam_start(void *p_drv, uint32_t *p_buf, uint32_t len_words);

/**
 * @brief            : [drv_cam_stop] Stop DCMI DMA capture.
 * @retval           : [0 OK, 1 invalid param, 2 driver error, 3 not inited]
 * @param[in]        : [p_drv]
 */
uint8_t drv_cam_stop(void *p_drv);
#ifdef __cplusplus
}
#endif
#endif /* OV2640_ADAPTER_H */
