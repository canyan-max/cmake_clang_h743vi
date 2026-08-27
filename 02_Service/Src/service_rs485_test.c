/**
 ******************************************************************************
 *@file               :   service_rs485_test.c
 *@brief              :   Modbus RTU hardware test using UART7 as master and
 *                        UART8 as slave. Function 0x06 writes one holding
 *                        register, then function 0x03 reads it back.
 ******************************************************************************
 */

#include <stddef.h>
#include <stdint.h>
#include "service_rs485_test.h"
#include "bsp_rs485.h"
#include "modbus_rtu.h"
#include "plat_log.h"
#include "plat_sys.h"

#define MODBUS_TEST_SLAVE_ADDRESS       (1U)
#define MODBUS_TEST_REGISTER_COUNT      (32U)
#define MODBUS_TEST_REGISTER_ADDRESS    (0x0010U)
#define MODBUS_TEST_SEND_TIMEOUT_MS     (100U)
#define MODBUS_TEST_RESPONSE_TIMEOUT_MS (200U)
#define MODBUS_TEST_CYCLE_PERIOD_MS     (500U)
#define MODBUS_TEST_FRAME_TIMEOUT_MS    (100U)
#define MODBUS_TEST_RX_CHUNK_SIZE       (64U)
#define MODBUS_TEST_REQUEST_SIZE        (8U)
#define MODBUS_TEST_EXCEPTION_SIZE      (5U)
#define MODBUS_TEST_MASTER_PORT         BSP_RS485_PORT_1
#define MODBUS_TEST_SLAVE_PORT          BSP_RS485_PORT_2

typedef enum
{
    MODBUS_TEST_PHASE_IDLE = 0U,
    MODBUS_TEST_PHASE_WAIT_WRITE_RESPONSE,
    MODBUS_TEST_PHASE_WAIT_READ_RESPONSE,
} modbus_test_phase_t;

typedef struct
{
    uint8_t data[MODBUS_RTU_MAX_ADU_SIZE];
    uint16_t index;
    uint16_t expected_size;
    uint32_t last_byte_tick;
} modbus_test_parser_t;

typedef enum
{
    MODBUS_TEST_PARSE_NONE = 0U,
    MODBUS_TEST_PARSE_FRAME,
    MODBUS_TEST_PARSE_ERROR,
} modbus_test_parse_result_t;

static uint16_t s_holding_registers[MODBUS_TEST_REGISTER_COUNT];
static modbus_test_parser_t s_master_parser;
static modbus_test_parser_t s_slave_parser;
static service_rs485_test_wake_cb_t s_wake_cb;
static service_rs485_test_event_cb_t s_event_cb;
static modbus_test_phase_t s_phase;
static uint16_t s_test_value;
static uint32_t s_last_transaction_tick;
static uint32_t s_reported_dropped_bytes[BSP_RS485_NUM];

static void modbus_test_emit(service_rs485_test_event_id_t event,
                             service_rs485_test_endpoint_t endpoint,
                             uint8_t function,
                             uint16_t register_address,
                             uint16_t value,
                             uint32_t status)
{
    if(NULL != s_event_cb)
    {
        const service_rs485_test_event_t event_data = {
            .event = event,
            .endpoint = endpoint,
            .function = function,
            .register_address = register_address,
            .value = value,
            .status = status,
        };
        s_event_cb(&event_data);
    }
}

static modbus_rtu_exception_t modbus_test_read_holding(
    void *p_context,
    uint16_t start_address,
    uint16_t quantity,
    uint16_t *p_values)
{
    ((void)p_context);
    if((NULL == p_values) || (0U == quantity) ||
       (start_address >= MODBUS_TEST_REGISTER_COUNT) ||
       (quantity > (uint16_t)(MODBUS_TEST_REGISTER_COUNT - start_address)))
    {
        return MODBUS_RTU_EXCEPTION_ILLEGAL_DATA_ADDRESS;
    }
    for(uint16_t i = 0U; i < quantity; i++)
    {
        p_values[i] = s_holding_registers[start_address + i];
    }
    return MODBUS_RTU_EXCEPTION_NONE;
}

static modbus_rtu_exception_t modbus_test_write_single(
    void *p_context,
    uint16_t register_address,
    uint16_t value)
{
    ((void)p_context);
    if(register_address >= MODBUS_TEST_REGISTER_COUNT)
    {
        return MODBUS_RTU_EXCEPTION_ILLEGAL_DATA_ADDRESS;
    }
    s_holding_registers[register_address] = value;
    return MODBUS_RTU_EXCEPTION_NONE;
}

static void modbus_test_parser_reset(modbus_test_parser_t *p_parser)
{
    if(NULL != p_parser)
    {
        p_parser->index = 0U;
        p_parser->expected_size = 0U;
        p_parser->last_byte_tick = 0U;
    }
}

static modbus_test_parse_result_t modbus_test_parser_feed(
    modbus_test_parser_t *p_parser,
    bsp_rs485_id_t endpoint,
    uint8_t byte)
{
    if(NULL == p_parser)
    {
        return MODBUS_TEST_PARSE_ERROR;
    }
    if(p_parser->index >= MODBUS_RTU_MAX_ADU_SIZE)
    {
        modbus_test_parser_reset(p_parser);
        return MODBUS_TEST_PARSE_ERROR;
    }
    p_parser->data[p_parser->index++] = byte;
    if(2U == p_parser->index)
    {
        uint8_t function = p_parser->data[1];
        if(0U != (function & 0x80U))
        {
            p_parser->expected_size = MODBUS_TEST_EXCEPTION_SIZE;
        }
        else if(MODBUS_TEST_SLAVE_PORT == endpoint)
        {
            if((MODBUS_RTU_FC_READ_HOLDING_REGISTERS == function) ||
               (MODBUS_RTU_FC_WRITE_SINGLE_REGISTER == function))
            {
                p_parser->expected_size = MODBUS_TEST_REQUEST_SIZE;
            }
            else
            {
                modbus_test_parser_reset(p_parser);
                return MODBUS_TEST_PARSE_ERROR;
            }
        }
        else if(MODBUS_RTU_FC_WRITE_SINGLE_REGISTER == function)
        {
            p_parser->expected_size = MODBUS_TEST_REQUEST_SIZE;
        }
        else if(MODBUS_RTU_FC_READ_HOLDING_REGISTERS != function)
        {
            modbus_test_parser_reset(p_parser);
            return MODBUS_TEST_PARSE_ERROR;
        }
        else
        {
            /* Function 0x03 response size follows in its byte-count field. */
        }
    }
    if((MODBUS_TEST_MASTER_PORT == endpoint) && (3U == p_parser->index) &&
       (MODBUS_RTU_FC_READ_HOLDING_REGISTERS == p_parser->data[1]))
    {
        uint16_t expected = (uint16_t)(5U + p_parser->data[2]);
        if((0U != (p_parser->data[2] & 1U)) ||
           (expected > MODBUS_RTU_MAX_ADU_SIZE))
        {
            modbus_test_parser_reset(p_parser);
            return MODBUS_TEST_PARSE_ERROR;
        }
        p_parser->expected_size = expected;
    }
    return ((0U != p_parser->expected_size) &&
            (p_parser->index == p_parser->expected_size))
               ? MODBUS_TEST_PARSE_FRAME
               : MODBUS_TEST_PARSE_NONE;
}

static void modbus_test_rx_notify(bsp_rs485_id_t id)
{
    if((id < BSP_RS485_NUM) && (NULL != s_wake_cb))
    {
        s_wake_cb();
    }
}

static platform_err_t modbus_test_send_read_request(uint32_t now)
{
    uint8_t request[MODBUS_TEST_REQUEST_SIZE];
    uint16_t request_size = 0U;
    modbus_rtu_status_t status = modbus_rtu_build_read_holding_request(
        MODBUS_TEST_SLAVE_ADDRESS, MODBUS_TEST_REGISTER_ADDRESS, 1U,
        request, sizeof(request), &request_size);
    if((MODBUS_RTU_OK != status) ||
       (PLATFORM_ERR_OK != bsp_rs485_send(MODBUS_TEST_MASTER_PORT, request,
                                          request_size,
                                          MODBUS_TEST_SEND_TIMEOUT_MS)))
    {
        modbus_test_emit(SERVICE_RS485_TEST_EVENT_SEND_ERROR,
                         SERVICE_RS485_TEST_ENDPOINT_MASTER,
                         MODBUS_RTU_FC_READ_HOLDING_REGISTERS,
                         MODBUS_TEST_REGISTER_ADDRESS, s_test_value, status);
        return PLATFORM_ERR_HW;
    }
    s_phase = MODBUS_TEST_PHASE_WAIT_READ_RESPONSE;
    s_last_transaction_tick = now;
    modbus_test_emit(SERVICE_RS485_TEST_EVENT_MASTER_READ_REQUEST,
                     SERVICE_RS485_TEST_ENDPOINT_MASTER,
                     MODBUS_RTU_FC_READ_HOLDING_REGISTERS,
                     MODBUS_TEST_REGISTER_ADDRESS, s_test_value,
                     MODBUS_RTU_OK);
    return PLATFORM_ERR_OK;
}

static void modbus_test_handle_slave_frame(const uint8_t *p_frame,
                                           uint16_t frame_size)
{
    modbus_rtu_slave_t slave = {
        .slave_address = MODBUS_TEST_SLAVE_ADDRESS,
        .read_holding = modbus_test_read_holding,
        .write_single = modbus_test_write_single,
        .p_context = NULL,
    };
    uint8_t response[MODBUS_RTU_MAX_ADU_SIZE];
    uint16_t response_size = 0U;
    modbus_rtu_status_t status = modbus_rtu_slave_process_request(
        &slave, p_frame, frame_size, response, sizeof(response),
        &response_size);
    if(MODBUS_RTU_OK != status)
    {
        modbus_test_emit(SERVICE_RS485_TEST_EVENT_PROTOCOL_ERROR,
                         SERVICE_RS485_TEST_ENDPOINT_SLAVE, p_frame[1],
                         0U, 0U, status);
        return;
    }
    if((0U == response_size) ||
       (PLATFORM_ERR_OK != bsp_rs485_send(MODBUS_TEST_SLAVE_PORT, response,
                                          response_size,
                                          MODBUS_TEST_SEND_TIMEOUT_MS)))
    {
        modbus_test_emit(SERVICE_RS485_TEST_EVENT_SEND_ERROR,
                         SERVICE_RS485_TEST_ENDPOINT_SLAVE, p_frame[1],
                         0U, 0U, PLATFORM_ERR_HW);
        return;
    }
    uint16_t address = (uint16_t)((uint16_t)p_frame[2] << 8U) | p_frame[3];
    uint16_t value = 0U;
    service_rs485_test_event_id_t event =
        SERVICE_RS485_TEST_EVENT_SLAVE_READ_REPLY;
    if(MODBUS_RTU_FC_WRITE_SINGLE_REGISTER == p_frame[1])
    {
        value = (uint16_t)((uint16_t)p_frame[4] << 8U) | p_frame[5];
        event = SERVICE_RS485_TEST_EVENT_SLAVE_WRITE_APPLIED;
    }
    else if(address < MODBUS_TEST_REGISTER_COUNT)
    {
        value = s_holding_registers[address];
    }
    else
    {
        /* The component has already encoded the applicable exception. */
    }
    modbus_test_emit(event, SERVICE_RS485_TEST_ENDPOINT_SLAVE, p_frame[1],
                     address, value, MODBUS_RTU_OK);
}

static void modbus_test_handle_master_frame(const uint8_t *p_frame,
                                            uint16_t frame_size,
                                            uint32_t now)
{
    modbus_rtu_exception_t exception = MODBUS_RTU_EXCEPTION_NONE;
    modbus_rtu_status_t status;
    if(MODBUS_TEST_PHASE_WAIT_WRITE_RESPONSE == s_phase)
    {
        status = modbus_rtu_parse_write_single_response(
            MODBUS_TEST_SLAVE_ADDRESS, MODBUS_TEST_REGISTER_ADDRESS,
            s_test_value, p_frame, frame_size, &exception);
        if(MODBUS_RTU_OK == status)
        {
            modbus_test_emit(SERVICE_RS485_TEST_EVENT_MASTER_WRITE_OK,
                             SERVICE_RS485_TEST_ENDPOINT_MASTER,
                             MODBUS_RTU_FC_WRITE_SINGLE_REGISTER,
                             MODBUS_TEST_REGISTER_ADDRESS, s_test_value,
                             MODBUS_RTU_OK);
            if(PLATFORM_ERR_OK != modbus_test_send_read_request(now))
            {
                s_phase = MODBUS_TEST_PHASE_IDLE;
                s_last_transaction_tick = now;
            }
            return;
        }
    }
    else if(MODBUS_TEST_PHASE_WAIT_READ_RESPONSE == s_phase)
    {
        uint16_t value = 0U;
        status = modbus_rtu_parse_read_holding_response(
            MODBUS_TEST_SLAVE_ADDRESS, 1U, p_frame, frame_size, &value, 1U,
            &exception);
        if(MODBUS_RTU_OK == status)
        {
            service_rs485_test_event_id_t event =
                (value == s_test_value)
                    ? SERVICE_RS485_TEST_EVENT_MASTER_READ_OK
                    : SERVICE_RS485_TEST_EVENT_VALUE_MISMATCH;
            modbus_test_emit(event, SERVICE_RS485_TEST_ENDPOINT_MASTER,
                             MODBUS_RTU_FC_READ_HOLDING_REGISTERS,
                             MODBUS_TEST_REGISTER_ADDRESS, value,
                             (value == s_test_value) ? MODBUS_RTU_OK
                                                     : s_test_value);
            s_phase = MODBUS_TEST_PHASE_IDLE;
            s_last_transaction_tick = now;
            return;
        }
    }
    else
    {
        status = MODBUS_RTU_ERR_FRAME;
    }
    modbus_test_emit(SERVICE_RS485_TEST_EVENT_PROTOCOL_ERROR,
                     SERVICE_RS485_TEST_ENDPOINT_MASTER,
                     (frame_size > 1U) ? p_frame[1] : 0U,
                     MODBUS_TEST_REGISTER_ADDRESS, (uint16_t)exception,
                     status);
}

static void modbus_test_drain(bsp_rs485_id_t id,
                              modbus_test_parser_t *p_parser,
                              uint32_t now)
{
    uint8_t buffer[MODBUS_TEST_RX_CHUNK_SIZE];
    uint16_t count;
    while((count = bsp_rs485_read(id, buffer, sizeof(buffer))) > 0U)
    {
        for(uint16_t i = 0U; i < count; i++)
        {
            modbus_test_parse_result_t result = modbus_test_parser_feed(
                p_parser, id, buffer[i]);
            p_parser->last_byte_tick = now;
            if(MODBUS_TEST_PARSE_FRAME == result)
            {
                if(MODBUS_TEST_SLAVE_PORT == id)
                {
                    modbus_test_handle_slave_frame(p_parser->data,
                                                   p_parser->index);
                }
                else
                {
                    modbus_test_handle_master_frame(p_parser->data,
                                                    p_parser->index, now);
                }
                modbus_test_parser_reset(p_parser);
            }
            else if(MODBUS_TEST_PARSE_ERROR == result)
            {
                modbus_test_emit(SERVICE_RS485_TEST_EVENT_PROTOCOL_ERROR,
                                 (MODBUS_TEST_MASTER_PORT == id)
                                     ? SERVICE_RS485_TEST_ENDPOINT_MASTER
                                     : SERVICE_RS485_TEST_ENDPOINT_SLAVE,
                                 0U, 0U, 0U, MODBUS_RTU_ERR_FRAME);
            }
            else
            {
                /* Incomplete ADU; wait for more DMA bytes. */
            }
        }
    }
}

static void modbus_test_expire_parser(
    modbus_test_parser_t *p_parser,
    service_rs485_test_endpoint_t endpoint,
    uint32_t now)
{
    if((NULL != p_parser) && (p_parser->index > 0U) &&
       ((uint32_t)(now - p_parser->last_byte_tick) >=
        MODBUS_TEST_FRAME_TIMEOUT_MS))
    {
        modbus_test_parser_reset(p_parser);
        modbus_test_emit(SERVICE_RS485_TEST_EVENT_FRAME_TIMEOUT, endpoint,
                         0U, 0U, 0U, MODBUS_RTU_ERR_FRAME);
    }
}

static void modbus_test_report_rx_overrun(
    bsp_rs485_id_t id,
    service_rs485_test_endpoint_t endpoint)
{
    uint32_t total = bsp_rs485_get_rx_dropped_bytes(id);
    if(total != s_reported_dropped_bytes[id])
    {
        uint32_t dropped = total - s_reported_dropped_bytes[id];
        s_reported_dropped_bytes[id] = total;
        modbus_test_emit(SERVICE_RS485_TEST_EVENT_RX_OVERRUN, endpoint, 0U,
                         0U, 0U, dropped);
    }
}

static void modbus_test_send_write_request(uint32_t now)
{
    uint8_t request[MODBUS_TEST_REQUEST_SIZE];
    uint16_t request_size = 0U;
    s_test_value++;
    modbus_rtu_status_t status = modbus_rtu_build_write_single_request(
        MODBUS_TEST_SLAVE_ADDRESS, MODBUS_TEST_REGISTER_ADDRESS, s_test_value,
        request, sizeof(request), &request_size);
    if((MODBUS_RTU_OK != status) ||
       (PLATFORM_ERR_OK != bsp_rs485_send(MODBUS_TEST_MASTER_PORT, request,
                                          request_size,
                                          MODBUS_TEST_SEND_TIMEOUT_MS)))
    {
        modbus_test_emit(SERVICE_RS485_TEST_EVENT_SEND_ERROR,
                         SERVICE_RS485_TEST_ENDPOINT_MASTER,
                         MODBUS_RTU_FC_WRITE_SINGLE_REGISTER,
                         MODBUS_TEST_REGISTER_ADDRESS, s_test_value, status);
        s_last_transaction_tick = now;
        return;
    }
    s_phase = MODBUS_TEST_PHASE_WAIT_WRITE_RESPONSE;
    s_last_transaction_tick = now;
    modbus_test_emit(SERVICE_RS485_TEST_EVENT_MASTER_WRITE_REQUEST,
                     SERVICE_RS485_TEST_ENDPOINT_MASTER,
                     MODBUS_RTU_FC_WRITE_SINGLE_REGISTER,
                     MODBUS_TEST_REGISTER_ADDRESS, s_test_value,
                     MODBUS_RTU_OK);
}

platform_err_t service_rs485_test_init(service_rs485_test_wake_cb_t wake_cb,
                                       service_rs485_test_event_cb_t event_cb)
{
    s_wake_cb = wake_cb;
    s_event_cb = event_cb;
    s_phase = MODBUS_TEST_PHASE_IDLE;
    s_test_value = 0U;
    for(uint16_t i = 0U; i < MODBUS_TEST_REGISTER_COUNT; i++)
    {
        s_holding_registers[i] = 0U;
    }
    for(uint8_t i = 0U; i < (uint8_t)BSP_RS485_NUM; i++)
    {
        s_reported_dropped_bytes[i] = 0U;
    }
    modbus_test_parser_reset(&s_master_parser);
    modbus_test_parser_reset(&s_slave_parser);

    platform_err_t status = bsp_rs485_init(MODBUS_TEST_MASTER_PORT);
    if(PLATFORM_ERR_OK == status)
    {
        status = bsp_rs485_set_rx_notify_cb(MODBUS_TEST_MASTER_PORT,
                                             modbus_test_rx_notify);
    }
    if(PLATFORM_ERR_OK == status)
    {
        status = bsp_rs485_init(MODBUS_TEST_SLAVE_PORT);
    }
    if(PLATFORM_ERR_OK == status)
    {
        status = bsp_rs485_set_rx_notify_cb(MODBUS_TEST_SLAVE_PORT,
                                             modbus_test_rx_notify);
    }
    if(PLATFORM_ERR_OK != status)
    {
        modbus_test_emit(SERVICE_RS485_TEST_EVENT_INIT_ERROR,
                         SERVICE_RS485_TEST_ENDPOINT_MASTER, 0U, 0U, 0U,
                         status);
        return status;
    }
    s_last_transaction_tick = plat_tick_get_ms() -
                              MODBUS_TEST_CYCLE_PERIOD_MS;
    modbus_test_emit(SERVICE_RS485_TEST_EVENT_INIT_OK,
                     SERVICE_RS485_TEST_ENDPOINT_MASTER, 0U, 0U, 0U,
                     MODBUS_RTU_OK);
    plat_log_i("modbus", "RTU 0x03/0x06 test init");
    return PLATFORM_ERR_OK;
}

void service_rs485_test_poll(void)
{
    uint32_t now = plat_tick_get_ms();
    modbus_test_drain(MODBUS_TEST_SLAVE_PORT, &s_slave_parser, now);
    modbus_test_drain(MODBUS_TEST_MASTER_PORT, &s_master_parser, now);
    modbus_test_report_rx_overrun(MODBUS_TEST_SLAVE_PORT,
                                  SERVICE_RS485_TEST_ENDPOINT_SLAVE);
    modbus_test_report_rx_overrun(MODBUS_TEST_MASTER_PORT,
                                  SERVICE_RS485_TEST_ENDPOINT_MASTER);
    modbus_test_expire_parser(&s_slave_parser,
                              SERVICE_RS485_TEST_ENDPOINT_SLAVE, now);
    modbus_test_expire_parser(&s_master_parser,
                              SERVICE_RS485_TEST_ENDPOINT_MASTER, now);

    if((MODBUS_TEST_PHASE_IDLE != s_phase) &&
       ((uint32_t)(now - s_last_transaction_tick) >=
        MODBUS_TEST_RESPONSE_TIMEOUT_MS))
    {
        uint8_t function =
            (MODBUS_TEST_PHASE_WAIT_WRITE_RESPONSE == s_phase)
                ? MODBUS_RTU_FC_WRITE_SINGLE_REGISTER
                : MODBUS_RTU_FC_READ_HOLDING_REGISTERS;
        modbus_test_emit(SERVICE_RS485_TEST_EVENT_RESPONSE_TIMEOUT,
                         SERVICE_RS485_TEST_ENDPOINT_MASTER, function,
                         MODBUS_TEST_REGISTER_ADDRESS, s_test_value,
                         MODBUS_RTU_ERR_FRAME);
        s_phase = MODBUS_TEST_PHASE_IDLE;
        s_last_transaction_tick = now;
    }
    if((MODBUS_TEST_PHASE_IDLE == s_phase) &&
       ((uint32_t)(now - s_last_transaction_tick) >=
        MODBUS_TEST_CYCLE_PERIOD_MS))
    {
        modbus_test_send_write_request(now);
    }
}
