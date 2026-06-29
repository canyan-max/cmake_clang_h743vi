/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *
 *@file               :   ampere_hour.c
 *
 *@pardependencies    :
 *                        ampere_hour.c
 *                        xxx.h
 *                        xxxx.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of description.
 *
 *@Processingflow     :
 *
 *@calldirectly       :
 *
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>       /* stdio lib header file */
#include <math.h>        /* math lib header file */
#include <stddef.h>      /* stdint lib header file */
#include "ampere_hour.h" /* self lib header file */
/* define   -----------------------------------------------------------------*/
#define SAMINTERVALTIME         (1000U)
#define TEMPER_TABLE_SIZE       (6U)
#define DSG_RATIO_TABLE_SIZE    (5U)
// #define ENABLE_DEBUG_MODE
/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
#ifdef ENABLE_DEBUG_MODE
float dsg_ratio      = 0; // mah
float avail_capacity = 0;
#endif // end of ENABLE_DEBUG_MODE
static const float TemperTable[TEMPER_TABLE_SIZE] = {-20, -10, 0, 10, 25, 55};
static const float DSGRatioTable[DSG_RATIO_TABLE_SIZE] = {0.5, 1, 2, 3, 4};
static const float AvailCapTable[TEMPER_TABLE_SIZE][DSG_RATIO_TABLE_SIZE] = {
    // 80  82  80  80
    {65, 68, 72, 78, 75},    // -20
    {85, 85, 81, 80, 75},    // -10
    {90, 90, 83, 83, 83},    // 0
    {98, 95, 90, 88, 88},    // 10
    {100, 98, 95, 90, 90},   // 25
    {100, 100, 100, 100, 98} // 55
};
/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/

/**
 * @brief            :  [calculate_real_dsgcapacity]
 * @retval           :  [float real_detal_availcapacity]
 * @param[in]        :  [float dsg_instcurrent, \
                         float availcapacity]
 */
static float calculate_real_dsgcapacity(float dsg_instcurrent,
                                        float availcapacity)
{
    float real_detal_availcapacity = 0;
    real_detal_availcapacity       = (dsg_instcurrent * SAMINTERVALTIME / 3600 /
                                      availcapacity) *
                                     100; // mAh
    return real_detal_availcapacity;
}
/**
 * @brief            :  [calculate_ranks]
 * @retval           :  [1-successful 0-err]
 * @param[in]        :  [float temp_value, float dsg_ratio, \
                        float *avaicap_value, float *temp_dsg_ratio]
 */
static uint8_t calculate_ranks(float  temp_value,
                               float  dsg_ratio,
                               float *avaicap_value,
                               float *temp_dsg_ratio)
{
    uint8_t      i  = 0;
    uint8_t      m1 = 0, m2 = 0, n1 = 0, n2 = 0;
    const float *temp_table      = TemperTable;
    const float *dsg_ratio_table = DSGRatioTable;
    if(avaicap_value == NULL || temp_dsg_ratio == NULL)
    {
        return 0;
    }
    // temperature ranks
    if(temp_value >= temp_table[TEMPER_TABLE_SIZE - 1])
    {
        m1 = TEMPER_TABLE_SIZE - 1;
        m2 = TEMPER_TABLE_SIZE - 1;
    }
    else if(temp_value <= temp_table[0])
    {
        m1 = 0;
        m2 = 0;
    }
    else
    {
        for(i = 0; i < TEMPER_TABLE_SIZE - 1; i++)
        {
            if(temp_value > temp_table[i] && temp_value < temp_table[i + 1])
            {
                m1 = i;
                m2 = i + 1;
                break;
            }
            else if(temp_value == temp_table[i])
            {
                m1 = i;
                m2 = i;
                break;
            }
        }
    }
    // dsg_ratio ranks
    if(dsg_ratio >= dsg_ratio_table[DSG_RATIO_TABLE_SIZE - 1])
    {
        n1 = DSG_RATIO_TABLE_SIZE - 1;
        n2 = DSG_RATIO_TABLE_SIZE - 1;
    }
    else if(dsg_ratio <= dsg_ratio_table[0])
    {
        n1 = 0;
        n2 = 0;
    }
    else
    {
        for(i = 0; i < DSG_RATIO_TABLE_SIZE - 1; i++)
        {
            if(dsg_ratio > dsg_ratio_table[i] &&
               dsg_ratio < dsg_ratio_table[i + 1])
            {
                n1 = i;
                n2 = i + 1;
                break;
            }
            else if(dsg_ratio == dsg_ratio_table[i])
            {
                n1 = i;
                n2 = i;
                break;
            }
        }
    }

    // available capacity value
    avaicap_value[0] = AvailCapTable[m1][n1];
    avaicap_value[1] = AvailCapTable[m1][n2];
    avaicap_value[2] = AvailCapTable[m2][n1];
    avaicap_value[3] = AvailCapTable[m2][n2];
    // temperature  value
    temp_dsg_ratio[0] = temp_table[m1];
    temp_dsg_ratio[1] = temp_table[m2];
    // dsg_ratio value
    temp_dsg_ratio[2] = dsg_ratio_table[n1];
    temp_dsg_ratio[3] = dsg_ratio_table[n2];
    for(uint8_t i = 0; i < 4; i++)
    {
        if(avaicap_value[i] == 0.0)
        {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief            :  [calculate_availcap]
 * @retval           :  [float cal_availcap]
 * @param[in]        :  [float temp_value, float dsg_ratio]
 */
static float calculate_availcap(float temp_value, float dsg_ratio)
{
    float cal_availcap = 0.0;

    float avai_cap_value[4]                 = {0, 0, 0, 0};
    float temp_dsg_ratio[4]                 = {0, 0, 0, 0};
    float low_dsg_ratio_low_temp_avai_cap   = 0.0;
    float high_dsg_ratio_low_temp_avai_cap  = 0.0;
    float low_dsg_ratio_high_temp_avai_cap  = 0.0;
    float high_dsg_ratio_high_temp_avai_cap = 0.0;

    float high_temp          = 0.0;
    float low_temp           = 0.0;
    float high_dsg_ratio     = 0.0;
    float low_dsg_ratio      = 0.0;
    float low_temp_avai_cap  = 0.0;
    float high_temp_avai_cap = 0.0;
    if(0U ==
       calculate_ranks(temp_value, dsg_ratio, avai_cap_value, temp_dsg_ratio))
    {
        return 0.0;
    }
    // 0-1 low temperature capcity
    low_dsg_ratio_low_temp_avai_cap  = avai_cap_value[0];
    high_dsg_ratio_low_temp_avai_cap = avai_cap_value[1];
    // 2-3 high temperature capcity
    low_dsg_ratio_high_temp_avai_cap  = avai_cap_value[2];
    high_dsg_ratio_high_temp_avai_cap = avai_cap_value[3];

    // 0-1 temperature  value
    low_temp  = temp_dsg_ratio[0];
    high_temp = temp_dsg_ratio[1];
    // 2-3 discharge ratio value
    low_dsg_ratio  = temp_dsg_ratio[2];
    high_dsg_ratio = temp_dsg_ratio[3];

    // insert the linear interpolation

    low_temp_avai_cap = (high_dsg_ratio_low_temp_avai_cap -
                         low_dsg_ratio_low_temp_avai_cap) /
                            (high_dsg_ratio - low_dsg_ratio + 0.0001f) *
                            (dsg_ratio - low_dsg_ratio) +
                        low_dsg_ratio_low_temp_avai_cap;

    high_temp_avai_cap = (high_dsg_ratio_high_temp_avai_cap -
                          low_dsg_ratio_high_temp_avai_cap) /
                             (high_dsg_ratio - low_dsg_ratio + 0.0001f) *
                             (dsg_ratio - low_dsg_ratio) +
                         low_dsg_ratio_high_temp_avai_cap;
    // insert the linear interpolation
    cal_availcap = (high_temp_avai_cap - low_temp_avai_cap) /
                       (high_temp - low_temp) * (temp_value - low_temp) +
                   low_temp_avai_cap;
    //    cal_availcap = fabs( cal_availcap*1.0 );
    return cal_availcap;
}

/**
 * @brief            :  [get_detal_capacity]
 * @retval           :  [float detal_cap]
 * @param[in]        :  [float current , float bat_cap_max , int16_t bat_temp]
 */
float get_detal_capacity(float current, float bat_cap_max, int16_t bat_temp)
{
    float detal_cap = 0;
#ifndef ENABLE_DEBUG_MODE
    float dsg_ratio      = 0;
    float avail_capacity = 0;
#endif // end of ENABLE_DEBUG_MODE

    // calculate discharge ratio
    dsg_ratio = (fabsf(current)) / (bat_cap_max); // mah
    // calculate temperature compensation
    avail_capacity = calculate_availcap(bat_temp, dsg_ratio); // 0-100
    // calculate temperature compensation  of the battery avilice capacity
    detal_cap = calculate_real_dsgcapacity((float)(current / 1000.0f),
                                           avail_capacity);
    return detal_cap;
}

/* end of file --------------------------------------------------------------*/
