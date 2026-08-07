/**
 ******************************************************************************
 *@file               :   camera_intf.h
 *@brief              :   Camera interface contract between device layer and BSP.
 *                        Device layer depends only on this header.
 *                        Chip-specific BSP implements g_camera_bsp_ops.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef CAMERA_INTF_H
#define CAMERA_INTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "plat_error.h"

/* ---- camera output contract ---------------------------------------------- */
#define BSP_CAMERA_OUT_W    (240U)
#define BSP_CAMERA_OUT_H    (240U)
#define BSP_CAMERA_BUF_SIZE (BSP_CAMERA_OUT_W * BSP_CAMERA_OUT_H * 2U)

/* ---- camera interface ops ------------------------------------------------ */
typedef struct BSP_CAMERA_OPS_T
{
    platform_err_t (*pf_init)(void);
    platform_err_t (*pf_start)(uint32_t *p_buf, uint32_t len_words);
    platform_err_t (*pf_stop)(void);
} bsp_camera_ops_t;

extern const bsp_camera_ops_t g_camera_bsp_ops;

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_INTF_H */
