/**
 ******************************************************************************
 *@file               :   board_config.h
 *@brief              :   Board-level hardware configuration constants.
 *                        Shared by BSP and STM32 MCU implementation.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "gpio.h"  /* LED1_GPIO_Port / KEY_1_GPIO_Port etc. */
#include "spi.h"   /* hspi4 */
#include "usart.h" /* hlpuart1 */

/* OV2640 camera ------------------------------------------------------------*/
#define BOARD_CAMERA_PWDN_PORT      DCMI_PWDN_GPIO_Port
#define BOARD_CAMERA_PWDN_PIN       DCMI_PWDN_Pin
#define BOARD_CAMERA_PWDN_ACTIVE_LEVEL  (1U)

/* ST7789 display ----------------------------------------------------------*/
#define BOARD_DISPLAY_WIDTH                (240U)
#define BOARD_DISPLAY_HEIGHT               (240U)
#define BOARD_DISPLAY_DC_PORT              SPI4_DC_GPIO_Port
#define BOARD_DISPLAY_DC_PIN               SPI4_DC_Pin
#define BOARD_DISPLAY_DC_COMMAND_LEVEL     (0U)
#define BOARD_DISPLAY_DC_DATA_LEVEL        (1U)
#define BOARD_DISPLAY_BACKLIGHT_PORT       BACK_LIGHT_GPIO_Port
#define BOARD_DISPLAY_BACKLIGHT_PIN        BACK_LIGHT_Pin
#define BOARD_DISPLAY_BACKLIGHT_ON_LEVEL   (1U)
#define BOARD_DISPLAY_BACKLIGHT_OFF_LEVEL  (0U)

/* EEPROM -------------------------------------------------------------------*/
#define BOARD_EEPROM_I2C_ADDRESS_7B (0x50U)

/* LED ----------------------------------------------------------------------*/
#define BOARD_LED1_PORT             LED1_GPIO_Port
#define BOARD_LED1_PIN              LED1_Pin
#define BOARD_LED1_ON_LEVEL         (0x01U)
#define BOARD_LED2_PORT             LED2_GPIO_Port
#define BOARD_LED2_PIN              LED2_Pin
#define BOARD_LED2_ON_LEVEL         (0x01U)

/* KEY ----------------------------------------------------------------------*/
#define BOARD_KEY1_PORT             KEY_1_GPIO_Port
#define BOARD_KEY1_PIN              KEY_1_Pin
#define BOARD_KEY2_PORT             KEY_2_GPIO_Port
#define BOARD_KEY2_PIN              KEY_2_Pin

/* HAL peripheral handles ---------------------------------------------------*/
#define BOARD_I2C0_HANDLE           hi2c1
#define BOARD_CAM_DCMI_HANDLE       hdcmi
#define BOARD_SPI0_HANDLE           hspi4
#define BOARD_UART_PROTO1_HANDLE    hlpuart1

#endif /* BOARD_CONFIG_H */
