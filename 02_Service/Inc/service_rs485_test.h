/**
 ******************************************************************************
 *@file               :   service_rs485_test.h
 *@brief              :   UART7/UART8 Modbus RTU 0x03/0x06 hardware test.
 ******************************************************************************
 */
#ifndef SERVICE_RS485_TEST_H
#define SERVICE_RS485_TEST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "plat_error.h"

#define SERVICE_RS485_TEST_POLL_PERIOD_MS  (20U)

typedef void (*service_rs485_test_wake_cb_t)(void);

typedef enum
{
    SERVICE_RS485_TEST_EVENT_INIT_OK = 0U,
    SERVICE_RS485_TEST_EVENT_INIT_ERROR,
    SERVICE_RS485_TEST_EVENT_MASTER_WRITE_REQUEST,
    SERVICE_RS485_TEST_EVENT_SLAVE_WRITE_APPLIED,
    SERVICE_RS485_TEST_EVENT_MASTER_WRITE_OK,
    SERVICE_RS485_TEST_EVENT_MASTER_READ_REQUEST,
    SERVICE_RS485_TEST_EVENT_SLAVE_READ_REPLY,
    SERVICE_RS485_TEST_EVENT_MASTER_READ_OK,
    SERVICE_RS485_TEST_EVENT_RESPONSE_TIMEOUT,
    SERVICE_RS485_TEST_EVENT_SEND_ERROR,
    SERVICE_RS485_TEST_EVENT_PROTOCOL_ERROR,
    SERVICE_RS485_TEST_EVENT_VALUE_MISMATCH,
    SERVICE_RS485_TEST_EVENT_FRAME_TIMEOUT,
    SERVICE_RS485_TEST_EVENT_RX_OVERRUN,
} service_rs485_test_event_id_t;

typedef enum
{
    SERVICE_RS485_TEST_ENDPOINT_MASTER = 0U,
    SERVICE_RS485_TEST_ENDPOINT_SLAVE,
} service_rs485_test_endpoint_t;

typedef struct
{
    service_rs485_test_event_id_t event;
    service_rs485_test_endpoint_t endpoint;
    uint8_t function;
    uint16_t register_address;
    uint16_t value;
    uint32_t status;
} service_rs485_test_event_t;

/** @brief Receives test events in task context; may be NULL. */
typedef void (*service_rs485_test_event_cb_t)(
    const service_rs485_test_event_t *p_event);

/** @brief Initialize both RS485 endpoints and the Modbus test state. */
platform_err_t service_rs485_test_init(service_rs485_test_wake_cb_t wake_cb,
                                       service_rs485_test_event_cb_t event_cb);

/** @brief Drain both endpoints and advance the 0x06 then 0x03 test cycle. */
void service_rs485_test_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_RS485_TEST_H */
