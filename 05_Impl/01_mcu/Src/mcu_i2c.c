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
                                            uint8_t       address_7b,
                                            uint32_t      timeout_ms)
{
    return (uint8_t)((NULL != plat_i2c_get_handle(id)) &&
                     (address_7b <= PLAT_I2C_ADDRESS_7B_MAX) &&
                     (0U != timeout_ms) && (UINT32_MAX != timeout_ms));
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

static uint8_t plat_i2c_memory_address_size_is_valid(
    plat_i2c_memory_address_size_t memory_address_size)
{
    switch(memory_address_size)
    {
        case PLAT_I2C_MEMORY_ADDRESS_SIZE_8BIT:
        case PLAT_I2C_MEMORY_ADDRESS_SIZE_16BIT:
            return 1U;

        default:
            return 0U;
    }
}

static uint16_t plat_i2c_get_hal_memory_address_size(
    plat_i2c_memory_address_size_t memory_address_size)
{
    switch(memory_address_size)
    {
        case PLAT_I2C_MEMORY_ADDRESS_SIZE_8BIT:
            return I2C_MEMADD_SIZE_8BIT;

        case PLAT_I2C_MEMORY_ADDRESS_SIZE_16BIT:
            return I2C_MEMADD_SIZE_16BIT;

        default:
            return 0U;
    }
}

/* Exported functions -------------------------------------------------------*/
platform_err_t plat_i2c_write(plat_i2c_id_t  id,
                              uint8_t        address_7b,
                              const uint8_t *p_data,
                              uint16_t       size,
                              uint32_t       timeout_ms)
{
    if((0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms)) ||
       (NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(plat_i2c_get_handle(id),
                                                       (uint16_t)address_7b
                                                           << 1U,
                                                       (uint8_t *)p_data, size,
                                                       timeout_ms);
    return plat_i2c_convert_hal_status(status);
}

platform_err_t plat_i2c_read(plat_i2c_id_t id,
                             uint8_t       address_7b,
                             uint8_t      *p_data,
                             uint16_t      size,
                             uint32_t      timeout_ms)
{
    if((0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms)) ||
       (NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(plat_i2c_get_handle(id),
                                                      (uint16_t)address_7b
                                                          << 1U,
                                                      p_data, size, timeout_ms);
    return plat_i2c_convert_hal_status(status);
}

platform_err_t
plat_i2c_memory_write(plat_i2c_id_t                  id,
                      uint8_t                        address_7b,
                      uint16_t                       memory_address,
                      plat_i2c_memory_address_size_t memory_address_size,
                      const uint8_t                 *p_data,
                      uint16_t                       size,
                      uint32_t                       timeout_ms)
{
    if((0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms)) ||
       (0U == plat_i2c_memory_address_size_is_valid(memory_address_size)) ||
       (NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_StatusTypeDef
        status = HAL_I2C_Mem_Write(plat_i2c_get_handle(id),
                                   (uint16_t)address_7b << 1U, memory_address,
                                   plat_i2c_get_hal_memory_address_size(
                                       memory_address_size),
                                   (uint8_t *)p_data, size, timeout_ms);
    return plat_i2c_convert_hal_status(status);
}

platform_err_t
plat_i2c_memory_read(plat_i2c_id_t                  id,
                     uint8_t                        address_7b,
                     uint16_t                       memory_address,
                     plat_i2c_memory_address_size_t memory_address_size,
                     uint8_t                       *p_data,
                     uint16_t                       size,
                     uint32_t                       timeout_ms)
{
    if((0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms)) ||
       (0U == plat_i2c_memory_address_size_is_valid(memory_address_size)) ||
       (NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_StatusTypeDef
        status = HAL_I2C_Mem_Read(plat_i2c_get_handle(id),
                                  (uint16_t)address_7b << 1U, memory_address,
                                  plat_i2c_get_hal_memory_address_size(
                                      memory_address_size),
                                  p_data, size, timeout_ms);
    return plat_i2c_convert_hal_status(status);
}

platform_err_t
plat_i2c_is_ready(plat_i2c_id_t id, uint8_t address_7b, uint32_t timeout_ms)
{
    if(0U == plat_i2c_common_params_valid(id, address_7b, timeout_ms))
    {
        return PLATFORM_ERR_PARAM;
    }

    I2C_HandleTypeDef *p_handle = plat_i2c_get_handle(id);
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(p_handle,
                                                     (uint16_t)address_7b << 1U,
                                                     1U, timeout_ms);
    /* STM32H7 HAL reports an exhausted one-trial NACK probe as TIMEOUT. The
     * BSP performs the bounded total wait, so these probe-only errors mean
     * "not ready yet" rather than a permanent bus fault. */
    uint32_t error           = HAL_I2C_GetError(p_handle);
    uint32_t not_ready_error = HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT;
    if((HAL_ERROR == status) && (0U != (error & not_ready_error)) &&
       (0U == (error & ~not_ready_error)))
    {
        return PLATFORM_ERR_BUSY;
    }

    return plat_i2c_convert_hal_status(status);
}

/* end of file --------------------------------------------------------------*/
