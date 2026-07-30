/**
 ******************************************************************************
 *@file               :   service_storage.h
 *@brief              :   Provide the service storage APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef SERVICE_STORAGE_H
#define SERVICE_STORAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "platform_error.h"

/* functions ----------------------------------------------------------------*/
platform_err_t service_storage_init(void);
platform_err_t service_storage_write(uint16_t addr,
                                     uint8_t *p_data,
                                     uint16_t size,
                                     uint32_t timeout);
platform_err_t service_storage_read(uint16_t addr,
                                    uint8_t *p_data,
                                    uint16_t size,
                                    uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_STORAGE_H */
