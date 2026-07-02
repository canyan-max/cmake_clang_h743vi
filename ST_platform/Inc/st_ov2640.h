/**
 ******************************************************************************
 *@file               :   st_ov2640.h
 *
 *@pardependencies    :   bsp_drv_ov2640.h
 *                        i2c.h
 *                        dcmi.h
 *
 *@brief              :   STM32H7 HAL implementation of ov2640_hw_ops_t.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef ST_OV2640_H
#define ST_OV2640_H

/* Includes -----------------------------------------------------------------*/
#include "bsp_drv_ov2640.h"

/* variables ----------------------------------------------------------------*/
extern const ov2640_hw_ops_t g_ov2640_hw_ops;

#endif /* ST_OV2640_H */
