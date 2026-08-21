/**
 ******************************************************************************
 *@file               :   mcu_sys.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <limits.h>
#include <stddef.h>
#include "stm32h7xx_hal.h"
#include "plat_sys.h"
/* define   -----------------------------------------------------------------*/
#define PLAT_DWT_LAR_KEY      (0xC5ACCE55UL)
#define PLAT_DWT_LSR_PRESENT  (1UL << 0U)
#define PLAT_DWT_LSR_LOCKED   (1UL << 1U)

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static uint8_t s_time_initialized;

/* private  functions  ------------------------------------------------------*/
static uint32_t plat_time_ticks_to_us(uint32_t ticks)
{
    if(0U == SystemCoreClock)
    {
        return 0U;
    }

    uint64_t time_us = (uint64_t)ticks * 1000000ULL / SystemCoreClock;
    return (uint32_t)time_us;
}

/* exported functions -------------------------------------------------------*/
platform_err_t plat_time_init(void)
{
    s_time_initialized = 0U;

    /* Keep the proven STM32H7 startup delay used by the previous DWT driver. */
    HAL_Delay(100U);

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    __DSB();

    uint32_t lock_status = DWT->LSR;
    if((0U != (lock_status & PLAT_DWT_LSR_PRESENT)) &&
       (0U != (lock_status & PLAT_DWT_LSR_LOCKED)))
    {
        DWT->LAR = PLAT_DWT_LAR_KEY;
    }

    if(0U != (DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk))
    {
        return PLATFORM_ERR_HW;
    }

    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __DSB();
    __ISB();

    if(0U == (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk))
    {
        return PLATFORM_ERR_HW;
    }

    uint32_t start_tick = DWT->CYCCNT;
    __NOP();
    __NOP();
    __NOP();
    __DSB();
    if(DWT->CYCCNT == start_tick)
    {
        return PLATFORM_ERR_HW;
    }

    s_time_initialized = 1U;
    return PLATFORM_ERR_OK;
}

uint32_t plat_time_get_us(void)
{
    if(0U == s_time_initialized)
    {
        return 0U;
    }

    return plat_time_ticks_to_us(DWT->CYCCNT);
}

void plat_delay_us(uint32_t us)
{
    if((0U == s_time_initialized) || (0U == us))
    {
        return;
    }

    uint32_t ticks_per_us = SystemCoreClock / 1000000U;
    if(0U == ticks_per_us)
    {
        return;
    }

    uint32_t max_chunk_us = UINT32_MAX / ticks_per_us;
    while(0U != us)
    {
        uint32_t chunk_us    = (us > max_chunk_us) ? max_chunk_us : us;
        uint32_t delay_ticks = chunk_us * ticks_per_us;
        uint32_t start_tick  = DWT->CYCCNT;
        while((uint32_t)(DWT->CYCCNT - start_tick) < delay_ticks)
        {
        }
        us -= chunk_us;
    }
}

uint32_t plat_tick_get_ms(void)
{
    return (uint32_t)HAL_GetTick();
}

void plat_delay_ms(uint32_t ms)
{
    if(0U == ms)
    {
        return;
    }

    HAL_Delay(ms);
}

void plat_dcache_invalidate(void *p_addr, int32_t size)
{
    SCB_InvalidateDCache_by_Addr((uint32_t *)p_addr, size);
}

/* end of  file -------------------------------------------------------------*/
