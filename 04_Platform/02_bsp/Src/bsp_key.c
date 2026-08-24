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
#include "board_resources.h"

/* ---- pin table ----------------------------------------------------------- */
static const plat_gpio_id_t key_table[] = {
    BOARD_GPIO_KEY1,
    BOARD_GPIO_KEY2,
};

#define KEY_COUNT  (sizeof(key_table) / sizeof(key_table[0]))

/* exported functions -------------------------------------------------------*/

/* returns 1 if key is pressed (active low), 0 otherwise */
uint8_t bsp_key_is_pressed(uint8_t id)
{
    uint8_t level;

    if(id >= (uint8_t)KEY_COUNT)
    {
        return 0U;
    }
    if(PLATFORM_ERR_OK != plat_gpio_read(key_table[id], &level))
    {
        return 0U;
    }
    return (0U == level) ? 1U : 0U;
}

/* end of file --------------------------------------------------------------*/
