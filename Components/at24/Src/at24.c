/**
 ******************************************************************************
 *@file               :   at24.c
 *@brief              :   Portable AT24 EEPROM protocol implementation.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include "at24.h"

/* define   -----------------------------------------------------------------*/
#define AT24_INITIALIZED      (1U)
#define AT24_NOT_INITIALIZED  (0U)
#define AT24_I2C_ADDRESS_MAX  (0x7FU)

/* typedef ------------------------------------------------------------------*/
typedef struct AT24_MODEL_CONFIG_T
{
    uint32_t capacity_bytes;
    uint16_t page_size_bytes;
    uint8_t  word_address_size_bytes;
    uint32_t init_timeout_ms;
    uint32_t write_timeout_ms;
} at24_model_config_t;

/* variables ----------------------------------------------------------------*/
static const at24_model_config_t s_model_config[AT24_MODEL_NUM] = {
    [AT24_MODEL_C02] =
        {
            .capacity_bytes          = 256U,
            .page_size_bytes         = 8U,
            .word_address_size_bytes = 1U,
            .init_timeout_ms         = 100U,
            .write_timeout_ms        = 10U,
        },
};

/* Private  functions  ------------------------------------------------------*/
static uint8_t at24_transport_is_valid(const at24_transport_t *p_transport)
{
    return (uint8_t)((NULL != p_transport) && (NULL != p_transport->pf_write) &&
                     (NULL != p_transport->pf_write_read) &&
                     (NULL != p_transport->pf_wait_ready));
}

static const at24_model_config_t *at24_get_model_config(at24_model_t model)
{
    if((uint32_t)model >= (uint32_t)AT24_MODEL_NUM)
    {
        return NULL;
    }

    return &s_model_config[model];
}

static uint8_t
at24_range_is_valid(const at24_t *p_dev, uint32_t memory_address, uint32_t size)
{
    const at24_model_config_t *p_config = at24_get_model_config(p_dev->model);
    return (uint8_t)((0U != size) &&
                     (memory_address < p_config->capacity_bytes) &&
                     (size <= (p_config->capacity_bytes - memory_address)));
}

static void at24_encode_word_address(const at24_t *p_dev,
                                     uint32_t      memory_address,
                                     uint8_t      *p_buffer)
{
    const at24_model_config_t *p_config = at24_get_model_config(p_dev->model);
    uint8_t                    address_size = p_config->word_address_size_bytes;
    for(uint8_t i = 0U; i < address_size; i++)
    {
        uint8_t shift = (uint8_t)(8U * (address_size - 1U - i));
        p_buffer[i]   = (uint8_t)(memory_address >> shift);
    }
}

static at24_status_t at24_wait_write_complete(at24_t *p_dev)
{
    const at24_model_config_t *p_config = at24_get_model_config(p_dev->model);
    return p_dev->p_transport->pf_wait_ready(p_dev->p_transport_context,
                                             p_dev->address_7b,
                                             p_config->write_timeout_ms);
}

/* Exported functions -------------------------------------------------------*/
at24_status_t at24_init(at24_t                 *p_dev,
                        at24_model_t            model,
                        const at24_transport_t *p_transport,
                        void                   *p_transport_context,
                        uint8_t                 address_7b)
{
    if(NULL == p_dev)
    {
        return AT24_STATUS_PARAM;
    }

    p_dev->initialized                  = AT24_NOT_INITIALIZED;
    const at24_model_config_t *p_config = at24_get_model_config(model);
    if((NULL == p_config) || (0U == at24_transport_is_valid(p_transport)) ||
       (NULL == p_transport_context) || (address_7b > AT24_I2C_ADDRESS_MAX))
    {
        return AT24_STATUS_PARAM;
    }

    p_dev->p_transport         = p_transport;
    p_dev->p_transport_context = p_transport_context;
    p_dev->model               = model;
    p_dev->address_7b          = address_7b;

    at24_status_t status = p_transport
                               ->pf_wait_ready(p_transport_context, address_7b,
                                               p_config->init_timeout_ms);
    if(AT24_STATUS_OK != status)
    {
        return status;
    }

    p_dev->initialized = AT24_INITIALIZED;
    return AT24_STATUS_OK;
}

at24_status_t at24_read(at24_t  *p_dev,
                        uint32_t memory_address,
                        uint8_t *p_data,
                        uint32_t size,
                        uint32_t timeout_ms)
{
    if((NULL == p_dev) || (NULL == p_data) || (0U == timeout_ms) ||
       (UINT32_MAX == timeout_ms))
    {
        return AT24_STATUS_PARAM;
    }
    if(AT24_INITIALIZED != p_dev->initialized)
    {
        return AT24_STATUS_NOT_INITIALIZED;
    }
    if(0U == at24_range_is_valid(p_dev, memory_address, size))
    {
        return AT24_STATUS_RANGE;
    }

    uint32_t                   remaining = size;
    const at24_model_config_t *p_config  = at24_get_model_config(p_dev->model);
    while(0U != remaining)
    {
        uint16_t chunk = (remaining > UINT16_MAX) ? UINT16_MAX
                                                  : (uint16_t)remaining;
        uint8_t  word_address[AT24_MAX_WORD_ADDR_BYTES];
        at24_encode_word_address(p_dev, memory_address, word_address);

        at24_status_t
            status = p_dev->p_transport
                         ->pf_write_read(p_dev->p_transport_context,
                                         p_dev->address_7b, word_address,
                                         p_config->word_address_size_bytes,
                                         p_data, chunk, timeout_ms);
        if(AT24_STATUS_OK != status)
        {
            return status;
        }

        memory_address += chunk;
        p_data += chunk;
        remaining -= chunk;
    }

    return AT24_STATUS_OK;
}

at24_status_t at24_write(at24_t        *p_dev,
                         uint32_t       memory_address,
                         const uint8_t *p_data,
                         uint32_t       size,
                         uint32_t       timeout_ms)
{
    if((NULL == p_dev) || (NULL == p_data) || (0U == timeout_ms) ||
       (UINT32_MAX == timeout_ms))
    {
        return AT24_STATUS_PARAM;
    }
    if(AT24_INITIALIZED != p_dev->initialized)
    {
        return AT24_STATUS_NOT_INITIALIZED;
    }
    if(0U == at24_range_is_valid(p_dev, memory_address, size))
    {
        return AT24_STATUS_RANGE;
    }

    uint32_t                   remaining = size;
    const at24_model_config_t *p_config  = at24_get_model_config(p_dev->model);
    while(0U != remaining)
    {
        uint16_t page_offset    = (uint16_t)(memory_address %
                                             p_config->page_size_bytes);
        uint16_t page_remaining = (uint16_t)(p_config->page_size_bytes -
                                             page_offset);
        uint16_t chunk = (remaining < page_remaining) ? (uint16_t)remaining
                                                      : page_remaining;
        uint8_t  address_size = p_config->word_address_size_bytes;

        at24_encode_word_address(p_dev, memory_address, p_dev->work_buffer);
        (void)memcpy(&p_dev->work_buffer[address_size], p_data, chunk);

        at24_status_t status = p_dev->p_transport
                                   ->pf_write(p_dev->p_transport_context,
                                              p_dev->address_7b,
                                              p_dev->work_buffer,
                                              (uint16_t)(address_size + chunk),
                                              timeout_ms);
        if(AT24_STATUS_OK != status)
        {
            return status;
        }

        status = at24_wait_write_complete(p_dev);
        if(AT24_STATUS_OK != status)
        {
            return status;
        }

        memory_address += chunk;
        p_data += chunk;
        remaining -= chunk;
    }

    return AT24_STATUS_OK;
}

at24_status_t at24_is_ready(at24_t *p_dev)
{
    if(NULL == p_dev)
    {
        return AT24_STATUS_PARAM;
    }
    if(AT24_INITIALIZED != p_dev->initialized)
    {
        return AT24_STATUS_NOT_INITIALIZED;
    }

    const at24_model_config_t *p_config = at24_get_model_config(p_dev->model);
    return p_dev->p_transport->pf_wait_ready(p_dev->p_transport_context,
                                             p_dev->address_7b,
                                             p_config->init_timeout_ms);
}

/* end of file --------------------------------------------------------------*/
