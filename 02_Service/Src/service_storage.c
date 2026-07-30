/**
 ******************************************************************************
 *@file               :   service_storage.c
 *@brief              :   Provide the service storage APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "service_storage.h"
#include "device_eeprom.h"

/* exported functions -------------------------------------------------------*/
platform_err_t service_storage_init(void)
{
    return device_eeprom_init();
}

platform_err_t service_storage_write(uint16_t addr,
                                     uint8_t *p_data,
                                     uint16_t size,
                                     uint32_t timeout)
{
    return device_eeprom_write(addr, p_data, size, timeout);
}

platform_err_t service_storage_read(uint16_t addr,
                                    uint8_t *p_data,
                                    uint16_t size,
                                    uint32_t timeout)
{
    return device_eeprom_read(addr, p_data, size, timeout);
}

/* end of file --------------------------------------------------------------*/
