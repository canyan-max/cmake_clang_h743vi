/**
 ******************************************************************************
 *@file               :   service_display.h
 *
 *@brief              :   Display service — owns ST7789 handle and provides
 *                        application-level drawing interface.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef SERVICE_DISPLAY_H
#define SERVICE_DISPLAY_H

#include <stdint.h>

void service_display_init(void);
void service_display_fill_screen(uint16_t color);
void service_display_show_frame(const uint8_t *p_pixels,
                                 uint16_t       x,
                                 uint16_t       y,
                                 uint16_t       w,
                                 uint16_t       h);

#endif /* SERVICE_DISPLAY_H */
