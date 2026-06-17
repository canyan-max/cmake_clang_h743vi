/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   st_lcd_spi.h
 *
 *@pardependencies    :
 *                        st_lcd_spi.h
 *                        xxx.h
 *                        xxxx.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the ST7789 SPI platform ops declaration.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef ST_LCD_SPI_H
#define ST_LCD_SPI_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct ST7789_SPI_OPS_T st7789_spi_ops_t;

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
extern const st7789_spi_ops_t g_st7789_spi_ops;

/* functions ----------------------------------------------------------------*/

#endif /* ST_LCD_SPI_H */
