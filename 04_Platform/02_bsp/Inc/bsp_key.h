/**
 ******************************************************************************
 *@file               :   bsp_key.h
 *@brief              :   Key BSP — raw press state by board index.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_KEY_H
#define BSP_KEY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/* returns 1 if pressed, 0 if not pressed */
uint8_t bsp_key_is_pressed(uint8_t id);

/* returns the number of keys on this board */
uint8_t bsp_key_count(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_KEY_H */
