/**
 ******************************************************************************
 *@file               :   board_stm32h743_binding.h
 *@brief              :   STM32H743/CubeMX bindings for board resources.
 *@version            :   V1.0
 *@note               :   Private to the MCU implementation layer.
 ******************************************************************************
 */
#ifndef BOARD_STM32H743_BINDING_H
#define BOARD_STM32H743_BINDING_H

#include "dcmi.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"

/* GPIO bindings ------------------------------------------------------------*/
#define BOARD_CAMERA_PWDN_PORT       DCMI_PWDN_GPIO_Port
#define BOARD_CAMERA_PWDN_PIN        DCMI_PWDN_Pin
#define BOARD_DISPLAY_DC_PORT        SPI4_DC_GPIO_Port
#define BOARD_DISPLAY_DC_PIN         SPI4_DC_Pin
#define BOARD_DISPLAY_BACKLIGHT_PORT BACK_LIGHT_GPIO_Port
#define BOARD_DISPLAY_BACKLIGHT_PIN  BACK_LIGHT_Pin
#define BOARD_LED1_PORT              LED1_GPIO_Port
#define BOARD_LED1_PIN               LED1_Pin
#define BOARD_LED2_PORT              LED2_GPIO_Port
#define BOARD_LED2_PIN               LED2_Pin
#define BOARD_KEY1_PORT              KEY_1_GPIO_Port
#define BOARD_KEY1_PIN               KEY_1_Pin
#define BOARD_KEY2_PORT              KEY_2_GPIO_Port
#define BOARD_KEY2_PIN               KEY_2_Pin

/* HAL peripheral handles ---------------------------------------------------*/
#define BOARD_I2C_DEVICE_BUS_HANDLE hi2c1
#define BOARD_CAM_DCMI_HANDLE    hdcmi
#define BOARD_SPI0_HANDLE        hspi4
#define BOARD_UART_PROTOCOL1_HANDLE hlpuart1
#define BOARD_UART_RS485_1_HANDLE huart7
#define BOARD_UART_RS485_2_HANDLE huart8

#endif /* BOARD_STM32H743_BINDING_H */
