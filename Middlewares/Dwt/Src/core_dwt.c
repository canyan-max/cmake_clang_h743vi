/**
*******************************************************************************
*Copyright (C), Inc.(Gmbh) or its affiliates.
*All Rights Reserved.
*
*@file               :   core_dwt.c
*
*@pardependencies    :
*                        core_dwt.c
*                        stdint.h
*                        stm32h7xx.h
*
*@author             :   null
*
*@brief              :   Provide the HAL APIs of dwt

                         The content of this source file has been tested.
                         The coterx-M7 core must write to the register LAR
                         0xC5ACCE55
*
*@Processingflow     :
*
*@calldirectly       :
*
*@version            :   V1.0
*
*@note               :   1 tab == 4 spaces!
*******************************************************************************
*/

/* Includes -----------------------------------------------------------------*/
#include "core_dwt.h"
#include "stm32h7xx.h"
/* Define   -----------------------------------------------------------------*/
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

#define DWT_LAR_KEY   0xC5ACCE55
#define DWT_LSR_SLI   (1 << 0)
#define DWT_LSR_SLK   (1 << 1)
/* Private  variables -------------------------------------------------------*/
/* Exported variables -------------------------------------------------------*/
/* Private  functions  ------------------------------------------------------*/
/**
 * @brief     	 	:  dwt_ticks_to_us
 * @retval      	:  time_us
 * @param      	 	:  ticks dwt ticks
 */
static uint32_t dwt_ticks_to_us(uint32_t ticks)
{
    // 64-bit data must be used to prevent data overflow.
    uint64_t time_us = (uint64_t)ticks * 1000000ULL / SystemCoreClock;
    return (uint32_t)time_us;
}
// static uint32_t get_cpu_freq(void)
//{
//   HAL_RCC_GetSysClockFreq() \
//   >> D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_D1CPRE) >>
//   RCC_D1CFGR_D1CPRE_Pos];
// }
/* Exported functions --------------------------------------------------------*/
/**
* @brief     	 	:  dwt_init
                       the coterx-M7 core must write to the register LAR
0xC5ACCE55 delay 100 ms to wait for the dwt ready
* @retval      		:  None
* @param      	 	:  None
*/
void dwt_init(void)
{

#ifdef STM32H7
    HAL_Delay(100);
    uint32_t lsr = *((volatile uint32_t *)DWT->LAR);
    if((lsr & DWT_LSR_SLI) && (lsr & DWT_LSR_SLK))
    {
        DWT->LAR = 0xC5ACCE55;
    }
    __DSB();
#endif // end of  STM32H7
    // set the demcr bit24
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    __DSB();
    // clear dwt tick
    DWT->CYCCNT = 0;
    // enable the cyccnt reg
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __DSB();
}

/**
 * @brief     	 	:  dwt_delay_us
 * @retval      		:  None
 * @param      	 	:  delay times for us
 */
void dwt_delay_us(uint32_t us)
{
    // check if the input parameters are valid
    if(0 == us)
        return;
    // calculate the us clock using the system clock
    uint32_t ticks_per_us = SystemCoreClock / 1000000;
    if(ticks_per_us == 0)
        ticks_per_us = 1;
    uint32_t delay_ticks = us * ticks_per_us;
    uint32_t start_tick  = DWT->CYCCNT;
    uint32_t elapsed_ticks;
    // waiting the delay time finished
    do
    {
        uint32_t current_tick = DWT->CYCCNT;
        if(current_tick >= start_tick)
        {
            elapsed_ticks = current_tick - start_tick;
        }
        else
        {
            elapsed_ticks = (UINT32_MAX - start_tick) + current_tick + 1;
        }
    } while(elapsed_ticks < delay_ticks);
}

/**
 * @brief     	 	:  get_dwt_us
 * @retval      		:  us
 * @param      	 	:  none
 */
uint32_t get_dwt_us(void)
{
    return dwt_ticks_to_us(DWT->CYCCNT);
}
