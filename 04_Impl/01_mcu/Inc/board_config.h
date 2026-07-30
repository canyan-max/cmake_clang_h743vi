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

/* AT24C02 EEPROM -----------------------------------------------------------*/
#define BOARD_EEPROM_MAX_BYTE_ADDR  (256U)
#define BOARD_EEPROM_PAGE_SIZE      (8U)
#define BOARD_EEPROM_ADR_SIZE       (1U)    /* AT24_MEMADD_SIZE_8BIT */
#define BOARD_EEPROM_DEV_ADDR       (0xA0U)

/* OV2640 camera ------------------------------------------------------------*/
#define BOARD_CAM_I2C_ADDR          (0x60U)
#define BOARD_CAM_I2C_TIMEOUT       (100U)

/* LED polarity -------------------------------------------------------------*/
#define BOARD_LED1_ON_LEVEL         (0x01U) /* LED_ON_LEVEL_HIGH */
#define BOARD_LED2_ON_LEVEL         (0x01U) /* LED_ON_LEVEL_HIGH */

#endif /* BOARD_CONFIG_H */
