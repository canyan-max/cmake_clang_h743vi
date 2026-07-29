/**
 ******************************************************************************
 *@file               :   st_lcd_spi.h
 *
 *@brief              :   Provide the HAL APIs of description.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef ST_LCD_SPI_H
#define ST_LCD_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct ST7789_SPI_OPS_T st7789_spi_ops_t;

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
extern const st7789_spi_ops_t g_st7789_spi_ops;

/* functions ----------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* ST_LCD_SPI_H */
