/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *
 *@file               :   ocv_calc.h
 * 
 *@pardependencies    :
 *                        ocv_calc.h
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
#ifndef OCV_CALC_H
#define OCV_CALC_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                                /* stdint lib header file */
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* function  ----------------------------------------------------------------*/

/**
 * @brief            :  [ocv_table_startup_soc]
 * @retval           :  [ovc_soc]
 * @param[in]        :  [float temp_v, int16_t temperao]
 */
float ocv_table_startup_soc(float temp_v, int16_t tempera);
/**
 * @brief            :  [ocv_table_calc_chg_soc]
 * @retval           :  [chg mode ovc_soc]
 * @param[in]        :  [float temp_v, int16_t temperao]
 */
float ocv_table_calc_chg_soc(float temp_v, int16_t tempera);

#endif
                    /*end of ocv_calc.h*/

