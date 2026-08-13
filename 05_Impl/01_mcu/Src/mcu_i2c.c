/**
 ******************************************************************************
 *@file               :   mcu_i2c.c
 *@brief              :   STM32 HAL implementation of the portable I2C API.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "plat_i2c.h"
#include "board_config.h"
#include "i2c.h"

/* define   -----------------------------------------------------------------*/
#define PLAT_I2C_ADDRESS_7B_MAX  (0x7FU)

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static I2C_HandleTypeDef *const s_i2c_handle_table[PLAT_I2C_ID_NUM] = {
    [PLAT_I2C_ID_0] = &BOARD_I2C0_HANDLE,
};

/* Private  functions  ------------------------------------------------------*/
static I2C_HandleTypeDef *plat_i2c_get_handle(plat_i2c_id_t id)
{
    if((uint32_t)id >= (uint32_t)PLAT_I2C_ID_NUM)
    {
        return NULL;
    }

    return s_i2c_handle_table[id];
}

static uint8_t plat_i2c_common_params_valid(plat_i2c_id_t id,
                                            uint8_t address_7b,
                                            uint32_t timeout_ms)
{
    return (uint8_t)((NULL != plat_i2c_get_handle(id)) &&
                     (address_7b <= PLAT_I2C_ADDRESS_7B_MAX) &&
                     (0U != timeout_ms) &&
                     (UINT32_MAX != timeout_ms));
}

static platform_err_t plat_i2c_convert_hal_status(HAL_StatusTypeDef status)
{
    switch(status)
    {
        case HAL_OK:
            return PLATFORM_ERR_OK;

        case HAL_BUSY:
            return PLATFORM_ERR_BUSY;

        case HAL_TIMEOUT:
            return PLATFORM_ERR_TIMEOUT;

        case HAL_ERROR:
        default:
            return PLATFORM_ERR_HW;
    }
}

/* Exported functions -------------------------------------------------------*/
platform_err_t plat_i2c_write(plat_i2c_id_t id,
                              uint8_t address_7b,
                              const uint8_t *p_data,
                              uint16_t size,
                              uint32_t timeout_ms)
{
    if((0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms)) ||
       (NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(
        plat_i2c_get_handle(id), (uint16_t)address_7b << 1U,
        (uint8_t *)p_data, size, timeout_ms);
    return plat_i2c_convert_hal_status(status);
}

platform_err_t plat_i2c_read(plat_i2c_id_t id,
                             uint8_t address_7b,
                             uint8_t *p_data,
                             uint16_t size,
                             uint32_t timeout_ms)
{
    if((0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms)) ||
       (NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(
        plat_i2c_get_handle(id), (uint16_t)address_7b << 1U,
        p_data, size, timeout_ms);
    return plat_i2c_convert_hal_status(status);
}

platform_err_t plat_i2c_write_read(plat_i2c_id_t id,
                                   uint8_t address_7b,
                                   const uint8_t *p_tx_data,
                                   uint16_t tx_size,
                                   uint8_t *p_rx_data,
                                   uint16_t rx_size,
                                   uint32_t timeout_ms)
{
    if((0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms)) ||
       (NULL == p_tx_data) || (NULL == p_rx_data) || (0U == tx_size) ||
       (tx_size > PLAT_I2C_WRITE_READ_PREFIX_MAX_SIZE) || (0U == rx_size))
    {
        return PLATFORM_ERR_PARAM;
    }

    uint16_t prefix = p_tx_data[0U];
    uint16_t address_size = I2C_MEMADD_SIZE_8BIT;
    if(2U == tx_size)
    {
        prefix = (uint16_t)(((uint16_t)p_tx_data[0U] << 8U) |
                            (uint16_t)p_tx_data[1U]);
        address_size = I2C_MEMADD_SIZE_16BIT;
    }

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        plat_i2c_get_handle(id), (uint16_t)address_7b << 1U,
        prefix, address_size, p_rx_data, rx_size, timeout_ms);
    return plat_i2c_convert_hal_status(status);
}

platform_err_t plat_i2c_is_ready(plat_i2c_id_t id,
                                 uint8_t address_7b,
                                 uint32_t timeout_ms)
{
    if(0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms))
    {
        return PLATFORM_ERR_PARAM;
    }

    I2C_HandleTypeDef *p_handle = plat_i2c_get_handle(id);
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(
        p_handle, (uint16_t)address_7b << 1U, 1U, timeout_ms);
    if((HAL_ERROR == status) &&
       (0U != (HAL_I2C_GetError(p_handle) & HAL_I2C_ERROR_AF)))
    {
        return PLATFORM_ERR_BUSY;
    }

    return plat_i2c_convert_hal_status(status);
}

/* end of file --------------------------------------------------------------*/
