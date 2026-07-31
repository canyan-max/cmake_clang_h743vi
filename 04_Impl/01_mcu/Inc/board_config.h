/**
 ******************************************************************************
 *@file               :   board_config.h
 *@brief              :   Board-level hardware configuration constants.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

/* OV2640 camera ------------------------------------------------------------*/
#define BOARD_CAM_I2C_ADDR          (0x60U)
#define BOARD_CAM_I2C_TIMEOUT       (100U)

/* LED polarity -------------------------------------------------------------*/
#define BOARD_LED1_ON_LEVEL         (0x01U) /* LED_ON_LEVEL_HIGH */
#define BOARD_LED2_ON_LEVEL         (0x01U) /* LED_ON_LEVEL_HIGH */

/* HAL peripheral handles ---------------------------------------------------*/
#define BOARD_CAM_I2C_HANDLE        hi2c1
#define BOARD_CAM_DCMI_HANDLE       hdcmi
#define BOARD_EEPROM_I2C_HANDLE     hi2c1

#endif /* BOARD_CONFIG_H */
