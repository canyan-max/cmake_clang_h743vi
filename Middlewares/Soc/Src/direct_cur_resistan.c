/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   direct_cur_resistan.c
 * 
 *@pardependencies    :   direct_cur_resistan.c
 *                        xxxx.h
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

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>                               /* stdint lib header file. */
#include "direct_cur_resistan.h"     /* direct_cur_resistan lib header file. */

/* define   -----------------------------------------------------------------*/
#define DIR_CUR_RES_NOT_USE_FUNCTION      __attribute__((unused))
#define DIR_CUR_RES_NOT_USE_VARIABLE(x)   ((void)(x))

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
float g_temp_table_of_r[6] = {-20, -10, 0, 10, 25, 55};
float g_cell_table_of_r[6] = {
    10, 7, 3, 2, 1, 0.9
};
float g_internal_r = 1;	//mohm

float g_cur_table_of_r [3] = {1, 2, 3}; // CurTableof
float g_r_of_cur_table[6] ={ // RofCurTable the cur ratio
    1, 1.1, 1.3
};
/* private  functions  ------------------------------------------------------*/
/**
 * @brief            :  [get_coef_r_cur]
                        though cur ratio clac internal resistance 
 * @retval           :  [float internal resistance value ]
 * @param[in]        :  [float cur_ratio]
 */
DIR_CUR_RES_NOT_USE_FUNCTION  static float get_coef_r_cur(float cur_ratio)
{
    float rcoefficient = 1;
    float currcoef = 0;
    currcoef = cur_ratio;
    if(currcoef <= g_cur_table_of_r[0])
    {
        rcoefficient = g_r_of_cur_table[0];
    }
    else if(currcoef <= g_cur_table_of_r[1])
    {
        rcoefficient = \
        g_r_of_cur_table[0] + ((currcoef - g_cur_table_of_r[0]) / \
        (g_cur_table_of_r[1] - g_cur_table_of_r[0])) * (g_r_of_cur_table[1] - g_r_of_cur_table[0]);
    }
    else if(currcoef <= g_cur_table_of_r[2])
    {
        rcoefficient = g_r_of_cur_table[1] + ((currcoef - g_cur_table_of_r[1]) / \
        (g_cur_table_of_r[2] - g_cur_table_of_r[1])) * (g_r_of_cur_table[2] - g_r_of_cur_table[1]);
    }
    else
    {
        rcoefficient = g_r_of_cur_table[2];
    }
    return rcoefficient;
}
/* exported functions -------------------------------------------------------*/

/* end of  file -------------------------------------------------------------*/
