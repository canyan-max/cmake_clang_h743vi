/**
 ******************************************************************************
 *@file               :   modbus_rtu.h
 *@brief              :   Transport-independent Modbus RTU ADU helpers for
 *                        function 0x03 and function 0x06.
 ******************************************************************************
 */
#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define MODBUS_RTU_FC_READ_HOLDING_REGISTERS  (0x03U)
#define MODBUS_RTU_FC_WRITE_SINGLE_REGISTER   (0x06U)
#define MODBUS_RTU_MAX_READ_REGISTERS         (125U)
#define MODBUS_RTU_MAX_ADU_SIZE               (256U)
#define MODBUS_RTU_MAX_SLAVE_ADDRESS           (247U)

typedef enum
{
    MODBUS_RTU_OK = 0U,
    MODBUS_RTU_ERR_PARAM,
    MODBUS_RTU_ERR_BUFFER_SIZE,
    MODBUS_RTU_ERR_FRAME,
    MODBUS_RTU_ERR_CRC,
    MODBUS_RTU_ERR_ADDRESS,
    MODBUS_RTU_ERR_FUNCTION,
    MODBUS_RTU_ERR_EXCEPTION,
} modbus_rtu_status_t;

typedef enum
{
    MODBUS_RTU_EXCEPTION_NONE = 0U,
    MODBUS_RTU_EXCEPTION_ILLEGAL_FUNCTION = 0x01U,
    MODBUS_RTU_EXCEPTION_ILLEGAL_DATA_ADDRESS = 0x02U,
    MODBUS_RTU_EXCEPTION_ILLEGAL_DATA_VALUE = 0x03U,
    MODBUS_RTU_EXCEPTION_SLAVE_DEVICE_FAILURE = 0x04U,
} modbus_rtu_exception_t;

typedef modbus_rtu_exception_t (*modbus_rtu_read_holding_cb_t)(
    void *p_context,
    uint16_t start_address,
    uint16_t quantity,
    uint16_t *p_values);

typedef modbus_rtu_exception_t (*modbus_rtu_write_single_cb_t)(
    void *p_context,
    uint16_t register_address,
    uint16_t value);

typedef struct
{
    uint8_t slave_address;
    modbus_rtu_read_holding_cb_t read_holding;
    modbus_rtu_write_single_cb_t write_single;
    void *p_context;
} modbus_rtu_slave_t;

/** @brief Calculate the standard Modbus RTU CRC16. */
uint16_t modbus_rtu_crc16(const uint8_t *p_data, uint16_t size);

/** @brief Build a function 0x03 request ADU. */
modbus_rtu_status_t modbus_rtu_build_read_holding_request(
    uint8_t slave_address,
    uint16_t start_address,
    uint16_t quantity,
    uint8_t *p_adu,
    uint16_t adu_capacity,
    uint16_t *p_adu_size);

/** @brief Parse a function 0x03 response and copy its register values. */
modbus_rtu_status_t modbus_rtu_parse_read_holding_response(
    uint8_t expected_slave_address,
    uint16_t expected_quantity,
    const uint8_t *p_adu,
    uint16_t adu_size,
    uint16_t *p_values,
    uint16_t value_capacity,
    modbus_rtu_exception_t *p_exception);

/** @brief Build a function 0x06 request ADU. */
modbus_rtu_status_t modbus_rtu_build_write_single_request(
    uint8_t slave_address,
    uint16_t register_address,
    uint16_t value,
    uint8_t *p_adu,
    uint16_t adu_capacity,
    uint16_t *p_adu_size);

/** @brief Validate a function 0x06 echo response. */
modbus_rtu_status_t modbus_rtu_parse_write_single_response(
    uint8_t expected_slave_address,
    uint16_t expected_register_address,
    uint16_t expected_value,
    const uint8_t *p_adu,
    uint16_t adu_size,
    modbus_rtu_exception_t *p_exception);

/**
 * @brief Process one complete request ADU and build its slave response.
 * @note A request for another slave returns MODBUS_RTU_ERR_ADDRESS with a
 *       zero response size. Broadcast requests are not handled by this
 *       initial 0x03/0x06 implementation.
 */
modbus_rtu_status_t modbus_rtu_slave_process_request(
    const modbus_rtu_slave_t *p_slave,
    const uint8_t *p_request,
    uint16_t request_size,
    uint8_t *p_response,
    uint16_t response_capacity,
    uint16_t *p_response_size);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_H */
