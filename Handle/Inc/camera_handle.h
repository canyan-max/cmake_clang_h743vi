/**
 ******************************************************************************
 *@file               :   camera_handle.h
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef CAMERA_HANDLE_H
#define CAMERA_HANDLE_H


#ifdef __cplusplus
extern "C" {
#endif


/* Includes -----------------------------------------------------------------*/
#include <stdint.h>

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t (*pf_drv_init)(void *p_drv, void *p_hw_ops, uint8_t sensor_mode);
    uint8_t (*pf_start)(void *p_drv, uint32_t *p_buf, uint32_t len_words);
    uint8_t (*pf_stop)(void *p_drv);
} camera_handle_ops_t;

typedef struct
{
    const camera_handle_ops_t *p_ops;
    void                      *p_drv;
} camera_handle_t;

/* functions ----------------------------------------------------------------*/
/**
  * @brief            : [camera_handle_instruct]
                        Initialize driver and bind ops to the handle.
  * @retval           : [0 OK, 1 invalid param, other driver error]
  * @param[in]        : [p_handle, p_ops, p_drv, p_hw_ops]
  */
uint8_t camera_handle_instruct(camera_handle_t           *p_handle,
                               const camera_handle_ops_t *p_ops,
                               void                      *p_drv,
                               void                      *p_hw_ops,
                               uint8_t                    sensor_mode);

/**
  * @brief            : [camera_handle_start]
                        Start continuous DCMI DMA capture into p_buf.
  * @retval           : [0 OK, 1 invalid param, other driver error]
  * @param[in]        : [p_handle, p_buf — DMA buffer, len_words — size in 32-bit words]
  */
uint8_t camera_handle_start(camera_handle_t *p_handle,
                             uint32_t        *p_buf,
                             uint32_t         len_words);

/**
  * @brief            : [camera_handle_stop] Stop DCMI DMA capture.
  * @retval           : [0 OK, 1 invalid param, other driver error]
  * @param[in]        : [p_handle]
  */
uint8_t camera_handle_stop(camera_handle_t *p_handle);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HANDLE_H */
