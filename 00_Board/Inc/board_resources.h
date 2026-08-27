/**
 ******************************************************************************
 *@file               :   board_resources.h
 *@brief              :   MCU-independent board resources and fixed hardware
 *                        properties shared by BSP and MCU implementation.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BOARD_RESOURCES_H
#define BOARD_RESOURCES_H

#include <stdint.h>

/* Logical GPIO resources ---------------------------------------------------*/
typedef uint8_t board_gpio_resource_id_t;

#define BOARD_GPIO_LED1               ((board_gpio_resource_id_t)0U)
#define BOARD_GPIO_LED2               ((board_gpio_resource_id_t)1U)
#define BOARD_GPIO_KEY1               ((board_gpio_resource_id_t)2U)
#define BOARD_GPIO_KEY2               ((board_gpio_resource_id_t)3U)
#define BOARD_GPIO_CAMERA_PWDN        ((board_gpio_resource_id_t)4U)
#define BOARD_GPIO_DISPLAY_DC         ((board_gpio_resource_id_t)5U)
#define BOARD_GPIO_DISPLAY_BACKLIGHT  ((board_gpio_resource_id_t)6U)
#define BOARD_GPIO_RESOURCE_COUNT     ((board_gpio_resource_id_t)7U)
#define BOARD_GPIO_RESOURCE_NONE      ((board_gpio_resource_id_t)0xFFU)

/* Logical UART resources ---------------------------------------------------*/
typedef uint8_t board_uart_resource_id_t;

#define BOARD_UART_PROTOCOL_1       ((board_uart_resource_id_t)0U)
#define BOARD_UART_RS485_1          ((board_uart_resource_id_t)1U)
#define BOARD_UART_RS485_2          ((board_uart_resource_id_t)2U)
#define BOARD_UART_RESOURCE_COUNT   ((board_uart_resource_id_t)3U)

/* RS485 direction-control properties --------------------------------------*/
typedef enum
{
    BOARD_RS485_DIRECTION_AUTO = 0U,
    BOARD_RS485_DIRECTION_GPIO,
} board_rs485_direction_mode_t;

#define BOARD_RS485_1_DIRECTION_MODE      BOARD_RS485_DIRECTION_AUTO
#define BOARD_RS485_1_DIRECTION_GPIO      BOARD_GPIO_RESOURCE_NONE
#define BOARD_RS485_1_DIRECTION_TX_LEVEL  (0U) /* unused in AUTO mode */
#define BOARD_RS485_1_DIRECTION_RX_LEVEL  (0U) /* unused in AUTO mode */

#define BOARD_RS485_2_DIRECTION_MODE      BOARD_RS485_DIRECTION_AUTO
#define BOARD_RS485_2_DIRECTION_GPIO      BOARD_GPIO_RESOURCE_NONE
#define BOARD_RS485_2_DIRECTION_TX_LEVEL  (0U) /* unused in AUTO mode */
#define BOARD_RS485_2_DIRECTION_RX_LEVEL  (0U) /* unused in AUTO mode */

/* Logical I2C resources ----------------------------------------------------*/
typedef uint8_t board_i2c_resource_id_t;

#define BOARD_I2C_DEVICE_BUS       ((board_i2c_resource_id_t)0U)
#define BOARD_I2C_CAMERA_BUS       BOARD_I2C_DEVICE_BUS
#define BOARD_I2C_EEPROM_BUS       BOARD_I2C_DEVICE_BUS
#define BOARD_I2C_RESOURCE_COUNT   ((board_i2c_resource_id_t)1U)

/* OV2640 camera ------------------------------------------------------------*/
#define BOARD_CAMERA_PWDN_ACTIVE_LEVEL  (1U)

/* ST7789 display -----------------------------------------------------------*/
#define BOARD_DISPLAY_WIDTH                (240U)
#define BOARD_DISPLAY_HEIGHT               (240U)
#define BOARD_DISPLAY_DC_COMMAND_LEVEL     (0U)
#define BOARD_DISPLAY_DC_DATA_LEVEL        (1U)
#define BOARD_DISPLAY_BACKLIGHT_ON_LEVEL   (1U)
#define BOARD_DISPLAY_BACKLIGHT_OFF_LEVEL  (0U)

/* EEPROM -------------------------------------------------------------------*/
#define BOARD_EEPROM_I2C_ADDRESS_7B  (0x50U)

/* LED ----------------------------------------------------------------------*/
#define BOARD_LED1_ON_LEVEL  (0x01U)
#define BOARD_LED2_ON_LEVEL  (0x01U)

/* Key ----------------------------------------------------------------------*/
#define BOARD_KEY1_ACTIVE_LEVEL  (0U)
#define BOARD_KEY2_ACTIVE_LEVEL  (0U)

#endif /* BOARD_RESOURCES_H */
