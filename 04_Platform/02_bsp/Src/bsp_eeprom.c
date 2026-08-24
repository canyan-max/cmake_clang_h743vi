/**
 ******************************************************************************
 *@file               :   bsp_eeprom.c
 *@brief              :   Board AT24 EEPROM instance and Platform adapter.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <limits.h>
#include <stddef.h>
#include "bsp_eeprom.h"
#include "at24cxx_driver.h"
#include "plat_i2c.h"
#include "plat_sys.h"
#include "board_resources.h"

/* define   -----------------------------------------------------------------*/
#define BSP_EEPROM_CAPACITY_BYTES        (256U)
#define BSP_EEPROM_PAGE_SIZE_BYTES       (8U)
#define BSP_EEPROM_INIT_TIMEOUT_MS       (100U)
#define AT24CXX_READY_PROBE_TIMEOUT_MS   (1U)
#define AT24CXX_READY_POLL_INTERVAL_MS   (1U)

/* variables ----------------------------------------------------------------*/
static at24cxx_driver_t s_eeprom;

/* Private  functions  ------------------------------------------------------*/
static at24cxx_status_t
at24cxx_transport_convert_platform_error(platform_err_t error)
{
    switch(error)
    {
        case PLATFORM_ERR_OK:
            return AT24CXX_STATUS_OK;

        case PLATFORM_ERR_BUSY:
            return AT24CXX_STATUS_BUSY;

        case PLATFORM_ERR_TIMEOUT:
            return AT24CXX_STATUS_TIMEOUT;

        case PLATFORM_ERR_PARAM:
        case PLATFORM_ERR_HW:
        default:
            return AT24CXX_STATUS_TRANSPORT;
    }
}

static at24cxx_status_t at24cxx_plat_write(uint8_t  address_7b,
                                           uint16_t word_address,
                                           uint8_t  word_address_size_bytes,
                                           const uint8_t *p_data,
                                           uint16_t       size,
                                           uint32_t       timeout_ms)
{
    plat_i2c_memory_address_size_t address_size;
    switch(word_address_size_bytes)
    {
        case 1U:
            address_size = PLAT_I2C_MEMORY_ADDRESS_SIZE_8BIT;
            break;

        case 2U:
            address_size = PLAT_I2C_MEMORY_ADDRESS_SIZE_16BIT;
            break;

        default:
            return AT24CXX_STATUS_PARAM;
    }

    return at24cxx_transport_convert_platform_error(
        plat_i2c_memory_write(BOARD_I2C_EEPROM_BUS, address_7b, word_address,
                              address_size, p_data, size, timeout_ms));
}

static at24cxx_status_t at24cxx_plat_read(uint8_t  address_7b,
                                          uint16_t word_address,
                                          uint8_t  word_address_size_bytes,
                                          uint8_t *p_data,
                                          uint16_t size,
                                          uint32_t timeout_ms)
{
    plat_i2c_memory_address_size_t address_size;
    switch(word_address_size_bytes)
    {
        case 1U:
            address_size = PLAT_I2C_MEMORY_ADDRESS_SIZE_8BIT;
            break;

        case 2U:
            address_size = PLAT_I2C_MEMORY_ADDRESS_SIZE_16BIT;
            break;

        default:
            return AT24CXX_STATUS_PARAM;
    }

    return at24cxx_transport_convert_platform_error(
        plat_i2c_memory_read(BOARD_I2C_EEPROM_BUS, address_7b, word_address,
                             address_size, p_data, size, timeout_ms));
}

/**
 * @brief Poll device acknowledgement until ready or the total timeout expires.
 */
static at24cxx_status_t at24cxx_plat_wait_ready(uint8_t  address_7b,
                                                uint32_t timeout_ms)
{
    if((0U == timeout_ms) || (UINT32_MAX == timeout_ms))
    {
        return AT24CXX_STATUS_PARAM;
    }

    uint32_t start_ms = plat_tick_get_ms();
    for(;;)
    {
        platform_err_t
            error = plat_i2c_is_ready(BOARD_I2C_EEPROM_BUS, address_7b,
                                      AT24CXX_READY_PROBE_TIMEOUT_MS);
        if(PLATFORM_ERR_OK == error)
        {
            return AT24CXX_STATUS_OK;
        }
        if((PLATFORM_ERR_BUSY != error) && (PLATFORM_ERR_TIMEOUT != error))
        {
            return at24cxx_transport_convert_platform_error(error);
        }
        if((plat_tick_get_ms() - start_ms) >= timeout_ms)
        {
            return AT24CXX_STATUS_TIMEOUT;
        }

        plat_delay_ms(AT24CXX_READY_POLL_INTERVAL_MS);
    }
}

static const at24cxx_transport_t s_at24cxx_transport = {
    .pf_write      = at24cxx_plat_write,
    .pf_read       = at24cxx_plat_read,
    .pf_wait_ready = at24cxx_plat_wait_ready,
};

static platform_err_t bsp_eeprom_convert_at24cxx_status(at24cxx_status_t status)
{
    switch(status)
    {
        case AT24CXX_STATUS_OK:
            return PLATFORM_ERR_OK;

        case AT24CXX_STATUS_PARAM:
        case AT24CXX_STATUS_RANGE:
            return PLATFORM_ERR_PARAM;

        case AT24CXX_STATUS_BUSY:
            return PLATFORM_ERR_BUSY;

        case AT24CXX_STATUS_TIMEOUT:
            return PLATFORM_ERR_TIMEOUT;

        case AT24CXX_STATUS_NOT_INITIALIZED:
        case AT24CXX_STATUS_TRANSPORT:
        default:
            return PLATFORM_ERR_HW;
    }
}

/* Exported functions -------------------------------------------------------*/
platform_err_t bsp_eeprom_init(void)
{
    return bsp_eeprom_convert_at24cxx_status(
        at24cxx_init(&s_eeprom, &s_at24cxx_transport, AT24CXX_MODEL_C02,
                     BOARD_EEPROM_I2C_ADDRESS_7B, BSP_EEPROM_CAPACITY_BYTES,
                     BSP_EEPROM_PAGE_SIZE_BYTES, BSP_EEPROM_INIT_TIMEOUT_MS));
}

platform_err_t bsp_eeprom_read(uint16_t memory_address,
                               uint8_t *p_data,
                               uint16_t size,
                               uint32_t timeout_ms)
{
    if((NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    return bsp_eeprom_convert_at24cxx_status(
        at24cxx_read(&s_eeprom, memory_address, p_data, size, timeout_ms));
}

platform_err_t bsp_eeprom_write(uint16_t       memory_address,
                                const uint8_t *p_data,
                                uint16_t       size,
                                uint32_t       timeout_ms)
{
    if((NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    return bsp_eeprom_convert_at24cxx_status(
        at24cxx_write(&s_eeprom, memory_address, p_data, size, timeout_ms));
}

/* end of file --------------------------------------------------------------*/
