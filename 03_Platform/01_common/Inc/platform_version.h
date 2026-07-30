/**
 ******************************************************************************
 *@file               :   platform_version.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef PLATFORM_VERSION_H
#define PLATFORM_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */
#include <stddef.h>                               /* stdint lib header file. */
#include "platform_error.h"
#include "version_config.h"
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
    uint8_t build;
    const char *p_product_name;
    const char *p_version_string;
    const char *p_git_hash;
    const char *p_build_date;
    const char *p_build_time;
} platform_version_t;
/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
platform_err_t platform_version_get(platform_version_t *p_out);
const char *platform_version_get_string(void);

#ifdef __cplusplus
}
#endif




#endif /* __PLATFORM_VERSION_H__ */
