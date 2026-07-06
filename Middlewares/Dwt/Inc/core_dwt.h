/**
 ******************************************************************************
 *@file               :   core_dwt.h
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef CORE_DWT_H
#define CORE_DWT_H


/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
/* Private includes ---------------------------------------------------------*/

/* Private typedef ----------------------------------------------------------*/

/* Private define -----------------------------------------------------------*/

/* Private variables --------------------------------------------------------*/

/* Private function prototypes ----------------------------------------------*/

/* Exported types -----------------------------------------------------------*/

/* Exported variables -------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/

/** 
* @brief     	 	:  dwt_init
* @retval      		:  None
* @param      	 	:  None 
*/
void dwt_init(void);
/** 
* @brief     	 	:  dwt_delay_us
* @retval      		:  None
* @param      	 	:  delay times for us
*/
void dwt_delay_us(uint32_t us);
/** 
* @brief     	 	:  get_dwt_us
* @retval      		:  us
* @param      	 	:  none
*/
uint32_t get_dwt_us(void);
#endif
  /*core_dwt.h*/
  
  
