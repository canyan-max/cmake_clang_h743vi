/**
 ******************************************************************************
 *@file               :   st_iic.h
 *
 *@brief              :   STM32 I2C HAL ops instance declaration.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef ST_IIC_H
#define ST_IIC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stddef lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct IIC_OPS_T iic_ops_t;

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
extern const iic_ops_t g_at24c02_iic_ops;

/* functions ----------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* ST_IIC_H */
