/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *
 *@file               :   soc.c
 * 
 *@pardependencies    :
 *                        soc.c
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
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>                                /* stdint lib header file */
#include "soc.h"                                     /* self lib header file */
#include <math.h>
#include "ocv_calc.h"
#include "ampere_hour.h"
/* define   -----------------------------------------------------------------*/
#define CAPS_LOST           (g_normal_cap*10)
#define CAPS_REATED         (g_normal_cap*1000)
/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
// capacity 
// soc = g_remain_capacity / g_full_max_capacity
float    g_remain_capacity       = 0.0f; // 
float    g_full_max_capacity     = 0.0f; // RealCap_CHG
float    g_full_real_capacity    = 0.0f; // CellRealCap_CHG
float    g_current_soc           = 0.0f; // current soc 0.0 ~ 1.0 
float    g_init_soc              = 0.0f; // init soc 0 -100
float    g_soc                   = 0.0f; // soc 0 ~ 100 
uint16_t g_soh                   = 1000;
float    g_dsg_capcity           = 0;
uint16_t g_cycle_count           = 0;// cycle count
uint16_t g_normal_cap            = 0;// NORMAL CAP 
uint16_t g_old_normal_cap        = 0;// NORMAL CAP 
uint8_t  g_empty_flg             = 0;
uint16_t g_cells_full_volt       = 3600; // mv
uint16_t g_cells_empty_volt      = 0; // mv
uint16_t g_volt_max_diff         = 1000; // mv
//static float    g_remain_capacity       = 0.0f; // 
//static float    g_full_max_capacity     = 0.0f; // RealCap_CHG
//static float    g_full_real_capacity    = 0.0f; // CellRealCap_CHG
//static float    g_current_soc           = 0.0f; // current soc 0.0 ~ 1.0 
//static float    g_init_soc              = 0.0f; // init soc 0 -100
//static float    g_soc                   = 0.0f;      // soc 0 ~ 100 
//static uint16_t g_soh                   = 1000;
//static float    g_dsg_capcity           = 0;
//static uint16_t g_cycle_count           = 0;// cycle count
//static uint16_t g_normal_cap            = 0;// NORMAL CAP 
//static uint16_t g_old_normal_cap        = 0;// NORMAL CAP 
//static uint8_t  g_empty_flg             = 0;

/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
/**
 * @brief            :  [check_bat_status]
 * @retval           :  [0 - idle 1 - charge 2 - discharge]
 * @param[in]        :  [float temp_i]
 */
static uint8_t check_bat_status (float temp_i)
{
    static uint8_t bat_chg_status_cnt = 0 , bat_dischg_status_cnt = 0;
    if( temp_i == 0.0f)
    {
        bat_chg_status_cnt = 0;
        bat_dischg_status_cnt = 0;
        return 0;
    }
    else
    {
        if(temp_i > 0.0f)
        {
            bat_chg_status_cnt+=1;
            if(bat_chg_status_cnt >= 3)
            {
                bat_chg_status_cnt = 4;
                return 1;
            }
        }
        else 
        {
            bat_chg_status_cnt = 0;
        }
        
        if(temp_i < 0.0f)
        {
            bat_dischg_status_cnt+=1;
            if(bat_dischg_status_cnt >= 3)
            {
                bat_dischg_status_cnt = 4;
                return 2;
            }
        }
        else 
        {
            bat_dischg_status_cnt = 0;
        }
    }
    return 0;
}


/**
 * @brief            :  [get_cap_lost_tatol]
 * @retval           :  [float cap_lost_tatols]
 * @param[in]        :  [uint16_t Cyc_DSG]
 */
static float get_cap_lost_tatol(uint16_t Cyc_DSG)
{
    float cap_lost_tatols = 0.0f;
    cap_lost_tatols = (Cyc_DSG / 50.0) * CAPS_LOST;		//mAh
    return cap_lost_tatols;
}
/**
 * @brief            :  [get_real_full_cap]
 * @retval           :  [ float read_caps_chg]
 * @param[in]        :  [float cap_lost_tatol]
 */
static float get_real_full_cap(float cap_lost_tatol)
{
    float read_caps_chg=0.0f;
    read_caps_chg = CAPS_REATED - cap_lost_tatol;		//mAh
    return read_caps_chg;
}
/**
 * @brief            :  [check_caps_change]
 */
static void check_caps_change(void)
{
    if(g_old_normal_cap!=g_normal_cap)
    {
        g_old_normal_cap=g_normal_cap;
        float cap_lost_tatol = get_cap_lost_tatol(g_cycle_count);
        g_full_max_capacity  = get_real_full_cap(cap_lost_tatol);
        g_remain_capacity    = g_current_soc*g_full_max_capacity;
        g_full_real_capacity = g_full_max_capacity;
    }
}

/**
 * @brief            :  [cycle_count_calculate]
 * @param[in]        :  [float dsg_caps]
 */
static void cycle_count_calculate(float dsg_caps)
{
    g_dsg_capcity+=dsg_caps;
    float avg_dsg_caps = fabsf(g_dsg_capcity);
    float caps_lost    = 0;
    if(avg_dsg_caps > g_full_max_capacity*0.9f)
    {
        g_dsg_capcity = 0;
        g_cycle_count+=1;
        caps_lost = get_cap_lost_tatol(g_cycle_count);
        g_soh     = (1-(caps_lost / CAPS_REATED))*1000;
        g_full_max_capacity = get_real_full_cap(caps_lost);
        g_full_real_capacity = g_full_max_capacity;
    }
}
/**
 * @brief            :  [compare_capcity]
 * @retval           :  [0 - dont updata caps 1 - updata caps]
 * @param[in]        :  [float caps_new]
 */
static uint8_t compare_capcity(float caps_new)
{
    float result = 0;
    result = caps_new  / g_full_max_capacity;
    if(result>=0.95f && result<=1.0f) return 1;
    return 0;
}
/**
 * @brief            :  [bat_volatge_full_check]
 * @retval           :  [0 - not 1 - full ]
 * @param[in]        :  [uint16_t v_max , double i_current]
 */
uint8_t bat_volatge_full_check(uint16_t v_max , float i_current) 
{
	static uint8_t full_count = 0;
    if(v_max > g_cells_full_volt && i_current > 0 )
	{
		full_count++;
		if(full_count > 5)
		{
			full_count=0;
			return 1;
		}
	}
	else
	{
		full_count = 0;
	}
	return 0;
}

/**
 * @brief            :  [bat_volatge_empty_check]
 * @retval           :  [0 - not 1 - empty ]
 * @param[in]        :  [uint16_t v_min , float i_current]
 */
uint8_t bat_volatge_empty_check(uint16_t v_min , float i_current)
{
	static uint8_t empty_count = 0;
    if(v_min <= g_cells_empty_volt && i_current < 0 )
	{
		empty_count++;
		if(empty_count > 5)
		{
			empty_count =0;
			return 1;
		}
		
	}	
	else 
	{
		empty_count = 0;
	}
	return 0;
}

/**
 * @brief            :  [soc_init_voltage_not_updata_capsoc]
 * @param[in]        :  [float i_current, \
                                        uint16_t vol, \
                                        int16_t T_average]
 */
static void soc_init_voltage_not_updata_capsoc(float i_current, \
                                        uint16_t vol_min, \
                                        int16_t T_average )
{
    float soc_min = 0;
    float soc_show_temp  = 0;
    static float socLast = 0;	
    float volt_min_f = (vol_min/1000.0f);
    if(i_current >= 0) return;

    if(g_soc > 0 && g_soc< 100)
    {

        soc_min = ocv_table_startup_soc(volt_min_f, T_average);

        if(fabsf(socLast - soc_min) > 0.05)
        {
            socLast = soc_min;
            return;
        }
        else
        {
            socLast = soc_min;
        }
        
        if((soc_min >= 0.20f)) // ocv_soc>=20 
        {
//            if((g_soc >= 20))
//            {
//                 soc_min  = g_soc / 100.0f;
//            }
//            else
//            {
//                soc_min = 0.20f;
//            }
//             soc_min  = g_soc / 100.0f;
            return ;
        }
        else // ocv_soc<20
        {
            soc_show_temp = g_soc / 100.0f;
            if(soc_show_temp <= soc_min)
            {
                soc_min = soc_show_temp;
            }
        }
        g_current_soc = soc_min;
        g_remain_capacity = g_current_soc*g_full_max_capacity;
        g_init_soc =  soc_min * 100;
        if(g_init_soc <= 0)g_init_soc = 1.0f;
        else if(g_init_soc >= 100) g_init_soc = 100.0f;
    }
}

/**
 * @brief            :  [soc_init_voltage_not_updata_capsoc]
 * @param[in]        :  [float i_current, \
                                        uint16_t vol, \
                                        int16_t T_average]
 */
static void soc_init_voltage_not_updata_capsoc_chg(float i_current, \
                                        uint16_t vol_min, \
                                        int16_t T_average )
{
    float soc_max = 0;
    float volt_max_f = (vol_min/1000.0f);
    if(i_current <= 0) return;

    if(g_soc > 0 && g_soc< 100)
    {

        soc_max = ocv_table_calc_chg_soc(volt_max_f, T_average);
        if(soc_max <= 0.8) 
        {
            if(g_soc <= 80.0f)
            {
                soc_max = g_soc / 100.0f;
            }
            else
            {
                soc_max = 0.80f;
            }
        }
        g_current_soc = soc_max;
        g_remain_capacity = g_current_soc*g_full_max_capacity;
        g_init_soc =  soc_max * 100.0f;
        if(g_init_soc <= 0)g_init_soc = 1.0f;
        else if(g_init_soc >= 100) g_init_soc = 100.0f;
    }
}

/**
 * @brief            :  [soc_init_value_though_input_value_upload]
 * @param[in]        :  [float socinput]
 */
static void soc_init_value_though_input_value_upload(float socinput)
{
    g_soc = socinput;
    if(g_soc < 0) g_soc = 0;
    else if(g_soc > 100) g_soc = 100;
    g_init_soc = g_soc;
    g_current_soc = g_init_soc / 100.0f;
    g_remain_capacity = g_current_soc * g_full_max_capacity;
    
}

/**
 * @brief            :  [soc_calibration]
 * @param[in]        :  [float i_current, \
                                        uint16_t vol, \
                                        int16_t T_average
 */
static void soc_calibration(float i_current ,uint16_t vol_max, \
                            uint16_t vol_min ,int16_t T_average)
{
    static uint32_t count_dsg=0,count_chg=0;
    static uint32_t count_standy=0;
    if(i_current == 0)
    {
        count_standy++;
        if(count_standy >=3600)
        {
            count_standy=0;
            float self_use_power = (1*10) / (g_normal_cap*10);
            float input_soc      = g_soc - self_use_power;
            soc_init_value_though_input_value_upload(input_soc);
        }
    }
    else 
    {
        count_standy=0;
    }
    
    if(i_current < 0)
    {
        count_dsg++;
        if(count_dsg>=10)
        {
            count_dsg =0;
            soc_init_voltage_not_updata_capsoc( i_current, vol_min, T_average );
        }
    }
    else 
    {
        count_dsg=0;
    }
    
    if(i_current > 0)
    {
        count_chg++;
        if(count_chg>=10)
        {
            count_chg =0;
            soc_init_voltage_not_updata_capsoc_chg( i_current, vol_max, T_average );
        }
    }
    else 
    {
        count_chg=0;
    }
}

/**
 * @brief            :  [soc_display]
 * @param[in]        :  [float current_i ,float soc , \
                         float detal_cap_mah ,float real_capacity]
 */
//uint8_t  bat_stasus =0;
static void soc_display(float current_i ,float soc , \
                        float detal_cap_mah ,float real_capacity)
{
    uint8_t  bat_stasus =0;
    uint16_t current_caps = g_normal_cap;
    bat_stasus = check_bat_status(current_i); // 0 - idle 1 - charging 2 - discharging
    if(bat_stasus == 1) // charging
    {
        if(soc < 1.0f)
        {
            g_init_soc += detal_cap_mah * 120.0f / real_capacity; // 105 110 120 
        }
        else 
        {
            g_init_soc = 100.0f;
        }
    }
    else if(bat_stasus == 2) // discharging
    {
        if(soc > 0.0f)
        {
            g_init_soc += detal_cap_mah * 105.0f / real_capacity; // 110
        }
        else
        {
            g_init_soc = 0.0f;
        }
    }

    if( g_init_soc >= 1 && 0 != g_empty_flg)
    {
        g_empty_flg = 0;
    }

    // init soc limit 
    if(g_init_soc > 100.0f)
    {
        g_init_soc = 100.0f;
    }
    else if (g_init_soc < 0.0f)
    {
        g_init_soc = 0.0f;
    }
    
    // g_soc  out  display smooth show
    float detal_soc = fabsf(g_init_soc - g_soc);
    if(detal_soc > 0.5f)
    {
        if( (current_i > 0 && bat_stasus == 1) || \
            (current_i < 0 && bat_stasus == 2) || \
            (current_i == 0 &&(g_init_soc == 0.0f || g_init_soc == 100.0f) )
          )
        {
            if(g_init_soc == 0.0f || g_init_soc == 100.0f)
            {
                g_soc = g_init_soc*0.3f+g_soc*0.7f;
            }
            else if (  ( ( (g_init_soc <= 3.0f && g_soc >= 6.0f  ) || \
                           (g_init_soc <= 8.0f && g_soc >= 10.0f ) //10
                         )  && \
                            current_i < 0 && (current_i*0.001f)  >  (-1*(current_caps*0.1f))
                       )  || \
                       ( 
                            ( (g_init_soc>=97.0f && g_soc <= 95.0f ) || \
                              (g_init_soc >= 92.0f &&g_soc <= 90.0f)
                            ) && \
                            current_i > 0 && (current_i*0.001f) < (current_caps*0.1f)
                       )
                    )
            {
                float detalsoc = fabsf(g_init_soc - g_soc);
                if(detalsoc >= 10.0f)
                {
                    g_soc = g_init_soc * 0.0006f + g_soc * 0.9994f;
                }
                else if(detalsoc >= 5.0f)
                {
                    g_soc = g_init_soc * 0.0009f + g_soc * 0.9991f;
                }
                else 
                {
                    g_soc = g_init_soc * 0.0015f + g_soc * 0.9985f;
                }
            }
            else
            {
                float tmeprate = 1.0f;
                if(current_i < 0.0f )
                {
                    tmeprate = g_soc / g_init_soc;
                    if(tmeprate >10.0f) //10
                    {
                        tmeprate = 10.0f; // 10
                    }
                    else if (tmeprate <0.6f   ) // 0.6 
                    {
                        tmeprate = 0.6f   ; // 0.6
                    }
                    g_soc += ((110.0f*detal_cap_mah) /real_capacity) * tmeprate; // 100-
                    if(g_soc <= 0.0f)
                    {
                        g_soc = 1.0f;
                    }
                }
                else if (current_i > 0.0f)
                {
                    tmeprate = (100.0-g_soc ) / (100.0-g_init_soc);
                    if(tmeprate >10) //10
                    {
                        tmeprate = 10;
                    }
                    else if (tmeprate < 0.6)
                    {
                        tmeprate = 0.6;
                    }
                    g_soc += ((120*detal_cap_mah) / real_capacity) * tmeprate;
                    if(g_soc >= 100)
                    {
                        g_soc = 100;
                    }
                }
            }
        }
        // if(g_init_soc == 0.0f || g_init_soc == 100.0f)
        // {
        //     g_soc = g_init_soc;
        // }
    }
    else
    {
        if( (g_soc  < g_init_soc && current_i > 0.0f) || \
            (g_soc  > g_init_soc && current_i < 0.0f) 
          )
        {
            g_soc = g_init_soc;
        }
        else if( current_i != 0.0f)
        {
            g_soc +=   ((detal_cap_mah * 100.0f / real_capacity)  * 0.6f);
        }

        if(g_init_soc == 0.0f || g_init_soc == 100.0f)
        {
            g_soc = g_init_soc;
        }
    }
    
    
    if(g_soc > 100.0f)
    {
        g_soc = 100.0f;
    }
    else if (g_soc < 0.0f) 
    {
        g_soc = 0.0f;
    }
}



/**
 * @brief            :  [soc_soh_calc]
 * @param[in]        :  [uint16_t volt_max, uint16_t volt_min, 
                         float i_current, int16_t tempera]
 */
#define SOC_DEBUG
float detal_cap_mah = 0;
void soc_soh_calc(uint16_t volt_max, uint16_t volt_min, \
                  float i_current, int16_t tempera )
{
    // i current ma  g_full_max_capacity  mah
    #ifndef SOC_DEBUG 
    float detal_cap_mah = get_detal_capacity (i_current, \
                                             g_full_max_capacity, \
                                             tempera);
    #else 
    detal_cap_mah = get_detal_capacity (i_current, \
                                        g_full_max_capacity, \
                                        tempera);
    #endif
    uint16_t temp_diff_volt = volt_max-volt_min;
    // circle count caclate
    if(i_current<0.0f)
    {
       cycle_count_calculate(detal_cap_mah);
    }
    // check the battry capcity change 
    check_caps_change();
    // calc ah soc 
    g_remain_capacity += detal_cap_mah;
    g_current_soc = g_remain_capacity / g_full_real_capacity; // current soc 0.0 ~ 1.0 
    if(g_current_soc>=1.0f) 
    {
       g_current_soc = 1.0f; 
    }
    else if (g_current_soc<=0.0f)
    {
        g_current_soc =0.0f;
    }
    
    if(temp_diff_volt < g_volt_max_diff) // max cell and min cell volt must 
    {
        if(1 == bat_volatge_full_check(volt_max,i_current))
        {
            g_current_soc = 1.0f;
            g_init_soc    = 100.0f;
            g_remain_capacity = g_full_real_capacity;
        }
        else if (1 == bat_volatge_empty_check(volt_min,i_current))
        {
            g_current_soc = 0.0f;
            g_init_soc    = 0.0f;
            g_remain_capacity = 0.0f;
        }
    }
    
    if(g_remain_capacity >= g_full_max_capacity)
    {
        g_remain_capacity = g_full_max_capacity;
    }
    else if(g_remain_capacity <= 0.0f)
    {
        g_remain_capacity = 0.0f;
    }
    
    if(g_full_real_capacity >= g_full_max_capacity)
    {
        g_full_real_capacity = g_full_max_capacity;
    }
    else if(g_full_real_capacity <= 0.0f)
    {
        g_full_real_capacity = 0.0f;
    }
    
    if(g_current_soc>=1.0f &&  g_remain_capacity > g_full_real_capacity)
    {
        g_full_real_capacity = g_remain_capacity;
    }
    


    // update soc show
    soc_display(i_current,  \
                g_current_soc,  \
                detal_cap_mah, \
                g_full_real_capacity);
    // soc-ocv clibrations
    if(temp_diff_volt < g_volt_max_diff) // max cell and min cell volt must 
    {
        soc_calibration(i_current,volt_max,volt_min,tempera);
    }
}


/**
 * @brief            :  [soc_max_or_min_calibration]
 * @param[in]        :  [uint8_t ov_flg,uint8_t uv_flg ,uint16_t volt_diff]
 */
void soc_max_or_min_calibration(uint8_t ov_flg,uint8_t uv_flg ,uint16_t volt_diff)
{
    static uint8_t max_calib_flg = 0;
    static uint8_t mix_calib_flg = 0;
    if(volt_diff>=g_volt_max_diff)
    {
        max_calib_flg = 0;
        mix_calib_flg = 0;
        return ;
    }        
    
    if((ov_flg) && 0 == max_calib_flg)
    {
        g_current_soc     = 1.0f;
        g_init_soc        = 100;
        max_calib_flg     = 1;
        if(1 == compare_capcity(g_full_real_capacity))
        {
            g_full_real_capacity = g_remain_capacity;
            g_remain_capacity = g_current_soc * g_full_real_capacity;
        }
        else 
        {
            g_remain_capacity = g_full_real_capacity;
        }
        g_soc                 = g_init_soc;
    }
    else if ( (0x00 == ov_flg ) && \
               1 == max_calib_flg)
    {
        max_calib_flg = 0;
    }
    
    
    if((uv_flg) && 0 == mix_calib_flg)
    {
        mix_calib_flg     = 1;
        g_remain_capacity = 0.0f;
        g_current_soc     = 0.0f;
        g_init_soc        = 0.0f;
        g_soc             = g_init_soc;
        g_empty_flg       = 1;
    }
    else if ( (0x00 == (uv_flg) ) && \
               1 == mix_calib_flg)
    {
        mix_calib_flg=0;
    }
}


/**
 * @brief            :  [set_soc_caps_value]
 * @retval           :  [none]
 * @param[in]        :  [uint16_t value]
 */
void set_soc_caps_value(uint16_t value)
{
      g_normal_cap = value;
} 

/**
 * @brief            :  [get_soc_caps_value]
 * @retval           :  [g_normal_cap]
 */
uint16_t get_soc_caps_value(void)
{
      return g_normal_cap;
} 

/**
 * @brief            :  [get_soc_caps_value]
 * @retval           :  [g_normal_cap]
 */
void set_soh_value(uint16_t value)
{
       g_soh = value;
} 


/**
 * @brief            :  [get_soh_value]
 * @retval           :  [g_soh]
 */
uint16_t get_soh_value(void)
{
     return g_soh;
} 

/**
 * @brief            :  [set_cycle_count]
 * @retval           :  [g_soh]
 * @param[in]        :  [uint16_t value]
 */
void set_cycle_count (uint16_t value )
{
    g_cycle_count = value;
}


/**
 * @brief            :  [get_cycle_count]
 * @retval           :  [g_cycle_count]
 */
uint16_t get_cycle_count (void)
{
    return g_cycle_count;
}

/**
 * @brief            :  [get_soc_cells_full_volt]
 * @retval           :  [g_cells_full_volt]
 */
uint16_t get_soc_cells_full_volt(void)
{
    return g_cells_full_volt;
}

/**
 * @brief            :  [get_soc_cells_empty_volt]
 * @retval           :  [g_cells_empty_volt]
 */
uint16_t get_soc_cells_empty_volt(void)
{
    return g_cells_empty_volt;
}

/**
 * @brief            :  [get_soc_volt_max_diff]
 * @retval           :  [g_volt_max_diff]
 */
uint16_t get_soc_volt_max_diff(void)
{
    return g_volt_max_diff;
}

/**
 * @brief            :  [get_soc_cells_full_volt]
 * @retval           :  [g_cells_full_volt]
 */
void set_soc_cells_full_volt(uint16_t value)
{
    g_cells_full_volt = value;
}

/**
 * @brief            :  [get_soc_cells_empty_volt]
 * @retval           :  [g_cells_empty_volt]
 */
void set_soc_cells_empty_volt(uint16_t value)
{
    g_cells_empty_volt = value;
}

/**
 * @brief            :  [set_soc_volt_max_diff]
 * @retval           :  [value]
 */
void set_soc_volt_max_diff(uint16_t value)
{
    g_volt_max_diff = value;
}

/**
 * @brief            :  [get_soc_value]
 * @retval           :  [g_soc]
 */
uint8_t get_soc_value(void)
{
    uint8_t temp_value_soc = 0;
    if(g_soc >= 99.5f)
    {
        temp_value_soc = 100;
        return temp_value_soc;
    }
    else if (g_soc <= 1.0f && g_empty_flg == 0)
    {
        temp_value_soc = 1;
        return temp_value_soc;
    }
    temp_value_soc = (uint8_t)g_soc;
    return temp_value_soc;
}
/**
 * @brief            :  [soc_init]
 * @retval           :  [g_soc]
 * @param[in]        :  [uint16_t volt, \
                         int16_t tempera , \
                         uint16_t bat_capcity]
 */
void soc_init (uint8_t pre_soc , uint16_t volt, int16_t tempera ,uint16_t bat_capcity)
{
    float soc_temp_value   =  ocv_table_startup_soc((volt/1000.0f), tempera);     // soc 0.0 ~ 1.0
    float volt_soc_rate100 =  soc_temp_value*100.0f;
    
    float detal_soc = fabsf(volt_soc_rate100 - pre_soc);
    if(detal_soc<=10.0f) soc_temp_value = pre_soc/100.0f;
    
    if(soc_temp_value >= 1.0f)
    {
        soc_temp_value = 1.0f;
    }
    else if (soc_temp_value<=0.0f)
    {
        soc_temp_value =0.0f;
    }
    // check input caps 
    if(0==g_normal_cap)
    {
        g_normal_cap = bat_capcity;      // keep the battry capci 
    }
    // soc 
    g_current_soc = soc_temp_value;  // current soc 0.0 ~ 1.0
    g_init_soc  = soc_temp_value * 100.0f; // init soc 0 -100
    g_soc = g_init_soc;      // soc 0 ~ 100
    // calc remain capacity 
    g_old_normal_cap        = g_normal_cap;
    g_full_max_capacity     = bat_capcity*g_soh;//  the battry max capcity uinit : mah
    g_full_real_capacity    = g_full_max_capacity; // mah
    g_remain_capacity       = g_current_soc * g_full_max_capacity; // remain capacity
    // 
    if(g_init_soc<=0.0f) g_empty_flg = 1;
}


/* end of file --------------------------------------------------------------*/




