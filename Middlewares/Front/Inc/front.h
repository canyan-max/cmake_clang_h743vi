/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   front.h
 *
 *@pardependencies    :
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of description.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef FRONT_H
#define FRONT_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/
// rgb 565 color  define
#define RGB565_BLACK  (0x0000U)
#define RGB565_WHITE  (0xFFFFU)
#define RGB565_RED    (0xF800U)
#define RGB565_GREEN  (0x07E0U)
#define RGB565_BLUE   (0x001FU)
#define RGB565_YELLOW (0xFFE0U)
/* typedef ------------------------------------------------------------------*/
typedef struct FRONT_DEF_T
{
    const uint8_t *p_data;     /* flat font byte table, column-major      */
    uint8_t        char_w;     /* pixel columns per character             */
    uint8_t        char_h;     /* pixel rows per character (multiple of 8)*/
    uint8_t        char_count; /* number of characters in table           */
} front_def_t;
/* exported types -----------------------------------------------------------*/
extern front_def_t g_f6x8;
extern front_def_t g_f8x16;
/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/

#endif /* FRONT_H */
