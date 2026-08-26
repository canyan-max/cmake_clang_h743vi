/**
 ******************************************************************************
 *@file               :   bsp_key.c
 *@brief              :   Key BSP — maps board key id to GPIO pin.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "bsp_key.h"
#include "plat_gpio.h"
#include "board_resources.h"

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    plat_gpio_id_t gpio_id;
    uint8_t        active_level;
} bsp_key_config_t;

/* variables ----------------------------------------------------------------*/
static const bsp_key_config_t key_table[] = {
    {BOARD_GPIO_KEY1, BOARD_KEY1_ACTIVE_LEVEL},
    {BOARD_GPIO_KEY2, BOARD_KEY2_ACTIVE_LEVEL},
};

#define KEY_COUNT  (sizeof(key_table) / sizeof(key_table[0]))

_Static_assert(BOARD_KEY1_ACTIVE_LEVEL <= 1U,
               "BOARD_KEY1_ACTIVE_LEVEL must be 0 or 1");
_Static_assert(BOARD_KEY2_ACTIVE_LEVEL <= 1U,
               "BOARD_KEY2_ACTIVE_LEVEL must be 0 or 1");

/* exported functions -------------------------------------------------------*/

uint8_t bsp_key_count(void)
{
    return (uint8_t)KEY_COUNT;
}

platform_err_t bsp_key_is_pressed(uint8_t id, uint8_t *p_pressed)
{
    uint8_t level;

    if((id >= (uint8_t)KEY_COUNT) || (NULL == p_pressed))
    {
        return PLATFORM_ERR_PARAM;
    }

    platform_err_t error = plat_gpio_read(key_table[id].gpio_id, &level);
    if(PLATFORM_ERR_OK != error)
    {
        return error;
    }

    *p_pressed = (level == key_table[id].active_level) ? 1U : 0U;
    return PLATFORM_ERR_OK;
}

/* end of file --------------------------------------------------------------*/
