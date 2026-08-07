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

void bsp_led_on(uint8_t id);
void bsp_led_off(uint8_t id);
void bsp_led_toggle(uint8_t id);

#ifdef __cplusplus
}
#endif

#endif /* BSP_LED_H */
