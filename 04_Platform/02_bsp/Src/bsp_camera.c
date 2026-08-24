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
#include "board_resources.h"

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
#define BSP_CAMERA_FRAME_BUFFER_COUNT       (2U)
#define BSP_CAMERA_BUFFER_INDEX_INVALID     (0xFFU)

/* typedef ------------------------------------------------------------------*/
typedef enum BSP_CAMERA_BUFFER_STATE_T
{
    BSP_CAMERA_BUFFER_FREE = 0U,
    BSP_CAMERA_BUFFER_CAPTURING,
    BSP_CAMERA_BUFFER_READY,
    BSP_CAMERA_BUFFER_IN_USE,
} bsp_camera_buffer_state_t;

/* variables ----------------------------------------------------------------*/
static ov2640_driver_t s_camera_sensor;
static uint8_t         s_camera_initialized;
static bsp_camera_event_cb_t volatile s_camera_event_callback;
__attribute__((section(".ram_d2_dma_buffers"), aligned(32))) static uint8_t
    s_camera_frame_buffers[BSP_CAMERA_FRAME_BUFFER_COUNT]
                          [BSP_CAMERA_FRAME_SIZE_BYTES];
static volatile bsp_camera_buffer_state_t
    s_camera_buffer_states[BSP_CAMERA_FRAME_BUFFER_COUNT];
static volatile uint8_t  s_camera_capture_buffer_index;
static volatile uint8_t  s_camera_ready_buffer_index;
static uint8_t           s_camera_dma_needs_stop;
static volatile uint32_t s_camera_overrun_count;
static volatile uint32_t s_camera_error_count;

/* Private  functions  ------------------------------------------------------*/
static ov2640_status_t bsp_camera_convert_platform_error(platform_err_t error)
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
        plat_i2c_write(BOARD_I2C_CAMERA_BUS, address_7b, data, sizeof(data),
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

    platform_err_t error = plat_i2c_write(BOARD_I2C_CAMERA_BUS, address_7b, &reg, 1U,
                                          timeout_ms);
    if(PLATFORM_ERR_OK != error)
    {
        return bsp_camera_convert_platform_error(error);
    }

    return bsp_camera_convert_platform_error(
        plat_i2c_read(BOARD_I2C_CAMERA_BUS, address_7b, p_value, 1U, timeout_ms));
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
    platform_err_t error   = plat_gpio_write(BOARD_GPIO_CAMERA_PWDN,
                                              power_on_level);
    if(PLATFORM_ERR_OK == error)
    {
        plat_delay_ms(BSP_CAMERA_POWER_SETTLE_MS);
    }
    return error;
}

static void bsp_camera_dcmi_event_from_isr(plat_dcmi_event_t event)
{
    bsp_camera_event_t camera_event;
    switch(event)
    {
        case PLAT_DCMI_EVENT_FRAME_READY:
            if((s_camera_capture_buffer_index >=
                BSP_CAMERA_FRAME_BUFFER_COUNT) ||
               (BSP_CAMERA_BUFFER_CAPTURING !=
                s_camera_buffer_states[s_camera_capture_buffer_index]))
            {
                s_camera_error_count++;
                camera_event = BSP_CAMERA_EVENT_ERROR;
                break;
            }
            s_camera_buffer_states[s_camera_capture_buffer_index] =
                BSP_CAMERA_BUFFER_READY;
            s_camera_ready_buffer_index   = s_camera_capture_buffer_index;
            s_camera_capture_buffer_index = BSP_CAMERA_BUFFER_INDEX_INVALID;
            camera_event = BSP_CAMERA_EVENT_FRAME_READY;
            break;

        case PLAT_DCMI_EVENT_OVERRUN:
            if(s_camera_capture_buffer_index <
               BSP_CAMERA_FRAME_BUFFER_COUNT)
            {
                s_camera_buffer_states[s_camera_capture_buffer_index] =
                    BSP_CAMERA_BUFFER_FREE;
                s_camera_capture_buffer_index =
                    BSP_CAMERA_BUFFER_INDEX_INVALID;
            }
            camera_event = BSP_CAMERA_EVENT_OVERRUN;
            s_camera_overrun_count++;
            break;

        case PLAT_DCMI_EVENT_ERROR:
        case PLAT_DCMI_EVENT_NUM:
        default:
            if(s_camera_capture_buffer_index <
               BSP_CAMERA_FRAME_BUFFER_COUNT)
            {
                s_camera_buffer_states[s_camera_capture_buffer_index] =
                    BSP_CAMERA_BUFFER_FREE;
                s_camera_capture_buffer_index =
                    BSP_CAMERA_BUFFER_INDEX_INVALID;
            }
            camera_event = BSP_CAMERA_EVENT_ERROR;
            s_camera_error_count++;
            break;
    }

    bsp_camera_event_cb_t callback = s_camera_event_callback;
    if(NULL != callback)
    {
        callback(camera_event);
    }
}

/* Exported functions -------------------------------------------------------*/
platform_err_t bsp_camera_set_event_callback(bsp_camera_event_cb_t callback)
{
    if(BSP_CAMERA_INITIALIZED == s_camera_initialized)
    {
        return PLATFORM_ERR_BUSY;
    }

    s_camera_event_callback = callback;
    return PLATFORM_ERR_OK;
}

platform_err_t bsp_camera_init(void)
{
    uint8_t buffer_index;

    s_camera_initialized   = BSP_CAMERA_NOT_INITIALIZED;
    s_camera_overrun_count = 0U;
    s_camera_error_count   = 0U;
    s_camera_capture_buffer_index = BSP_CAMERA_BUFFER_INDEX_INVALID;
    s_camera_ready_buffer_index   = BSP_CAMERA_BUFFER_INDEX_INVALID;
    s_camera_dma_needs_stop       = 0U;
    for(buffer_index = 0U; buffer_index < BSP_CAMERA_FRAME_BUFFER_COUNT;
        buffer_index++)
    {
        s_camera_buffer_states[buffer_index] = BSP_CAMERA_BUFFER_FREE;
    }

    platform_err_t error = plat_dcmi_set_event_callback(
        bsp_camera_dcmi_event_from_isr);
    if(PLATFORM_ERR_OK != error)
    {
        return error;
    }

    error = bsp_camera_power_on();
    if(PLATFORM_ERR_OK != error)
    {
        return error;
    }

    ov2640_status_t sensor_status = ov2640_init(&s_camera_sensor,
                                                &s_camera_sensor_transport,
                                                BSP_CAMERA_SENSOR_ADDRESS_7B,
                                                BSP_CAMERA_SENSOR_TIMEOUT_MS);
    if(OV2640_STATUS_OK != sensor_status)
    {
        return bsp_camera_convert_sensor_status(sensor_status);
    }

    sensor_status = ov2640_configure(&s_camera_sensor,
                                     OV2640_PROFILE_RGB565_CIF_60FPS,
                                     BSP_CAMERA_SENSOR_OUTPUT_WIDTH,
                                     BSP_CAMERA_SENSOR_OUTPUT_HEIGHT,
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

platform_err_t bsp_camera_start(void)
{
    if(BSP_CAMERA_INITIALIZED != s_camera_initialized)
    {
        return PLATFORM_ERR_HW;
    }

    return bsp_camera_capture_next();
}

platform_err_t bsp_camera_capture_next(void)
{
    uint8_t buffer_index;

    if(BSP_CAMERA_INITIALIZED != s_camera_initialized)
    {
        return PLATFORM_ERR_HW;
    }
    if(s_camera_capture_buffer_index < BSP_CAMERA_FRAME_BUFFER_COUNT)
    {
        return PLATFORM_ERR_BUSY;
    }

    for(buffer_index = 0U; buffer_index < BSP_CAMERA_FRAME_BUFFER_COUNT;
        buffer_index++)
    {
        if(BSP_CAMERA_BUFFER_FREE == s_camera_buffer_states[buffer_index])
        {
            break;
        }
    }
    if(buffer_index >= BSP_CAMERA_FRAME_BUFFER_COUNT)
    {
        return PLATFORM_ERR_BUSY;
    }

    if(0U != s_camera_dma_needs_stop)
    {
        platform_err_t status = plat_dcmi_stop();
        if(PLATFORM_ERR_OK != status)
        {
            return status;
        }
        s_camera_dma_needs_stop = 0U;
    }

    s_camera_buffer_states[buffer_index] = BSP_CAMERA_BUFFER_CAPTURING;
    s_camera_capture_buffer_index        = buffer_index;
    platform_err_t status = plat_dcmi_start_dma(
        s_camera_frame_buffers[buffer_index], BSP_CAMERA_FRAME_SIZE_BYTES,
        PLAT_DCMI_MODE_SNAPSHOT);
    if(PLATFORM_ERR_OK != status)
    {
        s_camera_buffer_states[buffer_index] = BSP_CAMERA_BUFFER_FREE;
        s_camera_capture_buffer_index = BSP_CAMERA_BUFFER_INDEX_INVALID;
        return status;
    }

    s_camera_dma_needs_stop = 1U;
    return PLATFORM_ERR_OK;
}

platform_err_t bsp_camera_stop(void)
{
    if(BSP_CAMERA_INITIALIZED != s_camera_initialized)
    {
        return PLATFORM_ERR_HW;
    }

    platform_err_t status = PLATFORM_ERR_OK;
    if(0U != s_camera_dma_needs_stop)
    {
        status = plat_dcmi_stop();
        if(PLATFORM_ERR_OK != status)
        {
            return status;
        }
        s_camera_dma_needs_stop = 0U;
    }

    if(s_camera_capture_buffer_index < BSP_CAMERA_FRAME_BUFFER_COUNT)
    {
        s_camera_buffer_states[s_camera_capture_buffer_index] =
            BSP_CAMERA_BUFFER_FREE;
        s_camera_capture_buffer_index = BSP_CAMERA_BUFFER_INDEX_INVALID;
    }
    return status;
}

platform_err_t bsp_camera_acquire_frame(const uint8_t **pp_frame)
{
    if(NULL == pp_frame)
    {
        return PLATFORM_ERR_PARAM;
    }
    *pp_frame = NULL;
    if(BSP_CAMERA_INITIALIZED != s_camera_initialized)
    {
        return PLATFORM_ERR_HW;
    }

    uint8_t buffer_index = s_camera_ready_buffer_index;
    if((buffer_index >= BSP_CAMERA_FRAME_BUFFER_COUNT) ||
       (BSP_CAMERA_BUFFER_READY != s_camera_buffer_states[buffer_index]))
    {
        return PLATFORM_ERR_BUSY;
    }

    s_camera_buffer_states[buffer_index] = BSP_CAMERA_BUFFER_IN_USE;
    s_camera_ready_buffer_index          = BSP_CAMERA_BUFFER_INDEX_INVALID;
    plat_dcache_invalidate(s_camera_frame_buffers[buffer_index],
                           (int32_t)BSP_CAMERA_FRAME_SIZE_BYTES);
    *pp_frame = s_camera_frame_buffers[buffer_index];
    return PLATFORM_ERR_OK;
}

platform_err_t bsp_camera_release_frame(const uint8_t *p_frame)
{
    uint8_t buffer_index;

    if(NULL == p_frame)
    {
        return PLATFORM_ERR_PARAM;
    }
    for(buffer_index = 0U; buffer_index < BSP_CAMERA_FRAME_BUFFER_COUNT;
        buffer_index++)
    {
        if(p_frame == s_camera_frame_buffers[buffer_index])
        {
            if(BSP_CAMERA_BUFFER_IN_USE !=
               s_camera_buffer_states[buffer_index])
            {
                return PLATFORM_ERR_BUSY;
            }
            s_camera_buffer_states[buffer_index] = BSP_CAMERA_BUFFER_FREE;
            return PLATFORM_ERR_OK;
        }
    }
    return PLATFORM_ERR_PARAM;
}

uint32_t bsp_camera_get_frame_buffer_size(void)
{
    return BSP_CAMERA_FRAME_SIZE_BYTES;
}

uint32_t bsp_camera_get_overrun_count(void)
{
    return s_camera_overrun_count;
}

uint32_t bsp_camera_get_error_count(void)
{
    return s_camera_error_count;
}

/* end of file --------------------------------------------------------------*/
