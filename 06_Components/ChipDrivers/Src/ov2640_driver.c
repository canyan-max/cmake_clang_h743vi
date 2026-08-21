/**
 ******************************************************************************
 *@file               :   ov2640_driver.c
 *@brief              :   Portable OV2640 image sensor protocol implementation.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "ov2640_driver.h"

/* define   -----------------------------------------------------------------*/
#define OV2640_INITIALIZED              (1U)
#define OV2640_NOT_INITIALIZED          (0U)
#define OV2640_SCCB_ADDRESS_MAX         (0x7FU)
#define OV2640_REG_BANK_SELECT          (0xFFU)
#define OV2640_BANK_DSP                 (0x00U)
#define OV2640_BANK_SENSOR              (0x01U)
#define OV2640_REG_PID_HIGH             (0x0AU)
#define OV2640_REG_PID_LOW              (0x0BU)
#define OV2640_PRODUCT_ID               (0x2642U)
#define OV2640_REG_SENSOR_RESET         (0x12U)
#define OV2640_SENSOR_RESET_VALUE       (0x80U)
#define OV2640_REG_DSP_WIDTH_LOW        (0x5AU)
#define OV2640_REG_DSP_HEIGHT_LOW       (0x5BU)
#define OV2640_REG_DSP_SIZE_HIGH        (0x5CU)
#define OV2640_REG_DSP_RESET            (0xE0U)
#define OV2640_PROFILE_MAX_WIDTH        (800U)
#define OV2640_PROFILE_MAX_HEIGHT       (600U)
#define OV2640_OUTPUT_SIZE_GRANULARITY  (4U)
#define OV2640_BANK_SWITCH_DELAY_MS     (2U)
#define OV2640_SENSOR_RESET_DELAY_MS    (50U)
#define OV2640_OUTPUT_SETTLE_DELAY_MS   (50U)

/* typedef ------------------------------------------------------------------*/
typedef struct OV2640_REGISTER_VALUE_T
{
    uint8_t reg;
    uint8_t value;
} ov2640_register_value_t;

/* variables ----------------------------------------------------------------*/
static const ov2640_register_value_t ov2640_rgb565_svga_config[] = {
    {0xFFU, 0x01U}, {0x12U, 0x80U}, {0xFFU, 0x00U}, {0x2CU, 0xFFU},
    {0x2EU, 0xDFU}, {0xFFU, 0x01U}, {0x3CU, 0x32U}, {0x2CU, 0x0CU},
    {0x33U, 0x78U}, {0x3AU, 0x33U}, {0x3BU, 0xFBU}, {0x3EU, 0x00U},
    {0x43U, 0x11U}, {0x16U, 0x10U}, {0x39U, 0x92U}, {0x35U, 0xDAU},
    {0x22U, 0x1AU}, {0x37U, 0xC3U}, {0x36U, 0x1AU}, {0x4CU, 0x00U},
    {0x5BU, 0x00U}, {0x42U, 0x03U}, {0x4AU, 0x81U}, {0x21U, 0x99U},
    {0x5CU, 0x00U}, {0x63U, 0x00U}, {0x7CU, 0x05U}, {0x6CU, 0x00U},
    {0x6DU, 0x80U}, {0x6EU, 0x00U}, {0x70U, 0x02U}, {0x71U, 0x94U},
    {0x73U, 0xC1U}, {0x20U, 0x80U}, {0x28U, 0x30U}, {0x37U, 0xC0U},
    {0x3DU, 0x38U}, {0x6DU, 0x00U}, {0x23U, 0x00U}, {0x06U, 0x88U},
    {0x07U, 0xC0U}, {0x0DU, 0x87U}, {0x0EU, 0x41U}, {0x11U, 0x80U},
    {0x09U, 0x02U}, {0x04U, 0xA8U}, {0x12U, 0x40U}, {0x14U, 0x48U},
    {0x15U, 0x00U}, {0x32U, 0x09U}, {0x03U, 0x86U}, {0x46U, 0x00U},
    {0x24U, 0x40U}, {0x25U, 0x38U}, {0x26U, 0x82U}, {0x34U, 0xC0U},
    {0x61U, 0x70U}, {0x62U, 0x80U}, {0x17U, 0x11U}, {0x18U, 0x43U},
    {0x19U, 0x00U}, {0x1AU, 0x97U}, {0x13U, 0xE5U}, {0x0CU, 0x3AU},
    {0x4FU, 0xBBU}, {0x50U, 0x9CU}, {0x5AU, 0x23U}, {0xFFU, 0x00U},
    {0xE5U, 0x7FU}, {0x41U, 0x24U}, {0x76U, 0xFFU}, {0x33U, 0xA0U},
    {0x42U, 0x20U}, {0x43U, 0x18U}, {0x4CU, 0x00U}, {0xD7U, 0x03U},
    {0xD9U, 0x10U}, {0x88U, 0x3FU}, {0xC8U, 0x08U}, {0xC9U, 0x80U},
    {0x7CU, 0x00U}, {0x7DU, 0x00U}, {0x7CU, 0x03U}, {0x7DU, 0x48U},
    {0x7DU, 0x48U}, {0x7CU, 0x08U}, {0x7DU, 0x20U}, {0x7DU, 0x10U},
    {0x7DU, 0x0EU}, {0x90U, 0x00U}, {0x91U, 0x0EU}, {0x91U, 0x1AU},
    {0x91U, 0x31U}, {0x91U, 0x5AU}, {0x91U, 0x69U}, {0x91U, 0x75U},
    {0x91U, 0x7EU}, {0x91U, 0x88U}, {0x91U, 0x8FU}, {0x91U, 0x96U},
    {0x91U, 0xA3U}, {0x91U, 0xAFU}, {0x91U, 0xC4U}, {0x91U, 0xD7U},
    {0x91U, 0xE8U}, {0x91U, 0x20U}, {0x92U, 0x00U}, {0x93U, 0x06U},
    {0x93U, 0xE3U}, {0x93U, 0x05U}, {0x93U, 0x05U}, {0x93U, 0x00U},
    {0x93U, 0x04U}, {0x93U, 0x00U}, {0x93U, 0x00U}, {0x93U, 0x00U},
    {0x93U, 0x00U}, {0x93U, 0x00U}, {0x93U, 0x00U}, {0x93U, 0x00U},
    {0x96U, 0x00U}, {0x97U, 0x08U}, {0x97U, 0x19U}, {0x97U, 0x02U},
    {0x97U, 0x0CU}, {0x97U, 0x24U}, {0x97U, 0x30U}, {0x97U, 0x28U},
    {0x97U, 0x26U}, {0x97U, 0x02U}, {0x97U, 0x98U}, {0x97U, 0x80U},
    {0x97U, 0x00U}, {0x97U, 0x00U}, {0xA4U, 0x00U}, {0xA8U, 0x00U},
    {0xC5U, 0x11U}, {0xC6U, 0x51U}, {0xBFU, 0x80U}, {0xC7U, 0x10U},
    {0xB6U, 0x66U}, {0xB8U, 0xA5U}, {0xB7U, 0x64U}, {0xB9U, 0x7CU},
    {0xB3U, 0xAFU}, {0xB4U, 0x97U}, {0xB5U, 0xFFU}, {0xB0U, 0xC5U},
    {0xB1U, 0x94U}, {0xB2U, 0x0FU}, {0xC4U, 0x5CU}, {0x7FU, 0x00U},
    {0xF9U, 0xC0U}, {0xE0U, 0x14U}, {0x87U, 0xD0U}, {0xC3U, 0xEDU},
    {0xC2U, 0x0EU}, {0x86U, 0x3DU}, {0xDAU, 0x08U}, {0xC0U, 0x64U},
    {0xC1U, 0x4BU}, {0x8CU, 0x00U}, {0x50U, 0x00U}, {0x51U, 0xC8U},
    {0x52U, 0x96U}, {0x53U, 0x00U}, {0x54U, 0x00U}, {0x55U, 0x00U},
    {0x57U, 0x00U}, {0xD3U, 0x80U}, {0xE5U, 0x1FU}, {0xE1U, 0x67U},
    {0xDDU, 0x7FU}, {0xE0U, 0x00U}, {0x05U, 0x00U},
};

/* Private  functions  ------------------------------------------------------*/
static uint8_t ov2640_transport_is_valid(const ov2640_transport_t *p_transport)
{
    return (uint8_t)((NULL != p_transport) &&
                     (NULL != p_transport->pf_write_reg) &&
                     (NULL != p_transport->pf_read_reg) &&
                     (NULL != p_transport->pf_delay_ms));
}

static uint8_t ov2640_timeout_is_valid(uint32_t timeout_ms)
{
    return (uint8_t)((0U != timeout_ms) && (UINT32_MAX != timeout_ms));
}

static ov2640_status_t ov2640_write_reg(ov2640_driver_t *p_driver,
                                        uint8_t           reg,
                                        uint8_t           value,
                                        uint32_t          timeout_ms)
{
    return p_driver->p_transport->pf_write_reg(p_driver->address_7b, reg, value,
                                               timeout_ms);
}

static ov2640_status_t ov2640_read_reg(ov2640_driver_t *p_driver,
                                       uint8_t           reg,
                                       uint8_t          *p_value,
                                       uint32_t          timeout_ms)
{
    return p_driver->p_transport->pf_read_reg(p_driver->address_7b, reg, p_value,
                                              timeout_ms);
}

static ov2640_status_t ov2640_select_bank(ov2640_driver_t *p_driver,
                                          uint8_t           bank,
                                          uint32_t          timeout_ms)
{
    return ov2640_write_reg(p_driver, OV2640_REG_BANK_SELECT, bank, timeout_ms);
}

static ov2640_status_t ov2640_read_product_id_raw(ov2640_driver_t *p_driver,
                                                  uint16_t *p_product_id,
                                                  uint32_t  timeout_ms)
{
    ov2640_status_t status = ov2640_select_bank(p_driver, OV2640_BANK_SENSOR,
                                                timeout_ms);
    if(OV2640_STATUS_OK != status)
    {
        return status;
    }

    uint8_t id_high;
    uint8_t id_low;
    status = ov2640_read_reg(p_driver, OV2640_REG_PID_HIGH, &id_high, timeout_ms);
    if(OV2640_STATUS_OK != status)
    {
        return status;
    }
    status = ov2640_read_reg(p_driver, OV2640_REG_PID_LOW, &id_low, timeout_ms);
    if(OV2640_STATUS_OK != status)
    {
        return status;
    }

    *p_product_id = ((uint16_t)id_high << 8U) | id_low;
    return OV2640_STATUS_OK;
}

static ov2640_status_t
ov2640_apply_register_config(ov2640_driver_t                  *p_driver,
                             const ov2640_register_value_t    *p_config,
                             uint32_t                          config_count,
                             uint32_t                          timeout_ms)
{
    for(uint32_t i = 0U; i < config_count; i++)
    {
        ov2640_status_t status = ov2640_write_reg(p_driver, p_config[i].reg,
                                                  p_config[i].value, timeout_ms);
        if(OV2640_STATUS_OK != status)
        {
            return status;
        }

        if(OV2640_REG_BANK_SELECT == p_config[i].reg)
        {
            p_driver->p_transport->pf_delay_ms(OV2640_BANK_SWITCH_DELAY_MS);
        }
        else if((OV2640_REG_SENSOR_RESET == p_config[i].reg) &&
                (OV2640_SENSOR_RESET_VALUE == p_config[i].value))
        {
            p_driver->p_transport->pf_delay_ms(OV2640_SENSOR_RESET_DELAY_MS);
        }
        else
        {
            /* No protocol delay is required for this register. */
        }
    }

    return OV2640_STATUS_OK;
}

static ov2640_status_t ov2640_set_output_size(ov2640_driver_t *p_driver,
                                               uint16_t          width,
                                               uint16_t          height,
                                               uint32_t          timeout_ms)
{
    ov2640_status_t status = ov2640_select_bank(p_driver, OV2640_BANK_DSP,
                                                timeout_ms);
    if(OV2640_STATUS_OK != status)
    {
        return status;
    }

    uint16_t width_div4  = width / OV2640_OUTPUT_SIZE_GRANULARITY;
    uint16_t height_div4 = height / OV2640_OUTPUT_SIZE_GRANULARITY;
    const ov2640_register_value_t output_config[] = {
        {OV2640_REG_DSP_WIDTH_LOW, (uint8_t)(width_div4 & 0xFFU)},
        {OV2640_REG_DSP_HEIGHT_LOW, (uint8_t)(height_div4 & 0xFFU)},
        {OV2640_REG_DSP_SIZE_HIGH,
         (uint8_t)(((width_div4 >> 8U) & 0x03U) |
                   ((height_div4 >> 6U) & 0x04U))},
        {OV2640_REG_DSP_RESET, 0x00U},
    };

    for(uint32_t i = 0U; i < (sizeof(output_config) / sizeof(output_config[0]));
        i++)
    {
        status = ov2640_write_reg(p_driver, output_config[i].reg,
                                  output_config[i].value, timeout_ms);
        if(OV2640_STATUS_OK != status)
        {
            return status;
        }
    }

    p_driver->p_transport->pf_delay_ms(OV2640_OUTPUT_SETTLE_DELAY_MS);
    return OV2640_STATUS_OK;
}

/* Exported functions -------------------------------------------------------*/
ov2640_status_t ov2640_init(ov2640_driver_t          *p_driver,
                            const ov2640_transport_t *p_transport,
                            uint8_t                    address_7b,
                            uint32_t                   timeout_ms)
{
    if(NULL == p_driver)
    {
        return OV2640_STATUS_PARAM;
    }

    p_driver->initialized = OV2640_NOT_INITIALIZED;
    if((0U == ov2640_transport_is_valid(p_transport)) ||
       (address_7b > OV2640_SCCB_ADDRESS_MAX) ||
       (0U == ov2640_timeout_is_valid(timeout_ms)))
    {
        return OV2640_STATUS_PARAM;
    }

    p_driver->p_transport = p_transport;
    p_driver->address_7b  = address_7b;

    uint16_t product_id;
    ov2640_status_t status = ov2640_read_product_id_raw(p_driver, &product_id,
                                                        timeout_ms);
    if(OV2640_STATUS_OK != status)
    {
        return status;
    }
    if(OV2640_PRODUCT_ID != product_id)
    {
        return OV2640_STATUS_ID_MISMATCH;
    }

    p_driver->initialized = OV2640_INITIALIZED;
    return OV2640_STATUS_OK;
}

ov2640_status_t ov2640_configure(ov2640_driver_t *p_driver,
                                 ov2640_profile_t profile,
                                 uint16_t          output_width,
                                 uint16_t          output_height,
                                 uint32_t          timeout_ms)
{
    if((NULL == p_driver) || (profile >= OV2640_PROFILE_NUM) ||
       (0U == output_width) || (0U == output_height) ||
       (output_width > OV2640_PROFILE_MAX_WIDTH) ||
       (output_height > OV2640_PROFILE_MAX_HEIGHT) ||
       (0U != (output_width % OV2640_OUTPUT_SIZE_GRANULARITY)) ||
       (0U != (output_height % OV2640_OUTPUT_SIZE_GRANULARITY)) ||
       (0U == ov2640_timeout_is_valid(timeout_ms)))
    {
        return OV2640_STATUS_PARAM;
    }
    if(OV2640_INITIALIZED != p_driver->initialized)
    {
        return OV2640_STATUS_NOT_INITIALIZED;
    }

    switch(profile)
    {
        case OV2640_PROFILE_RGB565_SVGA:
        {
            ov2640_status_t status = ov2640_apply_register_config(
                p_driver, ov2640_rgb565_svga_config,
                (uint32_t)(sizeof(ov2640_rgb565_svga_config) /
                           sizeof(ov2640_rgb565_svga_config[0])),
                timeout_ms);
            if(OV2640_STATUS_OK != status)
            {
                return status;
            }
            break;
        }

        case OV2640_PROFILE_NUM:
        default:
            return OV2640_STATUS_PARAM;
    }

    return ov2640_set_output_size(p_driver, output_width, output_height,
                                  timeout_ms);
}

ov2640_status_t ov2640_read_product_id(ov2640_driver_t *p_driver,
                                       uint16_t         *p_product_id,
                                       uint32_t          timeout_ms)
{
    if((NULL == p_driver) || (NULL == p_product_id) ||
       (0U == ov2640_timeout_is_valid(timeout_ms)))
    {
        return OV2640_STATUS_PARAM;
    }
    if(OV2640_INITIALIZED != p_driver->initialized)
    {
        return OV2640_STATUS_NOT_INITIALIZED;
    }

    return ov2640_read_product_id_raw(p_driver, p_product_id, timeout_ms);
}

/* end of file --------------------------------------------------------------*/
