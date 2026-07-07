/**
 ******************************************************************************
 *@file               :   led_adapter.h
 *
 *@brief              :   Provide the HAL APIs of description.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef LED_ADAPTER_H
#define LED_ADAPTER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/
#define DRV_LED_OK              (0U)
#define DRV_LED_ERR_PARAM       (1U)
#define DRV_LED_ERR_HW          (2U)

/* typedef ------------------------------------------------------------------*/

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
uint8_t drv_led_on(void *p_drv);
uint8_t drv_led_off(void *p_drv);
uint8_t drv_led_blink(void *p_drv);
uint8_t drv_led_init(void *p_drv, void *drv_ops);

#ifdef __cplusplus
}
#endif

#endif /* LED_ADAPTER_H */
