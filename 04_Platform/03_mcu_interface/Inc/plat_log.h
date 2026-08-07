/**
 ******************************************************************************
 *@file               :   plat_log.h
 *@brief              :   Platform log abstraction — callers only see
 *                        plat_log_x(tag, fmt, ...) and never touch the
 *                        backend (EasyLogger/RTT) or USE_DEBUG_LOG directly.
 *                        Compiles to no-ops when USE_DEBUG_LOG is off.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef PLAT_LOG_H
#define PLAT_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include "plat_error.h" /* platform error code header file. */

/* define -------------------------------------------------------------------*/
#ifdef USE_DEBUG_LOG
#include "elog.h"
#define plat_log_v(tag, ...)  elog_v(tag, __VA_ARGS__)
#define plat_log_d(tag, ...)  elog_d(tag, __VA_ARGS__)
#define plat_log_i(tag, ...)  elog_i(tag, __VA_ARGS__)
#define plat_log_w(tag, ...)  elog_w(tag, __VA_ARGS__)
#define plat_log_e(tag, ...)  elog_e(tag, __VA_ARGS__)
#define plat_log_hexdump(name, width, buf, size) \
    elog_hexdump(name, width, buf, size)
#else
#define plat_log_v(tag, ...)  ((void)0)
#define plat_log_d(tag, ...)  ((void)0)
#define plat_log_i(tag, ...)  ((void)0)
#define plat_log_w(tag, ...)  ((void)0)
#define plat_log_e(tag, ...)  ((void)0)
#define plat_log_hexdump(name, width, buf, size) ((void)0)
#endif /* USE_DEBUG_LOG */

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
platform_err_t plat_log_init(void);

#ifdef __cplusplus
}
#endif

#endif /* PLAT_LOG_H */
