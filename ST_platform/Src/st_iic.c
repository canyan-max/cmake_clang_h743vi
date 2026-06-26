/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   st_iic.c
 *
 *@pardependencies    :   st_iic.c
 *                        i2c.h
 *                        bsp_drv_at24.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of description.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>       /* stdint lib header file. */
#include "st_iic.h"       /* st_iic lib header file. */
#include "i2c.h"          /* i2c lib header file. */
#include "bsp_drv_at24.h" /* bsp_drv_at24 lib header file. */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static at24_state_t st_mem_read(void    *hi2c,
                                uint16_t dev_adr,
                                uint16_t mem_adr,
                                uint16_t mem_adr_size,
                                uint8_t *p_data,
                                uint16_t size,
                                uint32_t timeout);
static at24_state_t st_mem_write(void    *hi2c,
                                 uint16_t dev_adr,
                                 uint16_t mem_adr,
                                 uint16_t mem_adr_size,
                                 uint8_t *p_data,
                                 uint16_t size,
                                 uint32_t timeout);
static at24_state_t
st_dev_is_ready(void *hi2c, uint8_t dev_adr, uint32_t tails, uint32_t timeout);
const iic_ops_t g_at24c02_iic_ops = {
    .p_iic_handle       = (void *)&hi2c1,
    .pf_iic_init        = NULL,
    .pf_iic_deinit      = NULL,
    .pf_iic_dev_isready = st_dev_is_ready,
    .pf_mem_read        = st_mem_read,
    .pf_mem_write       = st_mem_write,
};
/* Private  functions  ------------------------------------------------------*/
static at24_state_t st_mem_read(void    *hi2c,
                                uint16_t dev_adr,
                                uint16_t mem_adr,
                                uint16_t mem_adr_size,
                                uint8_t *p_data,
                                uint16_t size,
                                uint32_t timeout)
{
    I2C_HandleTypeDef *p_hi2c = (I2C_HandleTypeDef *)hi2c;
    uint32_t hal_ret = HAL_I2C_Mem_Read(p_hi2c, dev_adr, mem_adr, mem_adr_size,
                                        (uint8_t *)p_data, size, timeout);
    if(HAL_OK != hal_ret)
    {
        return AT24_ERROR;
    }
    return AT24_OK;
}

static at24_state_t st_mem_write(void    *hi2c,
                                 uint16_t dev_adr,
                                 uint16_t mem_adr,
                                 uint16_t mem_adr_size,
                                 uint8_t *p_data,
                                 uint16_t size,
                                 uint32_t timeout)
{
    I2C_HandleTypeDef *p_hi2c = (I2C_HandleTypeDef *)hi2c;
    uint32_t hal_ret = HAL_I2C_Mem_Write(p_hi2c, dev_adr, mem_adr, mem_adr_size,
                                         (uint8_t *)p_data, size, timeout);
    if(HAL_OK != hal_ret)
    {
        return AT24_ERROR;
    }
    return AT24_OK;
}

static at24_state_t
st_dev_is_ready(void *hi2c, uint8_t dev_adr, uint32_t tails, uint32_t timeout)
{
    I2C_HandleTypeDef *p_hi2c = (I2C_HandleTypeDef *)hi2c;
    uint32_t hal_ret = HAL_I2C_IsDeviceReady(p_hi2c, dev_adr, tails, timeout);
    if(HAL_OK != hal_ret)
    {
        return AT24_ERROR;
    }
    return AT24_OK;
}
/* Exported functions -------------------------------------------------------*/

/* end of  file -------------------------------------------------------------*/
