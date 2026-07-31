/**
 ******************************************************************************
 *@file               :   mcu_at24.c
 *@brief              :   STM32 HAL implementation of AT24 I2C transport ops.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "mcu_at24.h"
#include "board_config.h"
#include "i2c.h"
#include "bsp_at24.h"

/* Private functions --------------------------------------------------------*/
static at24_state_t st_dev_is_ready(uint8_t dev_adr, uint32_t trials,
                                     uint32_t timeout)
{
    uint32_t ret = HAL_I2C_IsDeviceReady(&BOARD_EEPROM_I2C_HANDLE,
                                          dev_adr, trials, timeout);
    return (HAL_OK == ret) ? AT24_OK : AT24_ERROR;
}

static at24_state_t st_mem_read(uint16_t dev_adr, uint16_t mem_adr,
                                 uint16_t mem_adr_size, uint8_t *p_data,
                                 uint16_t size, uint32_t timeout)
{
    uint32_t ret = HAL_I2C_Mem_Read(&BOARD_EEPROM_I2C_HANDLE, dev_adr,
                                     mem_adr, mem_adr_size,
                                     (uint8_t *)p_data, size, timeout);
    return (HAL_OK == ret) ? AT24_OK : AT24_ERROR;
}

static at24_state_t st_mem_write(uint16_t dev_adr, uint16_t mem_adr,
                                  uint16_t mem_adr_size, uint8_t *p_data,
                                  uint16_t size, uint32_t timeout)
{
    uint32_t ret = HAL_I2C_Mem_Write(&BOARD_EEPROM_I2C_HANDLE, dev_adr,
                                      mem_adr, mem_adr_size,
                                      (uint8_t *)p_data, size, timeout);
    return (HAL_OK == ret) ? AT24_OK : AT24_ERROR;
}

/* Exported variables -------------------------------------------------------*/
const iic_ops_t g_at24c02_iic_ops = {
    .pf_iic_init        = NULL,
    .pf_iic_deinit      = NULL,
    .pf_iic_dev_isready = st_dev_is_ready,
    .pf_mem_read        = st_mem_read,
    .pf_mem_write       = st_mem_write,
};

/* end of file --------------------------------------------------------------*/
