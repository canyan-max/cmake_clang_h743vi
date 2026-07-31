/**
 ******************************************************************************
 *@file               :   bsp_key.c
 *@brief              :   Key BSP — maps board key id to GPIO pin.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "bsp_key.h"
#include "plat_gpio.h"
#include "board_config.h"

/* ---- pin table ----------------------------------------------------------- */
typedef struct
{
    void    *p_port;
    uint16_t pin;
} key_pin_t;

static const key_pin_t key_table[] = {
    { BOARD_KEY1_PORT, BOARD_KEY1_PIN },
    { BOARD_KEY2_PORT, BOARD_KEY2_PIN },
};

#define KEY_COUNT  (sizeof(key_table) / sizeof(key_table[0]))

/* exported functions -------------------------------------------------------*/

/* returns 1 if key is pressed (active low), 0 otherwise */
uint8_t bsp_key_is_pressed(uint8_t id)
{
    if(id >= (uint8_t)KEY_COUNT) { return 0U; }
    return (0U == plat_gpio_read(key_table[id].p_port, key_table[id].pin))
               ? 1U : 0U;
}

/* end of file --------------------------------------------------------------*/
