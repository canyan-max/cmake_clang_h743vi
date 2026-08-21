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
#include "board_config.h"

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    GPIO_TypeDef *p_port;
    uint16_t      pin;
} gpio_resource_t;

/* variables ----------------------------------------------------------------*/
static const gpio_resource_t gpio_resource_table[PLAT_GPIO_ID_NUM] = {
    [PLAT_GPIO_ID_LED1] = { BOARD_LED1_PORT, BOARD_LED1_PIN },
    [PLAT_GPIO_ID_LED2] = { BOARD_LED2_PORT, BOARD_LED2_PIN },
    [PLAT_GPIO_ID_KEY1] = { BOARD_KEY1_PORT, BOARD_KEY1_PIN },
    [PLAT_GPIO_ID_KEY2] = { BOARD_KEY2_PORT, BOARD_KEY2_PIN },
    [PLAT_GPIO_ID_CAMERA_PWDN] = { BOARD_CAMERA_PWDN_PORT,
                                  BOARD_CAMERA_PWDN_PIN },
};

/* Private  functions  ------------------------------------------------------*/

static const gpio_resource_t *plat_gpio_get_resource(plat_gpio_id_t id)
{
    if(id >= PLAT_GPIO_ID_NUM)
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
