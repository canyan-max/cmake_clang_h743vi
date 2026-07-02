/**
 ******************************************************************************
 *@file               :   bsp_drv_ov2640.h
 *
 *@pardependencies    :   bsp_drv_ov2640.h
 *
 *@brief              :   OV2640 CMOS camera driver interface.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef BSP_DRV_OV2640_H
#define BSP_DRV_OV2640_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>

/* define -------------------------------------------------------------------*/
#define OV2640_DRIVER_IS_INIT   (0x01U)
#define OV2640_DRIVER_NOT_INIT  (0x00U)

#define OV2640_I2C_ADDR         (0x60U)
#define OV2640_I2C_TIMEOUT      (100U)

/* SVGA mode: sensor 800×600 → DSP 400×300 → DCMI crop 240×240, ~29fps */
#define OV2640_SVGA_DSP_W       (400U)
#define OV2640_SVGA_DSP_H       (300U)
/* CIF mode: sensor 400×296 → DSP 400×296 → DCMI crop 240×240, ~60fps */
#define OV2640_CIF_DSP_W        (400U)
#define OV2640_CIF_DSP_H        (296U)
/* Final DCMI-cropped output, must match LCD resolution */
#define OV2640_OUT_W            (240U)
#define OV2640_OUT_H            (240U)
/* DMA transfer length in 32-bit words: OUT_W * OUT_H * 2 bytes / 4 */
#define OV2640_DMA_LEN_WORDS    (OV2640_OUT_W * OV2640_OUT_H * 2U / 4U)

/* typedef ------------------------------------------------------------------*/
typedef struct OV2640_DRIVER_T ov2640_driver_t;

typedef enum OV2640_SENSOR_MODE_T
{
    OV2640_MODE_SVGA = 0x00U, /* 800×600 sensor, ~29fps */
    OV2640_MODE_CIF  = 0x01U, /* 400×296 sensor, ~60fps */
} ov2640_sensor_mode_t;

typedef enum OV2640_STATE_T
{
    OV2640_OK            = 0x00U,
    OV2640_ERROR         = 0x01U,
    OV2640_BUSY          = 0x02U,
    OV2640_TIMEOUT       = 0x03U,
    OV2640_INVALID_PARAM = 0x04U,
} ov2640_state_t;

typedef enum OV2640_DCMI_MODE_T
{
    OV2640_DCMI_CONTINUOUS = 0x00U, /* Continuous frame capture (preview) */
    OV2640_DCMI_SNAPSHOT   = 0x01U, /* Single frame capture (snapshot) */
} ov2640_dcmi_mode_t;

typedef struct OV2640_HW_OPS_T
{
/* Write one register via SCCB/I2C */
    ov2640_state_t (*pf_write_reg)(uint8_t reg, uint8_t val);
    /* Read one register via SCCB/I2C */
    ov2640_state_t (*pf_read_reg)(uint8_t reg, uint8_t *p_val);
    /* Start DCMI DMA capture: CONTINUOUS for preview, SNAPSHOT for single frame */
    ov2640_state_t (*pf_dcmi_start_dma)(uint32_t           *p_buf,
                                        uint32_t            len_words,
                                        ov2640_dcmi_mode_t  mode);
    /* Stop DCMI DMA */
    ov2640_state_t (*pf_dcmi_stop)(void);
    /* Configure and enable DCMI crop window */
    ov2640_state_t (*pf_config_crop)(uint32_t x0,
                                     uint32_t y0,
                                     uint32_t xcnt,
                                     uint32_t ycnt);
    /* Millisecond delay */
    void (*pf_delay_ms)(uint32_t ms);
} ov2640_hw_ops_t;

typedef struct OV2640_DRIVER_T
{
    const ov2640_hw_ops_t *p_hw_ops;
    ov2640_sensor_mode_t   sensor_mode; /* set before calling ov2640_driver_instruct */
    ov2640_state_t (*pf_start)(ov2640_driver_t *p_drv,
                               uint32_t        *p_buf,
                               uint32_t         len_words);
    ov2640_state_t (*pf_stop)(ov2640_driver_t *p_drv);
    /* Set DSP output size; call before pf_start to change resolution */
    ov2640_state_t (*pf_set_out_size)(ov2640_driver_t *p_drv,
                                      uint16_t         w,
                                      uint16_t         h);
    uint8_t is_init;
} ov2640_driver_t;

/* functions ----------------------------------------------------------------*/
/**
  * @brief            : [ov2640_driver_instruct]
                        Reset and configure the OV2640 sensor, set DSP output
                        size, configure the DCMI crop window, then bind all
                        driver function pointers. Does NOT start DMA.
  * @retval           : [OV2640_OK / OV2640_ERROR / OV2640_INVALID_PARAM]
  * @param[in]        : [ov2640_driver_t *p_drv, const ov2640_hw_ops_t *p_hw_ops]
  */
ov2640_state_t ov2640_driver_instruct(ov2640_driver_t       *p_drv,
                                      const ov2640_hw_ops_t *p_hw_ops);

#endif /* BSP_DRV_OV2640_H */
