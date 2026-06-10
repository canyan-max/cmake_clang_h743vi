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
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
 
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef SOC_H
#define SOC_H
/*Include-----------------------------------------------------------*/
#include <stdint.h>
/*Define------------------------------------------------------------*/
#define CELL_CAPS                                                (15U)
/*Variable----------------------------------------------------------*/

/*Declaration-------------------------------------------------------*/
/**
 * @brief            :  [get_soc_volt_max_diff]
 * @retval           :  [g_volt_max_diff]
 */
uint16_t get_soc_volt_max_diff(void);
/**
 * @brief            :  [set_soc_volt_max_diff]
 * @retval           :  [value]
 */
void set_soc_volt_max_diff(uint16_t value);

/**
 * @brief            :  [get_soc_cells_full_volt]
 * @retval           :  [g_cells_full_volt]
 */
uint16_t get_soc_cells_full_volt(void);

/**
 * @brief            :  [get_soc_cells_empty_volt]
 * @retval           :  [g_cells_empty_volt]
 */
uint16_t get_soc_cells_empty_volt(void);
/**
 * @brief            :  [get_soc_cells_full_volt]
 * @retval           :  [g_cells_full_volt]
 */
void set_soc_cells_full_volt(uint16_t value);

/**
 * @brief            :  [get_soc_cells_empty_volt]
 * @retval           :  [g_cells_empty_volt]
 */
void set_soc_cells_empty_volt(uint16_t value);
/**
 * @brief            :  [set_soc_caps_value]
 * @retval           :  [none]
 * @param[in]        :  [uint16_t value]
 */
void set_soc_caps_value(uint16_t value);
/**
 * @brief            :  [get_soc_caps_value]
 * @retval           :  [g_normal_cap]
 */
uint16_t get_soc_caps_value(void);
/**
 * @brief            :  [get_soh_value]
 * @retval           :  [g_soh]
 */
uint16_t get_soh_value(void);

/**
 * @brief            :  [set_soh_value]
 */
void set_soh_value(uint16_t value);
/**
 * @brief            :  [set_cycle_count]
 * @retval           :  [g_soh]
 * @param[in]        :  [uint16_t value]
 */
void set_cycle_count (uint16_t value );

/**
 * @brief            :  [get_cycle_count]
 * @retval           :  [g_cycle_count]
 */
uint16_t get_cycle_count (void);
/**
 * @brief            :  [get_soc_value]
 * @retval           :  [g_soc]
 */
uint8_t get_soc_value(void);
/**
 * @brief            :  [soc_init]
 * @retval           :  [g_soc]
 * @param[in]        :  [uint16_t volt, int16_t tempera ,uint16_t bat_capcity]
 */
void soc_init (uint8_t pre_soc , uint16_t volt, int16_t tempera ,uint16_t bat_capcity);

/**
 * @brief            :  [soc_soh_calc]
 * @param[in]        :  [uint16_t volt, float i_current, int16_t tempera]
 */

void soc_soh_calc(uint16_t volt_max, uint16_t volt_min, float i_current, int16_t tempera );
/**
 * @brief            :  [soc_max_or_min_calibration]
 * @param[in]        :  [uint8_t ov_flg,uint8_t uv_flg]
 */
void soc_max_or_min_calibration(uint8_t ov_flg,uint8_t uv_flg ,uint16_t volt_diff);
#endif /*__SOC_H*/

