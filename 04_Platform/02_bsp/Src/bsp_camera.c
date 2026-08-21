/**
 ******************************************************************************
 *@file               :   bsp_camera.c
 *@brief              :   Assemble the fitted OV2640 sensor and STM32 DCMI.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "bsp_camera.h"
#include "ov2640_driver.h"
#include "plat_dcmi.h"
#include "plat_gpio.h"
#include "plat_i2c.h"
#include "plat_sys.h"
#include "board_config.h"

/* define   -----------------------------------------------------------------*/
#define BSP_CAMERA_SENSOR_ADDRESS_7B        (0x30U)
#define BSP_CAMERA_SENSOR_TIMEOUT_MS        (100U)
#define BSP_CAMERA_POWER_SETTLE_MS          (10U)
#define BSP_CAMERA_SENSOR_OUTPUT_WIDTH      (400U)
#define BSP_CAMERA_SENSOR_OUTPUT_HEIGHT     (296U)
#define BSP_CAMERA_CROP_X_OFFSET_BYTES      \
    (BSP_CAMERA_SENSOR_OUTPUT_WIDTH - BSP_CAMERA_FRAME_WIDTH)
#define BSP_CAMERA_CROP_Y_OFFSET_LINES      \
    (((BSP_CAMERA_SENSOR_OUTPUT_HEIGHT - BSP_CAMERA_FRAME_HEIGHT) / 2U) - 1U)
#define BSP_CAMERA_INITIALIZED              (1U)
#define BSP_CAMERA_NOT_INITIALIZED          (0U)

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static ov2640_driver_t s_camera_sensor;
static uint8_t          s_camera_initialized;

/* Private  functions  ------------------------------------------------------*/
static ov2640_status_t
bsp_camera_convert_platform_error(platform_err_t error)
{
    switch(error)
    {
        case PLATFORM_ERR_OK:
            return OV2640_STATUS_OK;

        case PLATFORM_ERR_BUSY:
            return OV2640_STATUS_BUSY;

        case PLATFORM_ERR_TIMEOUT:
            return OV2640_STATUS_TIMEOUT;

        case PLATFORM_ERR_PARAM:
        case PLATFORM_ERR_HW:
        default:
            return OV2640_STATUS_TRANSPORT;
    }
}

static platform_err_t bsp_camera_convert_sensor_status(ov2640_status_t status)
{
    switch(status)
    {
        case OV2640_STATUS_OK:
            return PLATFORM_ERR_OK;

        case OV2640_STATUS_PARAM:
            return PLATFORM_ERR_PARAM;

        case OV2640_STATUS_BUSY:
            return PLATFORM_ERR_BUSY;

        case OV2640_STATUS_TIMEOUT:
            return PLATFORM_ERR_TIMEOUT;

        case OV2640_STATUS_NOT_INITIALIZED:
        case OV2640_STATUS_ID_MISMATCH:
        case OV2640_STATUS_TRANSPORT:
        default:
            return PLATFORM_ERR_HW;
    }
}

static ov2640_status_t bsp_camera_sensor_write_reg(uint8_t  address_7b,
                                                   uint8_t  reg,
                                                   uint8_t  value,
                                                   uint32_t timeout_ms)
{
    uint8_t data[2] = {reg, value};
    return bsp_camera_convert_platform_error(
        plat_i2c_write(PLAT_I2C_ID_0, address_7b, data, sizeof(data),
                       timeout_ms));
}

static ov2640_status_t bsp_camera_sensor_read_reg(uint8_t  address_7b,
                                                  uint8_t  reg,
                                                  uint8_t *p_value,
                                                  uint32_t timeout_ms)
{
    if(NULL == p_value)
    {
        return OV2640_STATUS_PARAM;
    }

    platform_err_t error = plat_i2c_write(PLAT_I2C_ID_0, address_7b, &reg, 1U,
                                          timeout_ms);
    if(PLATFORM_ERR_OK != error)
    {
        return bsp_camera_convert_platform_error(error);
    }

    return bsp_camera_convert_platform_error(
        plat_i2c_read(PLAT_I2C_ID_0, address_7b, p_value, 1U, timeout_ms));
}

static void bsp_camera_sensor_delay_ms(uint32_t delay_ms)
{
    plat_delay_ms(delay_ms);
}

static const ov2640_transport_t s_camera_sensor_transport = {
    .pf_write_reg = bsp_camera_sensor_write_reg,
    .pf_read_reg  = bsp_camera_sensor_read_reg,
    .pf_delay_ms  = bsp_camera_sensor_delay_ms,
};

static platform_err_t bsp_camera_power_on(void)
{
    uint8_t power_on_level = (0U == BOARD_CAMERA_PWDN_ACTIVE_LEVEL) ? 1U : 0U;
    platform_err_t error = plat_gpio_write(PLAT_GPIO_ID_CAMERA_PWDN,
                                           power_on_level);
    if(PLATFORM_ERR_OK == error)
    {
        plat_delay_ms(BSP_CAMERA_POWER_SETTLE_MS);
    }
    return error;
}

/* Exported functions -------------------------------------------------------*/
platform_err_t bsp_camera_init(void)
{
    s_camera_initialized = BSP_CAMERA_NOT_INITIALIZED;

    platform_err_t error = bsp_camera_power_on();
    if(PLATFORM_ERR_OK != error)
    {
        return error;
    }

    ov2640_status_t sensor_status = ov2640_init(
        &s_camera_sensor, &s_camera_sensor_transport,
        BSP_CAMERA_SENSOR_ADDRESS_7B, BSP_CAMERA_SENSOR_TIMEOUT_MS);
    if(OV2640_STATUS_OK != sensor_status)
    {
        return bsp_camera_convert_sensor_status(sensor_status);
    }

    sensor_status = ov2640_configure(
        &s_camera_sensor, OV2640_PROFILE_RGB565_SVGA,
        BSP_CAMERA_SENSOR_OUTPUT_WIDTH, BSP_CAMERA_SENSOR_OUTPUT_HEIGHT,
        BSP_CAMERA_SENSOR_TIMEOUT_MS);
    if(OV2640_STATUS_OK != sensor_status)
    {
        return bsp_camera_convert_sensor_status(sensor_status);
    }

    const plat_dcmi_crop_t crop = {
        .horizontal_offset_bytes = BSP_CAMERA_CROP_X_OFFSET_BYTES,
        .vertical_offset_lines   = BSP_CAMERA_CROP_Y_OFFSET_LINES,
        .capture_width_bytes     = BSP_CAMERA_FRAME_WIDTH *
                                   BSP_CAMERA_BYTES_PER_PIXEL,
        .capture_height_lines    = BSP_CAMERA_FRAME_HEIGHT,
    };
    error = plat_dcmi_config_crop(&crop);
    if(PLATFORM_ERR_OK != error)
    {
        return error;
    }

    s_camera_initialized = BSP_CAMERA_INITIALIZED;
    return PLATFORM_ERR_OK;
}

platform_err_t bsp_camera_start(uint8_t *p_buffer, uint32_t size_bytes)
{
    if((NULL == p_buffer) || (size_bytes < BSP_CAMERA_FRAME_SIZE_BYTES))
    {
        return PLATFORM_ERR_PARAM;
    }
    if(BSP_CAMERA_INITIALIZED != s_camera_initialized)
    {
        return PLATFORM_ERR_HW;
    }

    return plat_dcmi_start_dma(p_buffer, BSP_CAMERA_FRAME_SIZE_BYTES,
                               PLAT_DCMI_MODE_CONTINUOUS);
}

platform_err_t bsp_camera_stop(void)
{
    if(BSP_CAMERA_INITIALIZED != s_camera_initialized)
    {
        return PLATFORM_ERR_HW;
    }

    return plat_dcmi_stop();
}

/* end of file --------------------------------------------------------------*/

