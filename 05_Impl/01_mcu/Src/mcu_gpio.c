/**
 ******************************************************************************
 *@file               :   mcu_gpio.c
 *@brief              :   STM32 implementation of plat_gpio.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "plat_gpio.h"
#include "board_resources.h"
#include "board_stm32h743_binding.h"

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    GPIO_TypeDef *p_port;
    uint16_t      pin;
} gpio_resource_t;

/* variables ----------------------------------------------------------------*/
static const gpio_resource_t gpio_resource_table[BOARD_GPIO_RESOURCE_COUNT] = {
    [BOARD_GPIO_LED1]        = {BOARD_LED1_PORT, BOARD_LED1_PIN},
    [BOARD_GPIO_LED2]        = {BOARD_LED2_PORT, BOARD_LED2_PIN},
    [BOARD_GPIO_KEY1]        = {BOARD_KEY1_PORT, BOARD_KEY1_PIN},
    [BOARD_GPIO_KEY2]        = {BOARD_KEY2_PORT, BOARD_KEY2_PIN},
    [BOARD_GPIO_CAMERA_PWDN] = {BOARD_CAMERA_PWDN_PORT,
                                BOARD_CAMERA_PWDN_PIN},
    [BOARD_GPIO_DISPLAY_DC]  = {BOARD_DISPLAY_DC_PORT, BOARD_DISPLAY_DC_PIN},
    [BOARD_GPIO_DISPLAY_BACKLIGHT] = {BOARD_DISPLAY_BACKLIGHT_PORT,
                                      BOARD_DISPLAY_BACKLIGHT_PIN},
};

_Static_assert((sizeof(gpio_resource_table) /
                sizeof(gpio_resource_table[0])) == BOARD_GPIO_RESOURCE_COUNT,
               "Board GPIO resource table size mismatch");

/* Private  functions  ------------------------------------------------------*/

static const gpio_resource_t *plat_gpio_get_resource(plat_gpio_id_t id)
{
    if(id >= BOARD_GPIO_RESOURCE_COUNT)
    {
        return NULL;
    }
    return &gpio_resource_table[id];
}

/* Exported functions -------------------------------------------------------*/

platform_err_t plat_gpio_write(plat_gpio_id_t id, uint8_t level)
{
    const gpio_resource_t *p_resource = plat_gpio_get_resource(id);
    if(NULL == p_resource)
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_GPIO_WritePin(p_resource->p_port, p_resource->pin,
                      (0U != level) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return PLATFORM_ERR_OK;
}

platform_err_t plat_gpio_read(plat_gpio_id_t id, uint8_t *p_level)
{
    const gpio_resource_t *p_resource = plat_gpio_get_resource(id);
    if((NULL == p_resource) || (NULL == p_level))
    {
        return PLATFORM_ERR_PARAM;
    }

    *p_level = (uint8_t)HAL_GPIO_ReadPin(p_resource->p_port, p_resource->pin);
    return PLATFORM_ERR_OK;
}

platform_err_t plat_gpio_toggle(plat_gpio_id_t id)
{
    const gpio_resource_t *p_resource = plat_gpio_get_resource(id);
    if(NULL == p_resource)
    {
        return PLATFORM_ERR_PARAM;
    }

    HAL_GPIO_TogglePin(p_resource->p_port, p_resource->pin);
    return PLATFORM_ERR_OK;
}

/* end of file --------------------------------------------------------------*/
