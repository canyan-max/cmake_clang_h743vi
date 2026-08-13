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
#include "at24.h"
#include "plat_i2c.h"
#include "plat_sys.h"
#include "board_config.h"

/* define   -----------------------------------------------------------------*/
#define AT24_READY_PROBE_TIMEOUT_MS  (1U)
#define AT24_READY_POLL_INTERVAL_MS  (1U)

/* typedef ------------------------------------------------------------------*/
typedef struct AT24_PLAT_CONTEXT_T
{
    plat_i2c_id_t i2c_id;
} at24_plat_context_t;

/* variables ----------------------------------------------------------------*/
static at24_t              s_eeprom;
static at24_plat_context_t s_eeprom_context = {
    .i2c_id = PLAT_I2C_ID_0,
};

/* Private  functions  ------------------------------------------------------*/
static at24_status_t at24_transport_convert_platform_error(platform_err_t error)
{
    switch(error)
    {
        case PLATFORM_ERR_OK:
            return AT24_STATUS_OK;

        case PLATFORM_ERR_BUSY:
            return AT24_STATUS_BUSY;

        case PLATFORM_ERR_TIMEOUT:
            return AT24_STATUS_TIMEOUT;

        case PLATFORM_ERR_PARAM:
        case PLATFORM_ERR_HW:
        default:
            return AT24_STATUS_TRANSPORT;
    }
}

static at24_status_t at24_plat_write(void          *p_context,
                                     uint8_t        address_7b,
                                     const uint8_t *p_data,
                                     uint16_t       size,
                                     uint32_t       timeout_ms)
{
    if(NULL == p_context)
    {
        return AT24_STATUS_TRANSPORT;
    }

    const at24_plat_context_t *p_ctx = (const at24_plat_context_t *)p_context;
    return at24_transport_convert_platform_error(
        plat_i2c_write(p_ctx->i2c_id, address_7b, p_data, size, timeout_ms));
}

static at24_status_t at24_plat_write_read(void          *p_context,
                                          uint8_t        address_7b,
                                          const uint8_t *p_tx_data,
                                          uint16_t       tx_size,
                                          uint8_t       *p_rx_data,
                                          uint16_t       rx_size,
                                          uint32_t       timeout_ms)
{
    if(NULL == p_context)
    {
        return AT24_STATUS_TRANSPORT;
    }

    const at24_plat_context_t *p_ctx = (const at24_plat_context_t *)p_context;
    return at24_transport_convert_platform_error(
        plat_i2c_write_read(p_ctx->i2c_id, address_7b, p_tx_data, tx_size,
                            p_rx_data, rx_size, timeout_ms));
}

/**
 * @brief Poll device acknowledgement until ready or the total timeout expires.
 */
static at24_status_t
at24_plat_wait_ready(void *p_context, uint8_t address_7b, uint32_t timeout_ms)
{
    if((NULL == p_context) || (0U == timeout_ms) || (UINT32_MAX == timeout_ms))
    {
        return AT24_STATUS_PARAM;
    }

    const at24_plat_context_t *p_ctx = (const at24_plat_context_t *)p_context;
    uint32_t                   start_ms = plat_tick_get_ms();
    for(;;)
    {
        platform_err_t error = plat_i2c_is_ready(p_ctx->i2c_id, address_7b,
                                                 AT24_READY_PROBE_TIMEOUT_MS);
        if(PLATFORM_ERR_OK == error)
        {
            return AT24_STATUS_OK;
        }
        if((PLATFORM_ERR_BUSY != error) && (PLATFORM_ERR_TIMEOUT != error))
        {
            return at24_transport_convert_platform_error(error);
        }
        if((plat_tick_get_ms() - start_ms) >= timeout_ms)
        {
            return AT24_STATUS_TIMEOUT;
        }

        plat_delay_ms(AT24_READY_POLL_INTERVAL_MS);
    }
}

static const at24_transport_t s_at24_transport = {
    .pf_write      = at24_plat_write,
    .pf_write_read = at24_plat_write_read,
    .pf_wait_ready = at24_plat_wait_ready,
};

static platform_err_t bsp_eeprom_convert_at24_status(at24_status_t status)
{
    switch(status)
    {
        case AT24_STATUS_OK:
            return PLATFORM_ERR_OK;

        case AT24_STATUS_PARAM:
        case AT24_STATUS_RANGE:
            return PLATFORM_ERR_PARAM;

        case AT24_STATUS_BUSY:
            return PLATFORM_ERR_BUSY;

        case AT24_STATUS_TIMEOUT:
            return PLATFORM_ERR_TIMEOUT;

        case AT24_STATUS_NOT_INITIALIZED:
        case AT24_STATUS_TRANSPORT:
        default:
            return PLATFORM_ERR_HW;
    }
}

/* Exported functions -------------------------------------------------------*/
platform_err_t bsp_eeprom_init(void)
{
    return bsp_eeprom_convert_at24_status(
        at24_init(&s_eeprom, AT24_MODEL_C02, &s_at24_transport,
                  &s_eeprom_context, BOARD_EEPROM_I2C_ADDRESS_7B));
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

    return bsp_eeprom_convert_at24_status(
        at24_read(&s_eeprom, memory_address, p_data, size, timeout_ms));
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

    return bsp_eeprom_convert_at24_status(
        at24_write(&s_eeprom, memory_address, p_data, size, timeout_ms));
}

/* end of file --------------------------------------------------------------*/
