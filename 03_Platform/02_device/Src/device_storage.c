/**
 ******************************************************************************
 *@file               :   device_storage.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>         /* stdint lib header file. */
#include "device_storage.h" /* device_storage lib header file. */
#include "bsp_drv_at24.h"
#include "plat_tick.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static at24_driver_t g_eeprom;
/* private  functions  ------------------------------------------------------*/

/* exported functions -------------------------------------------------------*/

platform_err_t device_storage_init(const void *p_iic_ops,
                                   uint32_t    max_byte_addr,
                                   uint32_t    page_size,
                                   uint16_t    adr_size,
                                   uint8_t     dev_adr)
{
    if(NULL == p_iic_ops)
    {
        return PLATFORM_ERR_PARAM;
    }
    at24_state_t ret = at24_driver_instruct(&g_eeprom,
                                            (const iic_ops_t *)p_iic_ops,
                                            max_byte_addr, page_size, adr_size,
                                            dev_adr);
    return (AT24_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_storage_write_page(uint16_t mem_adr,
                                         uint8_t *p_data,
                                         uint16_t size,
                                         uint32_t timeout)
{
    if(NULL == p_data || 0U == size)
    {
        return PLATFORM_ERR_PARAM;
    }
    at24_state_t ret = g_eeprom.pf_write_page(&g_eeprom, mem_adr, p_data, size,
                                              timeout);
    return (AT24_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_storage_write_cross_page(uint16_t mem_adr,
                                               uint8_t *p_data,
                                               uint16_t size,
                                               uint32_t timeout)
{
    uint16_t     left_size   = size;
    uint16_t     current_adr = mem_adr;
    uint8_t     *p_cursor    = p_data;
    at24_state_t ret         = AT24_OK;

    if(NULL == p_data || 0U == size)
    {
        return PLATFORM_ERR_PARAM;
    }

    if((mem_adr >= g_eeprom.max_byte_addr) ||
       ((uint32_t)mem_adr + (uint32_t)size) > g_eeprom.max_byte_addr)
    {
        return PLATFORM_ERR_PARAM;
    }

    while(0U != left_size)
    {
        uint16_t page_remain = g_eeprom.page_size;
        uint16_t chunk_size  = 0U;
        page_remain -= (uint16_t)(current_adr % g_eeprom.page_size);
        chunk_size = (left_size < page_remain) ? left_size : page_remain;

        ret = g_eeprom.pf_write_page(&g_eeprom, current_adr, p_cursor,
                                     chunk_size, timeout);
        if(AT24_OK != ret)
        {
            return PLATFORM_ERR_HW;
        }
        current_adr += chunk_size;
        p_cursor += chunk_size;
        left_size -= chunk_size;
        if(0U == left_size)
        {
            break;
        }

        uint32_t start        = plat_tick_get_ms();
        uint8_t  timeout_flag = 1U;
        while((plat_tick_get_ms() - start) <= 5U)
        {
            if(AT24_OK ==
               g_eeprom.iic_ops
                   ->pf_iic_dev_isready(g_eeprom.iic_ops->p_iic_handle,
                                        g_eeprom.dev_adr, 1U, 0U))
            {
                timeout_flag = 0U;
                break;
            }
        }
        if(timeout_flag)
        {
            return PLATFORM_ERR_TIMEOUT;
        }
    }
    return PLATFORM_ERR_OK;
}

platform_err_t device_storage_read(uint16_t mem_adr,
                                   uint8_t *p_data,
                                   uint16_t size,
                                   uint32_t timeout)
{
    if(NULL == p_data || 0U == size)
    {
        return PLATFORM_ERR_PARAM;
    }
    at24_state_t ret = g_eeprom.pf_read_bytes(&g_eeprom, mem_adr, p_data, size,
                                              timeout);
    return (AT24_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

/* end of  file -------------------------------------------------------------*/
