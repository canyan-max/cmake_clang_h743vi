/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   led_adapter.h
 *
 *@pardependencies    :
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
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef LED_ADAPTER_H
#define LED_ADAPTER_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
uint8_t drv_led_on(void *p_drv);
uint8_t drv_led_off(void *p_drv);
uint8_t drv_led_blink(void *p_drv);
uint8_t drv_led_init(void *p_drv, void *drv_ops);
#endif /* LED_ADAPTER_H */
