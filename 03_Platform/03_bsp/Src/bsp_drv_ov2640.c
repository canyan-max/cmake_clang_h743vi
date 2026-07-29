/**
 ******************************************************************************
 *@file               :   bsp_drv_ov2640.c
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 * 
 *@pardependencies    :   bsp_drv_ov2640.c
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "bsp_drv_ov2640.h"

/* Private variables --------------------------------------------------------*/

/* SVGA 模式寄存器配置表（传感器 800×600，DSP 输出 400×300 RGB565，~29fps）*/
static const uint8_t ov2640_svga_cfg[][2] = {

    {0xff, 0x01}, // 设置 sensor 寄存器组
    {0x12, 0x80}, // 复位全部寄存

    {0xff, 0x00}, // 设置DSP寄存器租

    {0x2c, 0xff},

    {0x2e, 0xdf},

    {0xff, 0x01}, // 设置 sensor 寄存器组

    {0x3c, 0x32}, {0x2c, 0x0c}, {0x33, 0x78}, {0x3a, 0x33}, {0x3b, 0xfB},
    {0x3e, 0x00}, {0x43, 0x11}, {0x16, 0x10}, {0x39, 0x92}, {0x35, 0xda},
    {0x22, 0x1a}, {0x37, 0xc3}, {0x36, 0x1a}, {0x4c, 0x00}, {0x5B, 0x00},
    {0x42, 0x03}, {0x4a, 0x81}, {0x21, 0x99}, {0x5c, 0x00}, {0x63, 0x00},
    {0x7c, 0x05}, {0x6c, 0x00}, {0x6d, 0x80}, {0x6e, 0x00}, {0x70, 0x02},
    {0x71, 0x94}, {0x73, 0xc1}, {0x20, 0x80}, {0x28, 0x30}, {0x37, 0xc0},
    {0x3d, 0x38}, {0x6d, 0x00}, {0x23, 0x00}, {0x06, 0x88}, {0x07, 0xc0},
    {0x0d, 0x87}, {0x0e, 0x41},

    {0x11, 0x80}, // CLKRC，bit7=1 旁路分频器，系统时钟 = XVCLK = 24MHz
    {0x09, 0x02}, // COM2，输出驱动能力
    {0x04, 0xA8}, // REG04，扫描方向等
    {0x12, 0x40}, // COM7，SVGA 分辨率
    {0x14, 0x48}, // COM9，增益设置
    {0x15, 0x00}, // COM10，极性控制
    {0x32, 0x09}, // REG32，像素时钟分频及窗口低3位
    {0x03, 0x86}, // COM1，无效帧及垂直窗口低2位
    {0x46, 0x00}, // FLL，帧率调整（0 = 最短帧长，最高帧率）
    {0x24, 0x40}, // AEW，AEC/AGC 上限亮度
    {0x25, 0x38}, // AEB，AEC/AGC 下限亮度
    {0x26, 0x82}, // VV，快速步进阈值
    {0x34, 0xc0}, // ARCOM2，缩放窗口水平起始
    {0x61, 0x70}, // HISTO_LOW
    {0x62, 0x80}, // HISTO_HIGH

    {0x17, 0x11}, // HREFST，水平窗口起始（高8位）
    {0x18, 0x43}, // HREFEND，水平窗口结束（高8位）
    {0x19, 0x00}, // VSTRT，垂直窗口起始（高8位）
    {0x1a, 0x97}, // VEND，垂直窗口结束（高8位）

    {0x13, 0xe5}, // COM8，自动曝光/增益/滤波
    {0x0c, 0x3a}, // COM3，带宽设置
    {0x4f, 0xbb}, // BD50，50Hz 带宽 AEC
    {0x50, 0x9c}, // BD60，60Hz 带宽 AEC
    {0x5a, 0x23},

    {0xff, 0x00}, // 设置DSP寄存器组

    {0xe5, 0x7f}, {0x41, 0x24}, {0x76, 0xff}, {0x33, 0xa0}, {0x42, 0x20},
    {0x43, 0x18}, {0x4c, 0x00}, {0xd7, 0x03}, {0xd9, 0x10}, {0x88, 0x3f},
    {0xc8, 0x08}, {0xc9, 0x80}, {0x7c, 0x00}, {0x7d, 0x00}, {0x7c, 0x03},
    {0x7d, 0x48}, {0x7d, 0x48}, {0x7c, 0x08}, {0x7d, 0x20}, {0x7d, 0x10},
    {0x7d, 0x0e}, {0x90, 0x00}, {0x91, 0x0e}, {0x91, 0x1a}, {0x91, 0x31},
    {0x91, 0x5a}, {0x91, 0x69}, {0x91, 0x75}, {0x91, 0x7e}, {0x91, 0x88},
    {0x91, 0x8f}, {0x91, 0x96}, {0x91, 0xa3}, {0x91, 0xaf}, {0x91, 0xc4},
    {0x91, 0xd7}, {0x91, 0xe8}, {0x91, 0x20}, {0x92, 0x00}, {0x93, 0x06},
    {0x93, 0xe3}, {0x93, 0x05}, {0x93, 0x05}, {0x93, 0x00}, {0x93, 0x04},
    {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00},
    {0x93, 0x00}, {0x93, 0x00}, {0x96, 0x00}, {0x97, 0x08}, {0x97, 0x19},
    {0x97, 0x02}, {0x97, 0x0c}, {0x97, 0x24}, {0x97, 0x30}, {0x97, 0x28},
    {0x97, 0x26}, {0x97, 0x02}, {0x97, 0x98}, {0x97, 0x80}, {0x97, 0x00},
    {0x97, 0x00}, {0xa4, 0x00}, {0xa8, 0x00}, {0xc5, 0x11}, {0xc6, 0x51},
    {0xbf, 0x80}, {0xc7, 0x10}, {0xb6, 0x66}, {0xb8, 0xA5}, {0xb7, 0x64},
    {0xb9, 0x7C}, {0xb3, 0xaf}, {0xb4, 0x97}, {0xb5, 0xFF}, {0xb0, 0xC5},
    {0xb1, 0x94}, {0xb2, 0x0f}, {0xc4, 0x5c}, {0x7f, 0x00},

    {0xf9, 0xc0}, // MC_BIST
    {0xe0, 0x14}, // RESET
    {0x87, 0xd0}, // CTRL3
    {0xc3, 0xed}, // CTRL1
    {0xc2, 0x0e}, // CTRL0，使能 RGB_EN
    {0x86, 0x3d}, // CTRL2
    {0xda, 0x08}, // 图像输出模式：RGB565

    {0xc0, 0x64}, // 图像水平尺寸 10~3 bit
    {0xc1, 0x4b}, // 图像垂直尺寸 10~3 bit
    {0x8c, 0x00},

    {0x50, 0x00}, // CTRLI，水平/垂直分频器
    {0x51, 0xc8}, // 水平尺寸 7~0 bit（= 200，×4 = 800）
    {0x52, 0x96}, // 垂直尺寸 7~0 bit（= 150，×4 = 600）
    {0x53, 0x00}, {0x54, 0x00}, {0x55, 0x00}, {0x57, 0x00},

    {0xd3, 0x80}, // R_DVP_SP，PCLK = 2×XVCLK / 1 = 48MHz

    {0xe5, 0x1f}, {0xe1, 0x67}, {0xdd, 0x7f},

    {0xe0, 0x00}, // RESET，取消复位
    {0x05, 0x00}, // 使能 DSP

    {0, 0}, /* 配置表结束标志 */
};

/* Private functions --------------------------------------------------------*/

/**
 * @brief            : [ov2640_apply_config] 逐条写入寄存器配置表
 * @retval           : [OV2640_OK / OV2640_ERROR]
 * @param[in]        : [p_drv, cfg — 以 {0,0} 结尾的配置表]
 */
static ov2640_state_t ov2640_apply_config(ov2640_driver_t *p_drv,
                                          const uint8_t (*cfg)[2])
{
    for(uint32_t i = 0U; cfg[i][0] != 0U; i++)
    {
        if(p_drv->p_hw_ops->pf_write_reg(cfg[i][0], cfg[i][1]) != OV2640_OK)
        {
            return OV2640_ERROR;
        }
        if(cfg[i][0] == 0xFFU)
        {
            /* Bank 切换后等待稳定 */
            p_drv->p_hw_ops->pf_delay_ms(2U);
        }
        else if(cfg[i][0] == 0x12U && cfg[i][1] == 0x80U)
        {
            /* 软件复位后等待传感器就绪 */
            p_drv->p_hw_ops->pf_delay_ms(50U);
        }
        else
        {
            /* nothing */
        }
    }
    return OV2640_OK;
}

static ov2640_state_t
ov2640_drv_set_out_size(ov2640_driver_t *p_drv, uint16_t w, uint16_t h)
{
    if(NULL == p_drv || 0U == w || 0U == h)
    {
        return OV2640_INVALID_PARAM;
    }
    const ov2640_hw_ops_t *ops = p_drv->p_hw_ops;
    ops->pf_write_reg(0xFFU, 0x00U);
    ops->pf_write_reg(0x5AU, (uint8_t)((w / 4U) & 0xFFU));
    ops->pf_write_reg(0x5BU, (uint8_t)((h / 4U) & 0xFFU));
    ops->pf_write_reg(0x5CU, (uint8_t)(((w / 4U) >> 8U & 0x03U) |
                                       ((h / 4U) >> 6U & 0x04U)));
    ops->pf_write_reg(0xE0U, 0x00U);
    ops->pf_delay_ms(50U);
    return OV2640_OK;
}

static ov2640_state_t
ov2640_drv_start(ov2640_driver_t *p_drv, uint32_t *p_buf, uint32_t len_words)
{
    if(NULL == p_drv || NULL == p_buf)
    {
        return OV2640_INVALID_PARAM;
    }
    return p_drv->p_hw_ops->pf_dcmi_start_dma(p_buf, len_words,
                                              OV2640_DCMI_CONTINUOUS);
}

static ov2640_state_t ov2640_drv_stop(ov2640_driver_t *p_drv)
{
    if(NULL == p_drv)
    {
        return OV2640_INVALID_PARAM;
    }
    return p_drv->p_hw_ops->pf_dcmi_stop();
}

/* Exported functions -------------------------------------------------------*/

/**
 * @brief            : [ov2640_driver_instruct]
 * @retval           : [OV2640_OK / OV2640_ERROR / OV2640_INVALID_PARAM]
 * @param[in]        : [(ov2640_driver_t       *p_drv,
                        const ov2640_hw_ops_t *p_hw_ops,
                        ov2640_sensor_mode_t   sensor_mode]
 */
ov2640_state_t ov2640_driver_instruct(ov2640_driver_t       *p_drv,
                                      const ov2640_hw_ops_t *p_hw_ops,
                                      ov2640_sensor_mode_t   sensor_mode)
{
    if(NULL == p_drv || NULL == p_hw_ops)
    {
        return OV2640_INVALID_PARAM;
    }

    p_drv->p_hw_ops    = p_hw_ops;
    p_drv->sensor_mode = sensor_mode;
    // open the power and wait for 10ms
    p_hw_ops->pf_power_ctrl(OV2640_POWER_ON);
    p_hw_ops->pf_delay_ms(10U);
    const uint8_t (*cfg)[2];
    uint32_t dsp_w;
    uint32_t dsp_h;

    if(OV2640_MODE_CIF == sensor_mode)
    {
        cfg   = ov2640_svga_cfg;
        dsp_w = OV2640_SVGA_DSP_W;
        dsp_h = OV2640_SVGA_DSP_H;
    }
    else
    {
        cfg   = ov2640_svga_cfg;
        dsp_w = OV2640_SVGA_DSP_W;
        dsp_h = OV2640_SVGA_DSP_H;
    }

    /* 2. 初始软件复位 */
    p_hw_ops->pf_write_reg(0xFFU, 0x01U);
    p_hw_ops->pf_write_reg(0x12U, 0x80U);
    p_hw_ops->pf_delay_ms(10U);

    /* 3. 写入配置表（表内含第二次复位） */
    if(OV2640_OK != ov2640_apply_config(p_drv, cfg))
    {
        return OV2640_ERROR;
    }
    p_hw_ops->pf_delay_ms(10U);

    /* 4. 设置 DSP 输出尺寸 */
    if(OV2640_OK !=
       ov2640_drv_set_out_size(p_drv, (uint16_t)dsp_w, (uint16_t)dsp_h))
    {
        return OV2640_ERROR;
    }

    /* 5. 配置 DCMI 裁剪窗口：从 DSP 输出截取 OUT_W × OUT_H */
    uint32_t x0   = dsp_w - OV2640_OUT_W;
    uint32_t y0   = (dsp_h - OV2640_OUT_H) / 2U - 1U;
    uint32_t xcnt = OV2640_OUT_W * 2U - 1U; /* RGB565：每像素 2 字节 */
    uint32_t ycnt = OV2640_OUT_H - 1U;
    if(p_hw_ops->pf_config_crop(x0, y0, xcnt, ycnt) != OV2640_OK)
    {
        return OV2640_ERROR;
    }

    /* 6. 绑定驱动函数指针 */
    p_drv->pf_start        = ov2640_drv_start;
    p_drv->pf_stop         = ov2640_drv_stop;
    p_drv->pf_set_out_size = ov2640_drv_set_out_size;
    p_drv->is_init         = OV2640_DRIVER_IS_INIT;

    return OV2640_OK;
}

/* end of file --------------------------------------------------------------*/
