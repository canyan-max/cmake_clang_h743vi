/**
 ******************************************************************************
 *@file               :   bsp_led.c
 *@brief              :   LED BSP — maps board LED id to GPIO pin + polarity.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "bsp_led.h"
#include "plat_gpio.h"
#include "board_config.h"

/* ---- pin table ----------------------------------------------------------- */
typedef struct
{
    void    *p_port;
    uint16_t pin;
    uint8_t  on_level;
} led_pin_t;

static const led_pin_t led_table[] = {
    { BOARD_LED1_PORT, BOARD_LED1_PIN, BOARD_LED1_ON_LEVEL },
    { BOARD_LED2_PORT, BOARD_LED2_PIN, BOARD_LED2_ON_LEVEL },
};

#define LED_COUNT  (sizeof(led_table) / sizeof(led_table[0]))

/* exported functions -------------------------------------------------------*/

void bsp_led_on(uint8_t id)
{
    if(id >= (uint8_t)LED_COUNT) { return; }
    plat_gpio_write(led_table[id].p_port, led_table[id].pin,
                    led_table[id].on_level);
}

void bsp_led_off(uint8_t id)
{
    if(id >= (uint8_t)LED_COUNT) { return; }
    plat_gpio_write(led_table[id].p_port, led_table[id].pin,
                    (0U == led_table[id].on_level) ? 1U : 0U);
}

void bsp_led_toggle(uint8_t id)
{
    if(id >= (uint8_t)LED_COUNT) { return; }
    plat_gpio_toggle(led_table[id].p_port, led_table[id].pin);
}

/* end of file --------------------------------------------------------------*/
