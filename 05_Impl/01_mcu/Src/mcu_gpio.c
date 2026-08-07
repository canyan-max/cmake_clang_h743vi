/**
 ******************************************************************************
 *@file               :   mcu_gpio.c
 *@brief              :   STM32 implementation of plat_gpio.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "plat_gpio.h"
#include "gpio.h"

/* exported functions -------------------------------------------------------*/

void plat_gpio_write(void *p_port, uint16_t pin, uint8_t level)
{
    GPIO_PinState state = (0U != level) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin((GPIO_TypeDef *)p_port, pin, state);
}

uint8_t plat_gpio_read(void *p_port, uint16_t pin)
{
    return (uint8_t)HAL_GPIO_ReadPin((GPIO_TypeDef *)p_port, pin);
}

void plat_gpio_toggle(void *p_port, uint16_t pin)
{
    HAL_GPIO_TogglePin((GPIO_TypeDef *)p_port, pin);
}

/* end of file --------------------------------------------------------------*/
