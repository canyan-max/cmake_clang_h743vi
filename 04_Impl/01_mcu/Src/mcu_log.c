/**
 ******************************************************************************
 *@file               :   mcu_log.c
 *@brief              :   STM32 implementation of plat_log — starts
 *                        EasyLogger (routed to RTT/UART per USE_JLINK_RTT).
 *                        No-op when USE_DEBUG_LOG is off.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "plat_log.h"
#ifdef USE_DEBUG_LOG
#include "elog.h"
#endif

/* exported functions -------------------------------------------------------*/

platform_err_t plat_log_init(void)
{
#ifdef USE_DEBUG_LOG
    if(ELOG_NO_ERR != elog_init())
    {
        return PLATFORM_ERR_HW;
    }
    elog_start();
#endif
    return PLATFORM_ERR_OK;
}

/* end of file --------------------------------------------------------------*/
