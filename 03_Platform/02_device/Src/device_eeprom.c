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
#include "bsp_drv_at24.h"
#include "plat_tick.h"
#include "board_config.h"

/* define   -----------------------------------------------------------------*/
#define AT24C02_MAX_BYTE_ADDR   BOARD_EEPROM_MAX_BYTE_ADDR
#define AT24C02_PAGE_SIZE       BOARD_EEPROM_PAGE_SIZE
#define AT24C02_ADR_SIZE        BOARD_EEPROM_ADR_SIZE
#define AT24C02_DEV_ADR         BOARD_EEPROM_DEV_ADDR

/* variables ----------------------------------------------------------------*/
static at24_dev_t g_eeprom;

/* private  functions  ------------------------------------------------------*/
static platform_err_t write_page(uint16_t mem_adr,
                                 uint8_t *p_data,
                                 uint16_t size,
                                 uint32_t timeout)
{
    if(NULL == p_data || 0U == size)
    {
        return PLATFORM_ERR_PARAM;
    }
    at24_state_t ret = at24_write_page(&g_eeprom, mem_adr, p_data, size,
                                              timeout);
    return (AT24_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

/* exported functions -------------------------------------------------------*/
platform_err_t device_eeprom_init(void)
{
    at24_state_t ret = bsp_at24_init(&g_eeprom,
                                      AT24C02_MAX_BYTE_ADDR,
                                      AT24C02_PAGE_SIZE,
                                      AT24C02_ADR_SIZE,
                                      AT24C02_DEV_ADR);
    return (AT24_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

platform_err_t device_eeprom_write(uint16_t mem_adr,
                                   uint8_t *p_data,
                                   uint16_t size,
                                   uint32_t timeout)
{
    uint16_t     left_size   = size;
    uint16_t     current_adr = mem_adr;
    uint8_t     *p_cursor    = p_data;
    platform_err_t ret       = PLATFORM_ERR_OK;

    if(NULL == p_data || 0U == size)
    {
        return PLATFORM_ERR_PARAM;
    }
    if((mem_adr >= AT24C02_MAX_BYTE_ADDR) ||
       ((uint32_t)mem_adr + (uint32_t)size) > AT24C02_MAX_BYTE_ADDR)
    {
        return PLATFORM_ERR_PARAM;
    }

    while(0U != left_size)
    {
        uint16_t page_remain = AT24C02_PAGE_SIZE;
        page_remain -= (uint16_t)(current_adr % AT24C02_PAGE_SIZE);
        uint16_t chunk_size = (left_size < page_remain) ? left_size : page_remain;

        ret = write_page(current_adr, p_cursor, chunk_size, timeout);
        if(PLATFORM_ERR_OK != ret)
        {
            return ret;
        }
        current_adr += chunk_size;
        p_cursor    += chunk_size;
        left_size   -= chunk_size;
        if(0U == left_size)
        {
            break;
        }

        uint32_t start        = plat_tick_get_ms();
        uint8_t  timeout_flag = 1U;
        while((plat_tick_get_ms() - start) <= 5U)
        {
            if(AT24_OK == g_eeprom.iic_ops->pf_iic_dev_isready(
                              g_eeprom.iic_ops->p_iic_handle,
                              AT24C02_DEV_ADR, 1U, 0U))
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

platform_err_t device_eeprom_read(uint16_t mem_adr,
                                  uint8_t *p_data,
                                  uint16_t size,
                                  uint32_t timeout)
{
    if(NULL == p_data || 0U == size)
    {
        return PLATFORM_ERR_PARAM;
    }
    at24_state_t ret = at24_read_bytes(&g_eeprom, mem_adr, p_data, size,
                                              timeout);
    return (AT24_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

/* end of  file -------------------------------------------------------------*/
