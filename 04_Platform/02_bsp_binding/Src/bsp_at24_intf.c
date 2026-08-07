/**
 ******************************************************************************
 *@file               :   bsp_at24_intf.c
 *@brief              :   AT24 implementation of eeprom_intf.
 *                        Swap this file when replacing EEPROM chip.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "bsp_at24.h"
#include "eeprom_intf.h"
#include "plat_sys.h"

/* variables ----------------------------------------------------------------*/
static at24_dev_t g_at24_dev;

/* Private functions --------------------------------------------------------*/
static platform_err_t eeprom_bsp_init(void)
{
    at24_state_t ret = bsp_at24_init(&g_at24_dev);
    return (AT24_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t eeprom_bsp_read(uint16_t mem_adr, uint8_t *p_data,
                                       uint16_t size, uint32_t timeout)
{
    at24_state_t ret = at24_read_bytes(&g_at24_dev, mem_adr, p_data, size,
                                        timeout);
    return (AT24_OK == ret) ? PLATFORM_ERR_OK : PLATFORM_ERR_HW;
}

static platform_err_t eeprom_bsp_write(uint16_t mem_adr, uint8_t *p_data,
                                        uint16_t size, uint32_t timeout)
{
    uint16_t left_size   = size;
    uint16_t current_adr = mem_adr;
    uint8_t *p_cursor    = p_data;

    while(0U != left_size)
    {
        uint16_t page_remain = AT24C02_PAGE_SIZE -
                               (uint16_t)(current_adr % AT24C02_PAGE_SIZE);
        uint16_t chunk = (left_size < page_remain) ? left_size : page_remain;

        at24_state_t ret = at24_write_page(&g_at24_dev, current_adr,
                                            p_cursor, chunk, timeout);
        if(AT24_OK != ret)
        {
            return PLATFORM_ERR_HW;
        }

        current_adr += chunk;
        p_cursor    += chunk;
        left_size   -= chunk;
        if(0U == left_size)
        {
            break;
        }

        // watiing max time out of atdevice 5ms 
        uint32_t start = plat_tick_get_ms();
        uint8_t  ready = 0U;
        while((plat_tick_get_ms() - start) <= 5U)
        {
            if(AT24_OK == g_at24_dev.iic_ops->pf_iic_dev_isready(
                              AT24C02_DEV_ADDR, 1U, 0U))
            {
                ready = 1U;
                break;
            }
        }
        if(0U == ready)
        {
            return PLATFORM_ERR_TIMEOUT;
        }
    }
    return PLATFORM_ERR_OK;
}

/* Exported variables -------------------------------------------------------*/
const bsp_eeprom_ops_t g_eeprom_bsp_ops = {
    .pf_init  = eeprom_bsp_init,
    .pf_read  = eeprom_bsp_read,
    .pf_write = eeprom_bsp_write,
};

/* end of file --------------------------------------------------------------*/
