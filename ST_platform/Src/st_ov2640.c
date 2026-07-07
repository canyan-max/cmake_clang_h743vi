/**
 ******************************************************************************
 *@file               :   st_ov2640.c
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 * 
 *@pardependencies    :   st_ov2640.c
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include "st_ov2640.h"
#include "i2c.h"
#include "dcmi.h"

/* Private functions --------------------------------------------------------*/
/**
 * @brief            :  [st_write_reg]
 * @retval           :  [   OV2640_OK            = 0x00U,
                            OV2640_ERROR         = 0x01U,
                            OV2640_BUSY          = 0x02U,
                            OV2640_TIMEOUT       = 0x03U,
                            OV2640_INVALID_PARAM = 0x04U,]
 * @param[in]        :  [uint8_t reg, uint8_t val]
 */
static ov2640_state_t st_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t           buf[2] = {reg, val};
    HAL_StatusTypeDef ret    = HAL_I2C_Master_Transmit(&hi2c1, OV2640_I2C_ADDR,
                                                       buf, 2U,
                                                       OV2640_I2C_TIMEOUT);
    return (ret == HAL_OK) ? OV2640_OK : OV2640_ERROR;
}
/**
 * @brief            :  [st_read_reg]
 * @retval           :  [   OV2640_OK            = 0x00U,
                            OV2640_ERROR         = 0x01U,
                            OV2640_BUSY          = 0x02U,
                            OV2640_TIMEOUT       = 0x03U,
                            OV2640_INVALID_PARAM = 0x04U,]
 * @param[in]        :  [uint8_t reg, uint8_t *p_val]
 */
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
/**
 * @brief            :  [st_dcmi_start_dma]
 * @retval           :  [   OV2640_OK            = 0x00U,
                            OV2640_ERROR         = 0x01U,
                            OV2640_BUSY          = 0x02U,
                            OV2640_TIMEOUT       = 0x03U,
                            OV2640_INVALID_PARAM = 0x04U,]
 * @param[in]        :  [uint32_t *p_buf, uint32_t len_words,
                         ov2640_dcmi_mode_t mode]
 */
static ov2640_state_t
st_dcmi_start_dma(uint32_t *p_buf, uint32_t len_words, ov2640_dcmi_mode_t mode)
{
    uint32_t hal_mode = (mode == OV2640_DCMI_SNAPSHOT) ? DCMI_MODE_SNAPSHOT
                                                       : DCMI_MODE_CONTINUOUS;
    HAL_StatusTypeDef ret = HAL_DCMI_Start_DMA(&hdcmi, hal_mode,
                                               (uint32_t)p_buf, len_words);
    return (ret == HAL_OK) ? OV2640_OK : OV2640_ERROR;
}
/**
 * @brief            :  [st_dcmi_stop]
 * @retval           :  [   OV2640_OK            = 0x00U,
                            OV2640_ERROR         = 0x01U,
                            OV2640_BUSY          = 0x02U,
                            OV2640_TIMEOUT       = 0x03U,
                            OV2640_INVALID_PARAM = 0x04U,]
 */
static ov2640_state_t st_dcmi_stop(void)
{
    HAL_StatusTypeDef ret = HAL_DCMI_Stop(&hdcmi);
    return (ret == HAL_OK) ? OV2640_OK : OV2640_ERROR;
}
/**
 * @brief            :  [st_config_crop]
 * @retval           :  [   OV2640_OK            = 0x00U,
                            OV2640_ERROR         = 0x01U,
                            OV2640_BUSY          = 0x02U,
                            OV2640_TIMEOUT       = 0x03U,
                            OV2640_INVALID_PARAM = 0x04U,]
 * @param[in]        :  [uint32_t x0, uint32_t y0, 
                         uint32_t xcnt, uint32_t ycnt]
 */
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
/**
 * @brief            :  [st_power_ctrl]
 * @param[in]        :  [uint8_t state]
 */
static void st_power_ctrl(uint8_t state)
{
    HAL_GPIO_WritePin(DCMI_PWDN_GPIO_Port, DCMI_PWDN_Pin,
                      (state == 0U) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
/**
 * @brief            :  [st_delay_ms]
 * @param[in]        :  [uint32_t ms]
 */
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
    .pf_power_ctrl     = st_power_ctrl,
};

/* end of file --------------------------------------------------------------*/
