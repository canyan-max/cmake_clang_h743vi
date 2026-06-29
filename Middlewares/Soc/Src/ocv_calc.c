/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *
 *@file               :   ocv_calc.c
 *
 *@pardependencies    :
 *                        ocv_calc.c
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
#include <stddef.h>   /* stdint lib header file */
#include "ocv_calc.h" /* self lib header file */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

// soc_ocv_table==chg                 //100%-0%
// 100    95     90     85     80
// 75     70     65     60     55
// 50     45     40     35     30
// 25     20     15     10     5
// 0
const float SOC_OCV_N0TEMP_CHG[21] = {3.480, 3.457, 3.398, 3.390, 3.381, 3.374,
                                      3.367, 3.361, 3.355, 3.348, 3.342, 3.335,
                                      3.329, 3.325, 3.321, 3.316, 3.302, 3.279,
                                      3.253, 3.235, 2.958};
const float SOC_OCV_10TEMP_CHG[21] = {3.480, 3.457, 3.398, 3.390, 3.381, 3.374,
                                      3.367, 3.361, 3.355, 3.348, 3.342, 3.335,
                                      3.329, 3.325, 3.321, 3.316, 3.302, 3.279,
                                      3.253, 3.235, 2.958};
const float SOC_OCV_25TEMP_CHG[21] = {3.480, 3.457, 3.398, 3.390, 3.381, 3.374,
                                      3.367, 3.361, 3.355, 3.348, 3.342, 3.335,
                                      3.329, 3.325, 3.321, 3.316, 3.302, 3.279,
                                      3.253, 3.235, 2.958};
// soc_ocv_table==dsg             //100%-0%
// 100    95     90     85     80
// 75     70     65     60     55
// 50     45     40     35     30
// 25     20     15     10     5
// 0
const float SOC_OCV_N20TEMP[21] = {3.350, 3.295, 3.292, 3.288, 3.284, 3.281,
                                   3.277, 3.273, 3.266, 3.258, 3.249, 3.234,
                                   3.210, 3.184, 3.156, 3.126, 3.094, 3.061,
                                   3.025, 2.989, 2.850}; // 100%-0%
const float SOC_OCV_N10TEMP[21] = {3.350, 3.295, 3.292, 3.288, 3.284, 3.281,
                                   3.277, 3.273, 3.266, 3.258, 3.249, 3.240,
                                   3.232, 3.225, 3.218, 3.059, 3.017, 2.971,
                                   2.919, 2.833, 2.800};
const float SOC_OCV_N0TEMP[21]  = {3.365, 3.311, 3.307, 3.304, 3.300, 3.297,
                                   3.293, 3.289, 3.282, 3.274, 3.265, 3.256,
                                   3.248, 3.241, 3.234, 3.190, 3.132, 3.036,
                                   2.911, 2.836, 2.802};
const float SOC_OCV_10TEMP[21]  = {3.375, 3.315, 3.313, 3.310, 3.308, 3.304,
                                   3.300, 3.296, 3.289, 3.280, 3.273, 3.262,
                                   3.254, 3.248, 3.241, 3.185, 3.135, 3.042,
                                   2.907, 2.850, 2.806};
const float SOC_OCV_25TEMP[21]  = {3.370, 3.318, 3.315, 3.313, 3.310, 3.309,
                                   3.305, 3.301, 3.293, 3.284, 3.277, 3.267,
                                   3.259, 3.252, 3.240, 3.219, 3.186, 3.149,
                                   3.105, 3.025, 2.906};
const float SOC_OCV_40TEMP[21]  = {3.365, 3.323, 3.320, 3.319, 3.316, 3.312,
                                   3.309, 3.305, 3.297, 3.289, 3.283, 3.272,
                                   3.264, 3.256, 3.242, 3.222, 3.194, 3.156,
                                   3.111, 3.080, 2.913};
/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/

/**
 * @brief            :  [ocv_table_startup_soc]
 * @retval           :  [ovc_soc]
 * @param[in]        :  [float temp_v, int16_t temperao]
 */
float ocv_table_startup_soc(float temp_v, int16_t tempera)
{
    const float *ocv_table = NULL;

    // look for the right table though the temperature
    if(tempera <= -15)
        ocv_table = SOC_OCV_N20TEMP;
    else if(tempera <= -5)
        ocv_table = SOC_OCV_N10TEMP;
    else if(tempera <= 5)
        ocv_table = SOC_OCV_N0TEMP;
    else if(tempera <= 20)
        ocv_table = SOC_OCV_10TEMP;
    else if(tempera <= 35)
        ocv_table = SOC_OCV_25TEMP;
    else
        ocv_table = SOC_OCV_40TEMP;

    if(ocv_table == NULL)
    {
        return 0.0;
    }

    // 2. check the voltage value is in the table or not
    if(temp_v >= ocv_table[0])
        return 1.0; // 100%

    if(temp_v <= ocv_table[20])
        return 0.0; // 0%

    // 3. calculate the soc value by linear interpolation
    for(uint8_t i = 1; i < 21; i++)
    {
        if(temp_v > ocv_table[i])
        {
            float soc_low  = 1.0 - 0.05 * i;
            float soc_high = 1.0 - 0.05 * (i - 1);
            return soc_high + (soc_low - soc_high) *
                                  (temp_v - ocv_table[i - 1]) /
                                  (ocv_table[i] - ocv_table[i - 1] + 0.0001f);
        }
    }

    return 0.0; // default return 0%
}
/**
 * @brief            :  [ocv_table_calc_chg_soc]
 * @retval           :  [chg mode ovc_soc]
 * @param[in]        :  [float temp_v, int16_t temperao]
 */
float ocv_table_calc_chg_soc(float temp_v, int16_t tempera)
{
    const float *ocv_table = NULL;

    // look for the right table though the temperature
    if(tempera <= 5)
        ocv_table = SOC_OCV_N0TEMP_CHG;
    else if(tempera <= 15)
        ocv_table = SOC_OCV_10TEMP_CHG;
    else
        ocv_table = SOC_OCV_25TEMP_CHG;

    if(ocv_table == NULL)
    {
        return 0.0;
    }

    // 2. check the voltage value is in the table or not
    if(temp_v >= ocv_table[0])
        return 1.0; // 100%

    if(temp_v <= ocv_table[20])
        return 0.0; // 0%

    // 3. calculate the soc value by linear interpolation
    for(uint8_t i = 1; i < 21; i++)
    {
        if(temp_v > ocv_table[i])
        {
            float soc_low  = 1.0 - 0.05 * i;
            float soc_high = 1.0 - 0.05 * (i - 1);
            return soc_high + (soc_low - soc_high) *
                                  (temp_v - ocv_table[i - 1]) /
                                  (ocv_table[i] - ocv_table[i - 1] + 0.0001);
        }
    }

    return 0.0; // default return 0%
}

/* end of file --------------------------------------------------------------*/
