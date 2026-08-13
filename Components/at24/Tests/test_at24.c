/**
 ******************************************************************************
 *@file               :   test_at24.c
 *@brief              :   Host-side protocol tests for the portable AT24 driver.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "at24.h"

/* define   -----------------------------------------------------------------*/
#define TEST_WRITE_LOG_COUNT  (4U)
#define TEST_WRITE_LOG_SIZE   (16U)

/* typedef ------------------------------------------------------------------*/
typedef struct TEST_CONTEXT_T
{
    uint32_t ready_calls;
    uint32_t last_ready_timeout_ms;
    uint32_t write_calls;
    uint32_t write_sizes[TEST_WRITE_LOG_COUNT];
    uint8_t  writes[TEST_WRITE_LOG_COUNT][TEST_WRITE_LOG_SIZE];
    uint8_t  read_prefix[AT24_MAX_WORD_ADDR_BYTES];
    uint16_t read_prefix_size;
    at24_status_t ready_status;
} test_context_t;

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/
static at24_status_t test_write(void *p_context,
                                uint8_t address_7b,
                                const uint8_t *p_data,
                                uint16_t size,
                                uint32_t timeout_ms)
{
    test_context_t *p_ctx = (test_context_t *)p_context;
    assert(0x50U == address_7b);
    assert(0U != timeout_ms);
    assert(p_ctx->write_calls < TEST_WRITE_LOG_COUNT);
    assert(size <= TEST_WRITE_LOG_SIZE);
    p_ctx->write_sizes[p_ctx->write_calls] = size;
    (void)memcpy(p_ctx->writes[p_ctx->write_calls], p_data, size);
    p_ctx->write_calls++;
    return AT24_STATUS_OK;
}

static at24_status_t test_write_read(
    void *p_context,
    uint8_t address_7b,
    const uint8_t *p_tx_data,
    uint16_t tx_size,
    uint8_t *p_rx_data,
    uint16_t rx_size,
    uint32_t timeout_ms)
{
    test_context_t *p_ctx = (test_context_t *)p_context;
    assert(0x50U == address_7b);
    assert(tx_size <= AT24_MAX_WORD_ADDR_BYTES);
    assert(0U != timeout_ms);
    p_ctx->read_prefix_size = tx_size;
    (void)memcpy(p_ctx->read_prefix, p_tx_data, tx_size);
    (void)memset(p_rx_data, 0xA5, rx_size);
    return AT24_STATUS_OK;
}

static at24_status_t test_wait_ready(void *p_context,
                                     uint8_t address_7b,
                                     uint32_t timeout_ms)
{
    test_context_t *p_ctx = (test_context_t *)p_context;
    assert(0x50U == address_7b);
    assert(0U != timeout_ms);
    p_ctx->ready_calls++;
    p_ctx->last_ready_timeout_ms = timeout_ms;
    return p_ctx->ready_status;
}

static const at24_transport_t s_transport = {
    .pf_write       = test_write,
    .pf_write_read  = test_write_read,
    .pf_wait_ready  = test_wait_ready,
};

static void test_cross_page_write(void)
{
    test_context_t ctx = {.ready_status = AT24_STATUS_OK};
    at24_t dev = {0};
    const uint8_t data[5] = {1U, 2U, 3U, 4U, 5U};

    assert(AT24_STATUS_OK ==
           at24_init(&dev, AT24_MODEL_C02, &s_transport, &ctx, 0x50U));
    assert(AT24_STATUS_OK == at24_write(&dev, 6U, data, sizeof(data), 2U));
    assert(2U == ctx.write_calls);
    assert(3U == ctx.write_sizes[0U]);
    assert(6U == ctx.writes[0U][0U]);
    assert(1U == ctx.writes[0U][1U]);
    assert(2U == ctx.writes[0U][2U]);
    assert(4U == ctx.write_sizes[1U]);
    assert(8U == ctx.writes[1U][0U]);
    assert(3U == ctx.writes[1U][1U]);
    assert(5U == ctx.writes[1U][3U]);
    assert(3U == ctx.ready_calls); /* init plus one probe per written page */
    assert(10U == ctx.last_ready_timeout_ms);
}

static void test_read_and_range(void)
{
    test_context_t ctx = {.ready_status = AT24_STATUS_OK};
    at24_t dev = {0};
    uint8_t data[2] = {0U};

    assert(AT24_STATUS_OK ==
           at24_init(&dev, AT24_MODEL_C02, &s_transport, &ctx, 0x50U));
    assert(AT24_STATUS_OK == at24_read(&dev, 0x12U, data, sizeof(data), 2U));
    assert(1U == ctx.read_prefix_size);
    assert(0x12U == ctx.read_prefix[0U]);
    assert(0xA5U == data[0U]);
    assert(AT24_STATUS_RANGE == at24_read(&dev, 255U, data, 2U, 2U));
}

static void test_write_cycle_timeout(void)
{
    test_context_t ctx = {.ready_status = AT24_STATUS_OK};
    at24_t dev = {0};
    uint8_t data = 0x5AU;

    assert(AT24_STATUS_OK ==
           at24_init(&dev, AT24_MODEL_C02, &s_transport, &ctx, 0x50U));
    ctx.ready_status = AT24_STATUS_TIMEOUT;
    assert(AT24_STATUS_TIMEOUT == at24_write(&dev, 0U, &data, 1U, 2U));
    assert(10U == ctx.last_ready_timeout_ms);
}

static void test_invalid_model(void)
{
    test_context_t ctx = {.ready_status = AT24_STATUS_OK};
    at24_t dev = {0};

    assert(AT24_STATUS_PARAM ==
           at24_init(&dev, AT24_MODEL_NUM, &s_transport, &ctx, 0x50U));
}

/* Exported functions -------------------------------------------------------*/
int main(void)
{
    test_cross_page_write();
    test_read_and_range();
    test_write_cycle_timeout();
    test_invalid_model();
    return 0;
}

/* end of file --------------------------------------------------------------*/
