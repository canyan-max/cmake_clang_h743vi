/**
 ******************************************************************************
 *@file               :   plat_gpio.h
 *@brief              :   MCU GPIO abstraction — write, read, toggle.
 *                        Only for board-level GPIO (LED, Key).
 *                        Complex peripherals (SPI/I2C) keep their own ops.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef PLAT_GPIO_H
#define PLAT_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void    plat_gpio_write(void *p_port, uint16_t pin, uint8_t level);
uint8_t plat_gpio_read(void *p_port, uint16_t pin);
void    plat_gpio_toggle(void *p_port, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_GPIO_H */
