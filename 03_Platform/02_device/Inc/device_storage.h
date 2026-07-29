/**
 ******************************************************************************
 *@file               :   device_storage.h
 *
 *@brief              :   Storage device — owns AT24 EEPROM driver, exposes clean API.
 *
 *@version            :   V2.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef DEVICE_STORAGE_H
#define DEVICE_STORAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "platform_error.h"

platform_err_t device_storage_init(const void *p_iic_ops,
                                    uint32_t    max_byte_addr,
                                    uint32_t    page_size,
                                    uint16_t    adr_size,
                                    uint8_t     dev_adr);
platform_err_t device_storage_write_page(uint16_t mem_adr, uint8_t *p_data,
                                          uint16_t size, uint32_t timeout);
platform_err_t device_storage_write_cross_page(uint16_t mem_adr, uint8_t *p_data,
                                                uint16_t size, uint32_t timeout);
platform_err_t device_storage_read(uint16_t mem_adr, uint8_t *p_data,
                                    uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_STORAGE_H */
