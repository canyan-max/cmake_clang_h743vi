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
#include "board_resources.h"

/* ---- pin table ----------------------------------------------------------- */
typedef struct
{
    plat_gpio_id_t gpio_id;
    uint8_t        on_level;
} led_pin_t;

static const led_pin_t led_table[] = {
    {BOARD_GPIO_LED1, BOARD_LED1_ON_LEVEL},
    {BOARD_GPIO_LED2, BOARD_LED2_ON_LEVEL},
};

#define LED_COUNT  (sizeof(led_table) / sizeof(led_table[0]))

_Static_assert(LED_COUNT == BSP_LED_NUM,
               "BSP LED table does not match ID enum");

/* exported functions -------------------------------------------------------*/

void bsp_led_on(bsp_led_id_t id)
{
    if(id >= BSP_LED_NUM)
    {
        return;
    }
    (void)plat_gpio_write(led_table[id].gpio_id, led_table[id].on_level);
}

void bsp_led_off(bsp_led_id_t id)
{
    if(id >= BSP_LED_NUM)
    {
        return;
    }
    (void)plat_gpio_write(led_table[id].gpio_id,
                          (0U == led_table[id].on_level) ? 1U : 0U);
}

void bsp_led_toggle(bsp_led_id_t id)
{
    if(id >= BSP_LED_NUM)
    {
        return;
    }
    (void)plat_gpio_toggle(led_table[id].gpio_id);
}

/* end of file --------------------------------------------------------------*/
