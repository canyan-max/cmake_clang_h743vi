/**
 ******************************************************************************
 *@file               :   service_display.h
 *@brief              :   Provide the display service api
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef SERVICE_DISPLAY_H
#define SERVICE_DISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
void service_display_init(void);
void service_display_fill_screen(uint16_t color);
void service_display_show_frame(
    const uint8_t *p_pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_DISPLAY_H */
