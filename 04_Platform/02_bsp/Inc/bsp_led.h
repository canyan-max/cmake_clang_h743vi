/**
 ******************************************************************************
 *@file               :   bsp_led.h
 *@brief              :   LED BSP — on / off / toggle by board index.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_LED_H
#define BSP_LED_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef enum BSP_LED_ID_T
{
    BSP_LED_1 = 0U,
    BSP_LED_2,
    BSP_LED_NUM,
} bsp_led_id_t;

void bsp_led_on(bsp_led_id_t id);
void bsp_led_off(bsp_led_id_t id);
void bsp_led_toggle(bsp_led_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* BSP_LED_H */
