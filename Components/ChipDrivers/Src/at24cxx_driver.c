/**
 ******************************************************************************
 *@file               :   at24cxx_driver.c
 *@brief              :   Portable AT24Cxx EEPROM protocol implementation.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <limits.h>
#include <stddef.h>
#include "at24cxx_driver.h"

/* define   -----------------------------------------------------------------*/
#define AT24CXX_INITIALIZED              (1U)
#define AT24CXX_NOT_INITIALIZED          (0U)
#define AT24CXX_I2C_ADDRESS_MAX          (0x7FU)
#define AT24CXX_C02_ADDRESS_SPACE_BYTES  (256U)
#define AT24CXX_C02_WORD_ADDRESS_BYTES   (1U)

/* typedef ------------------------------------------------------------------*/
typedef struct AT24CXX_RESOLVED_ADDRESS_T
{
    uint16_t word_address;
    uint8_t  device_address_7b;
    uint8_t  word_address_size_bytes;
} at24cxx_resolved_address_t;

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/
static uint8_t
at24cxx_transport_is_valid(const at24cxx_transport_t *p_transport)
{
    return (uint8_t)((NULL != p_transport) && (NULL != p_transport->pf_write) &&
                     (NULL != p_transport->pf_read) &&
                     (NULL != p_transport->pf_wait_ready));
}

static uint8_t at24cxx_timeout_is_valid(uint32_t timeout_ms)
{
    return (uint8_t)((0U != timeout_ms) && (UINT32_MAX != timeout_ms));
}

static uint8_t at24cxx_range_is_valid(const at24cxx_driver_t *p_driver,
                                      uint32_t                memory_address,
                                      uint32_t                size)
{
    return (uint8_t)((0U != size) &&
                     (memory_address < p_driver->capacity_bytes) &&
                     (size <= (p_driver->capacity_bytes - memory_address)));
}

/**
 * @brief Resolve one logical EEPROM address into one I2C memory transaction.
 */
static at24cxx_status_t
at24cxx_resolve_address(const at24cxx_driver_t     *p_driver,
                        uint32_t                    memory_address,
                        at24cxx_resolved_address_t *p_resolved)
{
    switch(p_driver->model)
    {
        case AT24CXX_MODEL_C02:
            if(memory_address >= AT24CXX_C02_ADDRESS_SPACE_BYTES)
            {
                return AT24CXX_STATUS_RANGE;
            }
            p_resolved->device_address_7b = p_driver->address_7b;
            p_resolved->word_address      = (uint16_t)memory_address;
            p_resolved
                ->word_address_size_bytes = AT24CXX_C02_WORD_ADDRESS_BYTES;
            return AT24CXX_STATUS_OK;

        case AT24CXX_MODEL_NUM:
        default:
            return AT24CXX_STATUS_PARAM;
    }
}

/* Exported functions -------------------------------------------------------*/
at24cxx_status_t at24cxx_init(at24cxx_driver_t          *p_driver,
                              const at24cxx_transport_t *p_transport,
                              at24cxx_model_t            model,
                              uint8_t                    address_7b,
                              uint32_t                   capacity_bytes,
                              uint16_t                   page_size_bytes,
                              uint32_t                   timeout_ms)
{
    if(NULL == p_driver)
    {
        return AT24CXX_STATUS_PARAM;
    }

    p_driver->initialized = AT24CXX_NOT_INITIALIZED;
    if((0U == at24cxx_transport_is_valid(p_transport)) ||
       (AT24CXX_MODEL_C02 != model) || (address_7b > AT24CXX_I2C_ADDRESS_MAX) ||
       (0U == capacity_bytes) ||
       (capacity_bytes > AT24CXX_C02_ADDRESS_SPACE_BYTES) ||
       (0U == page_size_bytes) || (page_size_bytes > capacity_bytes) ||
       (0U == at24cxx_timeout_is_valid(timeout_ms)))
    {
        return AT24CXX_STATUS_PARAM;
    }

    p_driver->p_transport     = p_transport;
    p_driver->capacity_bytes  = capacity_bytes;
    p_driver->page_size_bytes = page_size_bytes;
    p_driver->model           = model;
    p_driver->address_7b      = address_7b;

    at24cxx_status_t status = p_transport->pf_wait_ready(address_7b,
                                                         timeout_ms);
    if(AT24CXX_STATUS_OK != status)
    {
        return status;
    }

    p_driver->initialized = AT24CXX_INITIALIZED;
    return AT24CXX_STATUS_OK;
}

at24cxx_status_t at24cxx_read(at24cxx_driver_t *p_driver,
                              uint32_t          memory_address,
                              uint8_t          *p_data,
                              uint32_t          size,
                              uint32_t          timeout_ms)
{
    if((NULL == p_driver) || (NULL == p_data) ||
       (0U == at24cxx_timeout_is_valid(timeout_ms)))
    {
        return AT24CXX_STATUS_PARAM;
    }
    if(AT24CXX_INITIALIZED != p_driver->initialized)
    {
        return AT24CXX_STATUS_NOT_INITIALIZED;
    }
    if(0U == at24cxx_range_is_valid(p_driver, memory_address, size))
    {
        return AT24CXX_STATUS_RANGE;
    }

    uint32_t remaining = size;
    while(0U != remaining)
    {
        uint16_t chunk = (remaining > UINT16_MAX) ? UINT16_MAX
                                                  : (uint16_t)remaining;
        at24cxx_resolved_address_t resolved;
        at24cxx_status_t status = at24cxx_resolve_address(p_driver,
                                                          memory_address,
                                                          &resolved);
        if(AT24CXX_STATUS_OK != status)
        {
            return status;
        }

        status = p_driver->p_transport
                     ->pf_read(resolved.device_address_7b,
                               resolved.word_address,
                               resolved.word_address_size_bytes, p_data, chunk,
                               timeout_ms);
        if(AT24CXX_STATUS_OK != status)
        {
            return status;
        }

        memory_address += chunk;
        p_data += chunk;
        remaining -= chunk;
    }

    return AT24CXX_STATUS_OK;
}

at24cxx_status_t at24cxx_write(at24cxx_driver_t *p_driver,
                               uint32_t          memory_address,
                               const uint8_t    *p_data,
                               uint32_t          size,
                               uint32_t          timeout_ms)
{
    if((NULL == p_driver) || (NULL == p_data) ||
       (0U == at24cxx_timeout_is_valid(timeout_ms)))
    {
        return AT24CXX_STATUS_PARAM;
    }
    if(AT24CXX_INITIALIZED != p_driver->initialized)
    {
        return AT24CXX_STATUS_NOT_INITIALIZED;
    }
    if(0U == at24cxx_range_is_valid(p_driver, memory_address, size))
    {
        return AT24CXX_STATUS_RANGE;
    }

    uint32_t remaining = size;
    while(0U != remaining)
    {
        uint16_t page_offset    = (uint16_t)(memory_address %
                                             p_driver->page_size_bytes);
        uint16_t page_remaining = (uint16_t)(p_driver->page_size_bytes -
                                             page_offset);
        uint16_t chunk = (remaining < page_remaining) ? (uint16_t)remaining
                                                      : page_remaining;
        at24cxx_resolved_address_t resolved;
        at24cxx_status_t status = at24cxx_resolve_address(p_driver,
                                                          memory_address,
                                                          &resolved);
        if(AT24CXX_STATUS_OK != status)
        {
            return status;
        }

        status = p_driver->p_transport
                     ->pf_write(resolved.device_address_7b,
                                resolved.word_address,
                                resolved.word_address_size_bytes, p_data, chunk,
                                timeout_ms);
        if(AT24CXX_STATUS_OK != status)
        {
            return status;
        }

        status = p_driver->p_transport
                     ->pf_wait_ready(resolved.device_address_7b, timeout_ms);
        if(AT24CXX_STATUS_OK != status)
        {
            return status;
        }

        memory_address += chunk;
        p_data += chunk;
        remaining -= chunk;
    }

    return AT24CXX_STATUS_OK;
}

at24cxx_status_t at24cxx_is_ready(at24cxx_driver_t *p_driver,
                                  uint32_t          timeout_ms)
{
    if((NULL == p_driver) || (0U == at24cxx_timeout_is_valid(timeout_ms)))
    {
        return AT24CXX_STATUS_PARAM;
    }
    if(AT24CXX_INITIALIZED != p_driver->initialized)
    {
        return AT24CXX_STATUS_NOT_INITIALIZED;
    }

    return p_driver->p_transport->pf_wait_ready(p_driver->address_7b,
                                                timeout_ms);
}

/* end of file --------------------------------------------------------------*/
