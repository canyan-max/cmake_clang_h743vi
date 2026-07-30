/**
 ******************************************************************************
 *@file               :   service_camera.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef SERVICE_CAMERA_H
#define SERVICE_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */
#include <stddef.h>                               /* stdint lib header file. */
/* define -------------------------------------------------------------------*/
#define SERVICE_CAMERA_BUF_SIZE  (115200U)
/* typedef ------------------------------------------------------------------*/

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
void         service_camera_init(void);
void         service_camera_start(void);
uint8_t     *service_camera_get_buffer(void);
uint32_t     service_camera_get_buffer_len(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_CAMERA_H */
