/**
 ******************************************************************************
 *@file               :   platform_version.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>       /* stdint lib header file. */
#include "platform_version.h"

/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static const platform_version_t g_platform_version = {
    (uint8_t)PLATFORM_VERSION_MAJOR,
    (uint8_t)PLATFORM_VERSION_MINOR,
    (uint8_t)PLATFORM_VERSION_PATCH,
    (uint8_t)PLATFORM_VERSION_BUILD,
    PLATFORM_PRODUCT_NAME,
    PLATFORM_VERSION_STRING,
    PLATFORM_GIT_HASH,
    PLATFORM_BUILD_DATE,
    PLATFORM_BUILD_TIME
};
/* private  functions  ------------------------------------------------------*/

/* exported functions -------------------------------------------------------*/
platform_err_t platform_version_get(platform_version_t *p_out)
{
    if (NULL == p_out)
    {
        return PLATFORM_ERR_PARAM;
    }

    *p_out = g_platform_version;

    return PLATFORM_ERR_OK;
}

const char *platform_version_get_string(void)
{
    return g_platform_version.p_version_string;
}

/* end of  file -------------------------------------------------------------*/


