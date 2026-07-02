/**
 ******************************************************************************
 *@file               :   st_ov2640.c
 *
 *@pardependencies    :   st_ov2640.h
 *                        i2c.h
 *                        dcmi.h
 *
 *@brief              :   STM32H7 HAL implementation of ov2640_hw_ops_t.
 *                        Uses hi2c1 for SCCB and hdcmi for DCMI DMA.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "st_ov2640.h"
#include "i2c.h"
#include "dcmi.h"

/* Private functions --------------------------------------------------------*/
static ov2640_state_t st_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t           buf[2] = {reg, val};
    HAL_StatusTypeDef ret    = HAL_I2C_Master_Transmit(&hi2c1, OV2640_I2C_ADDR,
                                                       buf, 2U,
                                                       OV2640_I2C_TIMEOUT);
    return (ret == HAL_OK) ? OV2640_OK : OV2640_ERROR;
}

static ov2640_state_t st_read_reg(uint8_t reg, uint8_t *p_val)
{
    if(NULL == p_val)
    {
        return OV2640_INVALID_PARAM;
    }
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(&hi2c1, OV2640_I2C_ADDR,
                                                    &reg, 1U,
                                                    OV2640_I2C_TIMEOUT);
    if(ret != HAL_OK)
    {
        return OV2640_ERROR;
    }
    ret = HAL_I2C_Master_Receive(&hi2c1, OV2640_I2C_ADDR, p_val, 1U,
                                 OV2640_I2C_TIMEOUT);
    return (ret == HAL_OK) ? OV2640_OK : OV2640_ERROR;
}

static ov2640_state_t
st_dcmi_start_dma(uint32_t *p_buf, uint32_t len_words, ov2640_dcmi_mode_t mode)
{
    uint32_t hal_mode = (mode == OV2640_DCMI_SNAPSHOT) ? DCMI_MODE_SNAPSHOT
                                                       : DCMI_MODE_CONTINUOUS;
    HAL_StatusTypeDef ret = HAL_DCMI_Start_DMA(&hdcmi, hal_mode,
                                               (uint32_t)p_buf, len_words);
    return (ret == HAL_OK) ? OV2640_OK : OV2640_ERROR;
}

static ov2640_state_t st_dcmi_stop(void)
{
    HAL_StatusTypeDef ret = HAL_DCMI_Stop(&hdcmi);
    return (ret == HAL_OK) ? OV2640_OK : OV2640_ERROR;
}

static ov2640_state_t
st_config_crop(uint32_t x0, uint32_t y0, uint32_t xcnt, uint32_t ycnt)
{
    HAL_StatusTypeDef ret = HAL_DCMI_ConfigCrop(&hdcmi, x0, y0, xcnt, ycnt);
    if(ret != HAL_OK)
    {
        return OV2640_ERROR;
    }
    HAL_DCMI_EnableCrop(&hdcmi);
    return OV2640_OK;
}

static void st_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

/* Exported variables -------------------------------------------------------*/
const ov2640_hw_ops_t g_ov2640_hw_ops = {
    .pf_write_reg      = st_write_reg,
    .pf_read_reg       = st_read_reg,
    .pf_dcmi_start_dma = st_dcmi_start_dma,
    .pf_dcmi_stop      = st_dcmi_stop,
    .pf_config_crop    = st_config_crop,
    .pf_delay_ms       = st_delay_ms,
};

/* end of file --------------------------------------------------------------*/
