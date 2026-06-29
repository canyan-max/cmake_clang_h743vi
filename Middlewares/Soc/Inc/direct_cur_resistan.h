/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   direct_cur_resistan.h
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
#ifndef DIRECT_CUR_RESISTAN_H
#define DIRECT_CUR_RESISTAN_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
 * @brief            :  [update_bat_resistance]
                        get the battry internal resistance
 * @retval           :  [float internal resistance value ]
 * @param[in]        :  [int16_t t_avg ,float cur_ratio,uint16_t cap_normal]
 */
/**
 * @brief            :  [update_bat_resistance]
                        get the battry internal resistance
 * @retval           :  [float internal resistance value ]
 * @param[in]        :  [int16_t t_avg ,float cur_ratio,uint16_t cap_normal]
 */
float update_bat_resistance(int16_t t_avg, float cur_a, uint16_t cap_normal);

/**
 * @brief            :  [calculator_resistance_factor]
                        updata the g_coef_cell_learn value .
                        thought cur and volt
                        R = (vt1 - vt0 )/(curt1 - curt0)
 * @retval           :  [float internal resistance value ]
 * @param[in]        :  [int16_t t_avg ,float cur_ratio,uint16_t cap_normal]
 */
float calculator_resistance_factor(float    cur,
                                   int16_t  tempture,
                                   uint16_t voltage,
                                   uint16_t cap_normal);
#endif /* DIRECT_CUR_RESISTAN_H */
