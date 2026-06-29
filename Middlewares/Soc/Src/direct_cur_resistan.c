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
#include <stddef.h>              /* stdint lib header file. */
#include "direct_cur_resistan.h" /* direct_cur_resistan lib header file. */
#include <math.h>
/* define   -----------------------------------------------------------------*/
#define DIR_CUR_RES_NOT_USE_FUNCTION      __attribute__((unused))
#define DIR_CUR_RES_NOT_USE_VARIABLE(x)   ((void)(x))

#define COEF_OF_CELL_R	        (1.1f)
#define CELL_VOLTAGE_NORM       (100.0f)
#define R_MIN_LIMIT_FACTOR      (0.4f)
#define R_MAX_LIMIT_FACTOR      (20.0f)
#define RESISTANCE_ARYY_SIZE    (10U)
/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static float    g_coef_cell_learn = 1;  // CoefCellLearn
static float    g_r_of_cellbase   = 1;  // RofCellBase
static uint16_t coef_cell_set     = 10; // CoefCellSet
static float    g_internal_r      = 1;  // mohm
// TempTableofR
static float g_temp_table_of_r[6] = {-20.0f, -10.0f, 0.0f, 10.0f, 25.0f, 55.0f};
// g_cell_table_of_r
static float g_cell_table_of_r[6] = {10.0f, 7.0f, 3.0f, 2.0f, 1.0f, 0.9f};
// CurTableof
static float g_cur_table_of_r[3] = {1, 2, 3};
// RofCurTable the cur ratio
static float g_r_of_cur_table[3] = {1, 1.1, 1.3};
// cur of resistance arry
uint8_t  g_resistance_indx_of_arry;
float    g_resistance_cur_arry[RESISTANCE_ARYY_SIZE];
uint16_t g_resistance_volt_arry[RESISTANCE_ARYY_SIZE];

/* private functions------------------------------------------------------*/
/**
 * @brief            :  [linear_interp]
                        though cur ratio clac internal resistance
 * @retval           :  [float internal resistance value ]
 * @param[in]        :  [float cur_ratio]
 */
static float
linear_interp(float x, const float x_table[], const float y_table[], uint8_t n)
{

    if(x <= x_table[0])
    {
        return y_table[0];
    }
    if(x >= x_table[n - 1])
    {
        return y_table[n - 1];
    }

    for(int i = 0; i < n - 1; i++)
    {
        if(x < x_table[i + 1])
        {
            float t = (x - x_table[i]) / (x_table[i + 1] - x_table[i]);
            return y_table[i] + t * (y_table[i + 1] - y_table[i]);
        }
    }
    return y_table[n - 1];
}
/**
 * @brief            :  [get_coef_r_cur]
                        though cur ratio clac internal resistance
 * @retval           :  [float internal resistance value ]
 * @param[in]        :  [float cur_ratio]
 */
static float get_coef_r_cur(float cur_ratio)
{
    uint8_t table_size = sizeof(g_cur_table_of_r) / sizeof(g_cur_table_of_r[0]);
    float   liner_value = linear_interp(cur_ratio, g_cur_table_of_r,
                                        g_r_of_cur_table, table_size);
    return liner_value;
}

static void update_resistance_arry(uint16_t v_min, float cur)
{

    if(g_resistance_indx_of_arry >= RESISTANCE_ARYY_SIZE)
    {
        g_resistance_indx_of_arry = 0x00U;
    }
    g_resistance_cur_arry[g_resistance_indx_of_arry]  = cur;
    g_resistance_volt_arry[g_resistance_indx_of_arry] = v_min;
    g_resistance_indx_of_arry += 1;
}

static uint8_t get_arry_cur_max_and_min_value(float  *p_table,
                                              uint8_t size,
                                              float  *max_value,
                                              float  *min_value)
{
    if(NULL == max_value || NULL == min_value || 0 == size || NULL == p_table)
    {
        return 1;
    }
    float cur_max = p_table[0], cur_min = p_table[0];
    for(uint8_t i = 1; i < size; i++)
    {
        // max
        if(p_table[i] > cur_max)
        {
            cur_max = p_table[i];
        }
        // min
        if(p_table[i] < cur_min)
        {
            cur_min = p_table[i];
        }
    }
    *max_value = cur_max;
    *min_value = cur_min;
    return 0;
}

static uint8_t get_arry_volt_max_and_min_value(uint16_t *p_table,
                                               uint8_t   size,
                                               uint16_t *max_value,
                                               uint16_t *min_value)
{
    if(NULL == max_value || NULL == min_value || 0 == size || NULL == p_table)
    {
        return 1;
    }
    uint16_t v_max = p_table[0], v_min = p_table[0];
    for(uint8_t i = 1; i < size; i++)
    {
        // max
        if(p_table[i] > v_max)
        {
            v_max = p_table[i];
        }
        // min
        if(p_table[i] < v_min)
        {
            v_min = p_table[i];
        }
    }
    *max_value = v_max;
    *min_value = v_min;
    return 0;
}

/* exported functions -------------------------------------------------------*/
/**
 * @brief            :  [update_bat_resistance]
                        get the battry internal resistance
 * @retval           :  [float internal resistance value ]
 * @param[in]        :  [int16_t t_avg ,float cur_ratio,uint16_t cap_normal]
 */
float update_bat_resistance(int16_t t_avg, float cur_a, uint16_t cap_normal)
{
    float   r_cell_base = 0.0f, r_coef = 0.0f, norm_factor = 0.0f, scale = 0.0f;
    float   r_final = 0.0f, low_limit = 0.0f, high_limit = 0.0f;
    float   cur_ratio       = (fabsf(cur_a)) / (cap_normal);
    uint8_t temp_table_size = sizeof(g_temp_table_of_r) /
                              sizeof(g_temp_table_of_r[0]);
    r_cell_base             = linear_interp((float)t_avg, g_temp_table_of_r,
                                            g_cell_table_of_r, temp_table_size);
    r_coef                  = get_coef_r_cur(cur_ratio);
    norm_factor             = CELL_VOLTAGE_NORM / (float)cap_normal;
    scale = r_cell_base * r_coef * norm_factor * COEF_OF_CELL_R *
            (coef_cell_set / 10.0f);

    r_final         = scale * g_coef_cell_learn;
    low_limit       = R_MIN_LIMIT_FACTOR * norm_factor;
    high_limit      = R_MAX_LIMIT_FACTOR * norm_factor;
    g_r_of_cellbase = scale;

    if(r_final > low_limit && r_final <= high_limit)
    {
        g_internal_r = r_final;
    }
    return g_internal_r;
}
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
                                   uint16_t cap_normal)
{
    float    cur_max = 0.0f, cur_min = 0.0f, cur_temp = 0.0f;
    float    leran_temp_factor = 1.0f;
    uint16_t volt_max = 0, volt_min = 0, volt_temp = 0;
    if(tempture > -20)
    {
        update_resistance_arry(voltage, cur);
        get_arry_cur_max_and_min_value(g_resistance_cur_arry,
                                       RESISTANCE_ARYY_SIZE, &cur_max,
                                       &cur_min);
        get_arry_volt_max_and_min_value(g_resistance_volt_arry,
                                        RESISTANCE_ARYY_SIZE, &volt_max,
                                        &volt_min);
    }

    cur_temp  = fabsf(cur_max - cur_min);
    volt_temp = volt_max - volt_min;
    // check the volt is normal
    if(volt_temp < 800U && volt_max > 1000U && volt_max < 5000U &&
       volt_min > 1000U && volt_min < 5000U)
    {
        if(cur_temp > (0.2 * cap_normal) && cur_temp < (5 * cap_normal) &&
           (cur_max == 0 || cur_min == 0)) //  && currContinueCount >= 5
        {
            leran_temp_factor = (volt_temp / cur_temp) / g_r_of_cellbase;

            if(leran_temp_factor > 1 && leran_temp_factor < 5)
            {
                g_coef_cell_learn = leran_temp_factor;
            }
            else
            {
                g_coef_cell_learn = 1.0f;
            }
        }
    }
    return g_coef_cell_learn;
}
/* end of  file-------------------------------------------------------------*/
