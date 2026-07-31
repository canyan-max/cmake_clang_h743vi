/**
 ******************************************************************************
 *@file               :   device_eeprom.h
 *@brief              :   Provide the device eeprom APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef DEVICE_EEPROM_H
#define DEVICE_EEPROM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"

/* functions ----------------------------------------------------------------*/
platform_err_t device_eeprom_init(void);
platform_err_t device_eeprom_write(uint16_t mem_adr,
                                   uint8_t *p_data,
                                   uint16_t size,
                                   uint32_t timeout);
platform_err_t device_eeprom_read(uint16_t mem_adr,
                                  uint8_t *p_data,
                                  uint16_t size,
                                  uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_EEPROM_H */
