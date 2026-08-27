/**
 ******************************************************************************
 *@file               :   modbus_rtu.c
 *@brief              :   Modbus RTU function 0x03/0x06 implementation.
 ******************************************************************************
 */

#include <stddef.h>
#include "modbus_rtu.h"

#define MODBUS_RTU_REQUEST_SIZE       (8U)
#define MODBUS_RTU_EXCEPTION_SIZE     (5U)
#define MODBUS_RTU_READ_RESPONSE_BASE (5U)

static uint16_t get_u16_be(const uint8_t *p_data)
{
    return (uint16_t)((uint16_t)((uint16_t)p_data[0] << 8U) | p_data[1]);
}

static void put_u16_be(uint8_t *p_data, uint16_t value)
{
    p_data[0] = (uint8_t)(value >> 8U);
    p_data[1] = (uint8_t)(value & 0xFFU);
}

static void append_crc(uint8_t *p_adu, uint16_t payload_size)
{
    uint16_t crc = modbus_rtu_crc16(p_adu, payload_size);
    p_adu[payload_size] = (uint8_t)(crc & 0xFFU);
    p_adu[payload_size + 1U] = (uint8_t)(crc >> 8U);
}

static modbus_rtu_status_t validate_crc(const uint8_t *p_adu,
                                        uint16_t adu_size)
{
    if((NULL == p_adu) || (adu_size < MODBUS_RTU_EXCEPTION_SIZE))
    {
        return MODBUS_RTU_ERR_PARAM;
    }
    uint16_t received_crc = (uint16_t)p_adu[adu_size - 2U] |
                            (uint16_t)((uint16_t)p_adu[adu_size - 1U] << 8U);
    return (received_crc == modbus_rtu_crc16(p_adu,
                                             (uint16_t)(adu_size - 2U)))
               ? MODBUS_RTU_OK
               : MODBUS_RTU_ERR_CRC;
}

static modbus_rtu_status_t parse_exception(uint8_t expected_address,
                                           uint8_t expected_function,
                                           const uint8_t *p_adu,
                                           uint16_t adu_size,
                                           modbus_rtu_exception_t *p_exception)
{
    if(NULL != p_exception)
    {
        *p_exception = MODBUS_RTU_EXCEPTION_NONE;
    }
    if((MODBUS_RTU_EXCEPTION_SIZE == adu_size) &&
       (expected_address == p_adu[0]) &&
       ((uint8_t)(expected_function | 0x80U) == p_adu[1]))
    {
        if(NULL != p_exception)
        {
            *p_exception = (modbus_rtu_exception_t)p_adu[2];
        }
        return MODBUS_RTU_ERR_EXCEPTION;
    }
    return MODBUS_RTU_ERR_FRAME;
}

static modbus_rtu_status_t build_exception_response(
    uint8_t address,
    uint8_t function,
    modbus_rtu_exception_t exception,
    uint8_t *p_response,
    uint16_t response_capacity,
    uint16_t *p_response_size)
{
    if((NULL == p_response) || (NULL == p_response_size) ||
       (response_capacity < MODBUS_RTU_EXCEPTION_SIZE))
    {
        return MODBUS_RTU_ERR_BUFFER_SIZE;
    }
    p_response[0] = address;
    p_response[1] = (uint8_t)(function | 0x80U);
    p_response[2] = (uint8_t)exception;
    append_crc(p_response, 3U);
    *p_response_size = MODBUS_RTU_EXCEPTION_SIZE;
    return MODBUS_RTU_OK;
}

uint16_t modbus_rtu_crc16(const uint8_t *p_data, uint16_t size)
{
    if((NULL == p_data) || (0U == size))
    {
        return 0U;
    }
    uint16_t crc = 0xFFFFU;
    for(uint16_t i = 0U; i < size; i++)
    {
        crc ^= p_data[i];
        for(uint8_t bit = 0U; bit < 8U; bit++)
        {
            crc = (0U != (crc & 1U))
                      ? (uint16_t)((crc >> 1U) ^ 0xA001U)
                      : (uint16_t)(crc >> 1U);
        }
    }
    return crc;
}

modbus_rtu_status_t modbus_rtu_build_read_holding_request(
    uint8_t slave_address,
    uint16_t start_address,
    uint16_t quantity,
    uint8_t *p_adu,
    uint16_t adu_capacity,
    uint16_t *p_adu_size)
{
    if((0U == slave_address) ||
       (slave_address > MODBUS_RTU_MAX_SLAVE_ADDRESS) ||
       (NULL == p_adu) || (NULL == p_adu_size) ||
       (0U == quantity) || (quantity > MODBUS_RTU_MAX_READ_REGISTERS))
    {
        return MODBUS_RTU_ERR_PARAM;
    }
    if(adu_capacity < MODBUS_RTU_REQUEST_SIZE)
    {
        return MODBUS_RTU_ERR_BUFFER_SIZE;
    }
    p_adu[0] = slave_address;
    p_adu[1] = MODBUS_RTU_FC_READ_HOLDING_REGISTERS;
    put_u16_be(&p_adu[2], start_address);
    put_u16_be(&p_adu[4], quantity);
    append_crc(p_adu, 6U);
    *p_adu_size = MODBUS_RTU_REQUEST_SIZE;
    return MODBUS_RTU_OK;
}

modbus_rtu_status_t modbus_rtu_parse_read_holding_response(
    uint8_t expected_slave_address,
    uint16_t expected_quantity,
    const uint8_t *p_adu,
    uint16_t adu_size,
    uint16_t *p_values,
    uint16_t value_capacity,
    modbus_rtu_exception_t *p_exception)
{
    if(NULL != p_exception)
    {
        *p_exception = MODBUS_RTU_EXCEPTION_NONE;
    }
    if((0U == expected_slave_address) ||
       (expected_slave_address > MODBUS_RTU_MAX_SLAVE_ADDRESS) ||
       (0U == expected_quantity) ||
       (expected_quantity > MODBUS_RTU_MAX_READ_REGISTERS) ||
       (NULL == p_adu) || (NULL == p_values) ||
       (value_capacity < expected_quantity))
    {
        return MODBUS_RTU_ERR_PARAM;
    }
    modbus_rtu_status_t status = validate_crc(p_adu, adu_size);
    if(MODBUS_RTU_OK != status)
    {
        return status;
    }
    if((MODBUS_RTU_EXCEPTION_SIZE == adu_size) &&
       (0U != (p_adu[1] & 0x80U)))
    {
        return parse_exception(expected_slave_address,
                               MODBUS_RTU_FC_READ_HOLDING_REGISTERS,
                               p_adu, adu_size, p_exception);
    }
    uint16_t expected_size = (uint16_t)(MODBUS_RTU_READ_RESPONSE_BASE +
                                        (2U * expected_quantity));
    if((adu_size != expected_size) ||
       (expected_slave_address != p_adu[0]) ||
       (MODBUS_RTU_FC_READ_HOLDING_REGISTERS != p_adu[1]) ||
       ((uint16_t)p_adu[2] != (2U * expected_quantity)))
    {
        return MODBUS_RTU_ERR_FRAME;
    }
    if(NULL != p_exception)
    {
        *p_exception = MODBUS_RTU_EXCEPTION_NONE;
    }
    for(uint16_t i = 0U; i < expected_quantity; i++)
    {
        p_values[i] = get_u16_be(&p_adu[3U + (2U * i)]);
    }
    return MODBUS_RTU_OK;
}

modbus_rtu_status_t modbus_rtu_build_write_single_request(
    uint8_t slave_address,
    uint16_t register_address,
    uint16_t value,
    uint8_t *p_adu,
    uint16_t adu_capacity,
    uint16_t *p_adu_size)
{
    if((0U == slave_address) ||
       (slave_address > MODBUS_RTU_MAX_SLAVE_ADDRESS) ||
       (NULL == p_adu) || (NULL == p_adu_size))
    {
        return MODBUS_RTU_ERR_PARAM;
    }
    if(adu_capacity < MODBUS_RTU_REQUEST_SIZE)
    {
        return MODBUS_RTU_ERR_BUFFER_SIZE;
    }
    p_adu[0] = slave_address;
    p_adu[1] = MODBUS_RTU_FC_WRITE_SINGLE_REGISTER;
    put_u16_be(&p_adu[2], register_address);
    put_u16_be(&p_adu[4], value);
    append_crc(p_adu, 6U);
    *p_adu_size = MODBUS_RTU_REQUEST_SIZE;
    return MODBUS_RTU_OK;
}

modbus_rtu_status_t modbus_rtu_parse_write_single_response(
    uint8_t expected_slave_address,
    uint16_t expected_register_address,
    uint16_t expected_value,
    const uint8_t *p_adu,
    uint16_t adu_size,
    modbus_rtu_exception_t *p_exception)
{
    if(NULL != p_exception)
    {
        *p_exception = MODBUS_RTU_EXCEPTION_NONE;
    }
    if((0U == expected_slave_address) ||
       (expected_slave_address > MODBUS_RTU_MAX_SLAVE_ADDRESS) ||
       (NULL == p_adu))
    {
        return MODBUS_RTU_ERR_PARAM;
    }
    modbus_rtu_status_t status = validate_crc(p_adu, adu_size);
    if(MODBUS_RTU_OK != status)
    {
        return status;
    }
    if((MODBUS_RTU_EXCEPTION_SIZE == adu_size) &&
       (0U != (p_adu[1] & 0x80U)))
    {
        return parse_exception(expected_slave_address,
                               MODBUS_RTU_FC_WRITE_SINGLE_REGISTER,
                               p_adu, adu_size, p_exception);
    }
    if((MODBUS_RTU_REQUEST_SIZE != adu_size) ||
       (expected_slave_address != p_adu[0]) ||
       (MODBUS_RTU_FC_WRITE_SINGLE_REGISTER != p_adu[1]) ||
       (expected_register_address != get_u16_be(&p_adu[2])) ||
       (expected_value != get_u16_be(&p_adu[4])))
    {
        return MODBUS_RTU_ERR_FRAME;
    }
    if(NULL != p_exception)
    {
        *p_exception = MODBUS_RTU_EXCEPTION_NONE;
    }
    return MODBUS_RTU_OK;
}

modbus_rtu_status_t modbus_rtu_slave_process_request(
    const modbus_rtu_slave_t *p_slave,
    const uint8_t *p_request,
    uint16_t request_size,
    uint8_t *p_response,
    uint16_t response_capacity,
    uint16_t *p_response_size)
{
    if((NULL == p_slave) || (0U == p_slave->slave_address) ||
       (p_slave->slave_address > MODBUS_RTU_MAX_SLAVE_ADDRESS) ||
       (NULL == p_request) || (NULL == p_response) ||
       (NULL == p_response_size))
    {
        return MODBUS_RTU_ERR_PARAM;
    }
    *p_response_size = 0U;
    if(MODBUS_RTU_REQUEST_SIZE != request_size)
    {
        return MODBUS_RTU_ERR_FRAME;
    }
    modbus_rtu_status_t status = validate_crc(p_request, request_size);
    if(MODBUS_RTU_OK != status)
    {
        return status;
    }
    if(p_slave->slave_address != p_request[0])
    {
        return MODBUS_RTU_ERR_ADDRESS;
    }

    uint8_t function = p_request[1];
    modbus_rtu_exception_t exception = MODBUS_RTU_EXCEPTION_NONE;
    if(MODBUS_RTU_FC_READ_HOLDING_REGISTERS == function)
    {
        uint16_t start_address = get_u16_be(&p_request[2]);
        uint16_t quantity = get_u16_be(&p_request[4]);
        if((0U == quantity) || (quantity > MODBUS_RTU_MAX_READ_REGISTERS))
        {
            exception = MODBUS_RTU_EXCEPTION_ILLEGAL_DATA_VALUE;
        }
        else if(NULL == p_slave->read_holding)
        {
            exception = MODBUS_RTU_EXCEPTION_SLAVE_DEVICE_FAILURE;
        }
        else
        {
            uint16_t values[MODBUS_RTU_MAX_READ_REGISTERS];
            exception = p_slave->read_holding(p_slave->p_context,
                                              start_address, quantity,
                                              values);
            if(MODBUS_RTU_EXCEPTION_NONE == exception)
            {
                uint16_t response_size = (uint16_t)(
                    MODBUS_RTU_READ_RESPONSE_BASE + (2U * quantity));
                if(response_capacity < response_size)
                {
                    return MODBUS_RTU_ERR_BUFFER_SIZE;
                }
                p_response[0] = p_slave->slave_address;
                p_response[1] = function;
                p_response[2] = (uint8_t)(2U * quantity);
                for(uint16_t i = 0U; i < quantity; i++)
                {
                    put_u16_be(&p_response[3U + (2U * i)], values[i]);
                }
                append_crc(p_response, (uint16_t)(response_size - 2U));
                *p_response_size = response_size;
                return MODBUS_RTU_OK;
            }
        }
    }
    else if(MODBUS_RTU_FC_WRITE_SINGLE_REGISTER == function)
    {
        if(NULL == p_slave->write_single)
        {
            exception = MODBUS_RTU_EXCEPTION_SLAVE_DEVICE_FAILURE;
        }
        else
        {
            exception = p_slave->write_single(p_slave->p_context,
                                              get_u16_be(&p_request[2]),
                                              get_u16_be(&p_request[4]));
            if(MODBUS_RTU_EXCEPTION_NONE == exception)
            {
                if(response_capacity < MODBUS_RTU_REQUEST_SIZE)
                {
                    return MODBUS_RTU_ERR_BUFFER_SIZE;
                }
                for(uint16_t i = 0U; i < MODBUS_RTU_REQUEST_SIZE; i++)
                {
                    p_response[i] = p_request[i];
                }
                *p_response_size = MODBUS_RTU_REQUEST_SIZE;
                return MODBUS_RTU_OK;
            }
        }
    }
    else
    {
        exception = MODBUS_RTU_EXCEPTION_ILLEGAL_FUNCTION;
    }

    status = build_exception_response(p_slave->slave_address, function,
                                      exception, p_response,
                                      response_capacity, p_response_size);
    return (MODBUS_RTU_OK == status) ? MODBUS_RTU_OK : status;
}
