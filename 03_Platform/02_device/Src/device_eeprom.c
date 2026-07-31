/**
 ******************************************************************************
 *@file               :   device_eeprom.c
 *@brief              :   Provide the device eeprom APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "device_eeprom.h"
#include "eeprom_intf.h"

/* exported functions -------------------------------------------------------*/
platform_err_t device_eeprom_init(void)
{
    return g_eeprom_bsp_ops.pf_init();
}

platform_err_t device_eeprom_write(uint16_t mem_adr,
                                   uint8_t *p_data,
                                   uint16_t size,
                                   uint32_t timeout)
{
    if(NULL == p_data || 0U == size)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(mem_adr >= BSP_EEPROM_MAX_BYTE_ADDR ||
       ((uint32_t)mem_adr + (uint32_t)size) > BSP_EEPROM_MAX_BYTE_ADDR)
    {
        return PLATFORM_ERR_PARAM;
    }
    return g_eeprom_bsp_ops.pf_write(mem_adr, p_data, size, timeout);
}

platform_err_t device_eeprom_read(uint16_t mem_adr,
                                  uint8_t *p_data,
                                  uint16_t size,
                                  uint32_t timeout)
{
    if(NULL == p_data || 0U == size)
    {
        return PLATFORM_ERR_PARAM;
    }
    return g_eeprom_bsp_ops.pf_read(mem_adr, p_data, size, timeout);
}

/* end of  file -------------------------------------------------------------*/
