/**
 ******************************************************************************
 *@file               :   device_storage.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef DEVICE_STORAGE_H
#define DEVICE_STORAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stdint lib header file. */
#include "platform_error.h"

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/

platform_err_t device_storage_init(const void *p_iic_ops,
                                   uint32_t    max_byte_addr,
                                   uint32_t    page_size,
                                   uint16_t    adr_size,
                                   uint8_t     dev_adr);
platform_err_t device_storage_write_page(uint16_t mem_adr,
                                         uint8_t *p_data,
                                         uint16_t size,
                                         uint32_t timeout);
platform_err_t device_storage_write_cross_page(uint16_t mem_adr,
                                               uint8_t *p_data,
                                               uint16_t size,
                                               uint32_t timeout);
platform_err_t device_storage_read(uint16_t mem_adr,
                                   uint8_t *p_data,
                                   uint16_t size,
                                   uint32_t timeout);
#ifdef __cplusplus
}
#endif

#endif /* DEVICE_STORAGE_H */
