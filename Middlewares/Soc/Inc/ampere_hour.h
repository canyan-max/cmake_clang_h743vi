/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *
 *@file               :   ampere_hour.h
 *
 *@pardependencies    :
 *                        ampere_hour.h
 *                        xxx.h
 *                        xxxx.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of .
 *
 *@Processingflow     :
 *
 *@calldirectly       :
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef AMPERE_HOUR_H
#define AMPERE_HOUR_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file */
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* function  ----------------------------------------------------------------*/
/**
 * @brief            :  [get_detal_capacity]
 * @retval           :  [float detal_cap]
 * @param[in]        :  [float current , float bat_cap_max , int16_t bat_temp]
 */
float get_detal_capacity(float current, float bat_cap_max, int16_t bat_temp);

/**
 * @brief            :  [get_cur_ratios]
 * @retval           :  [float detal_cap]
 * @param[in]        :  [float current ,float bat_cap_max]
 */

#endif
/*end of ampere_hour.h*/
