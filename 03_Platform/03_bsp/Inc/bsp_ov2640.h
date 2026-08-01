/**
 ******************************************************************************
 *@file               :   bsp_ov2640.h
 *@brief              :   OV2640 driver 鈥?protocol functions + hw ops transport.
 *                        Transport is the ONLY replaceable part (I2C/DCMI/power/delay).
 *                        Protocol functions are direct calls, not function pointers.
 *@version            :   V2.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_DRV_OV2640_H
#define BSP_DRV_OV2640_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

/* ---- defines ------------------------------------------------------------- */
#define OV2640_DRIVER_IS_INIT   (0x01U)
#define OV2640_DRIVER_NOT_INIT  (0x00U)

#define OV2640_SVGA_DSP_W       (400U)
#define OV2640_SVGA_DSP_H       (300U)
#define OV2640_CIF_DSP_W        (400U)
#define OV2640_CIF_DSP_H        (296U)
#define OV2640_OUT_W            (240U)
#define OV2640_OUT_H            (240U)
#define OV2640_DMA_LEN_WORDS    (OV2640_OUT_W * OV2640_OUT_H * 2U / 4U)
#define OV2640_POWER_ON         (0x00U)
#define OV2640_POWER_OFF        (0x01U)

/* ---- forward decalarations ----------------------------------------------- */
typedef struct OV2640_DEV_T     ov2640_dev_t;

/* ---- state enum ---------------------------------------------------------- */
typedef enum
{
    OV2640_OK            = 0x00U,
    OV2640_ERROR         = 0x01U,
    OV2640_BUSY          = 0x02U,
    OV2640_TIMEOUT       = 0x03U,
    OV2640_INVALID_PARAM = 0x04U,
} ov2640_state_t;

/* 预留：不同输出分辨率的传感器模式，配置表后续按需填充 */
typedef enum
{
    OV2640_MODE_SVGA = 0x00U,
    OV2640_MODE_CIF  = 0x01U,
} ov2640_sensor_mode_t;

typedef enum
{
    OV2640_DCMI_CONTINUOUS = 0x00U,
    OV2640_DCMI_SNAPSHOT   = 0x01U,
} ov2640_dcmi_mode_t;

/* ---- transport (the only replaceable part) ------------------------------- */
typedef struct OV2640_HW_OPS_T
{
    ov2640_state_t (*pf_write_reg)(uint8_t reg, uint8_t val);
    ov2640_state_t (*pf_read_reg)(uint8_t reg, uint8_t *p_val);
    ov2640_state_t (*pf_dcmi_start_dma)(uint32_t *p_buf, uint32_t len_words,
                                         ov2640_dcmi_mode_t mode);
    ov2640_state_t (*pf_dcmi_stop)(void);
    ov2640_state_t (*pf_config_crop)(uint32_t x0, uint32_t y0,
                                      uint32_t xcnt, uint32_t ycnt);
    void (*pf_power_ctrl)(uint8_t state);
    void (*pf_delay_ms)(uint32_t ms);
} ov2640_hw_ops_t;

/* ---- hw ops instance (provided by MCU impl) ------------------------------ */
extern const ov2640_hw_ops_t g_ov2640_hw_ops;

/* ---- device instance ----------------------------------------------------- */
struct OV2640_DEV_T
{
    const ov2640_hw_ops_t *p_hw_ops;
    uint8_t                 is_init;
    ov2640_sensor_mode_t    sensor_mode;
};

/* ---- protocol functions (direct calls, no ops indirection) --------------- */
ov2640_state_t ov2640_start(ov2640_dev_t *p_dev, uint32_t *p_buf, uint32_t len_words);
ov2640_state_t ov2640_stop(ov2640_dev_t *p_dev);
ov2640_state_t ov2640_set_out_size(ov2640_dev_t *p_dev, uint16_t w, uint16_t h);

/* ---- board-level init (wires Impl transport + sensor config) ------------- */
ov2640_state_t bsp_ov2640_init(ov2640_dev_t *p_dev, ov2640_sensor_mode_t sensor_mode);

#ifdef __cplusplus
}
#endif

#endif /* BSP_DRV_OV2640_H */
