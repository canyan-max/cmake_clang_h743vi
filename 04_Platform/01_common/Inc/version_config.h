/**
 ******************************************************************************
 *@file               :   version_config.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef VERSION_CONFIG_H
#define VERSION_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes -----------------------------------------------------------------*/
/* define -------------------------------------------------------------------*/
#define PLATFORM_VERSION_MAJOR   1U
#define PLATFORM_VERSION_MINOR   0U
#define PLATFORM_VERSION_PATCH   0U
#define PLATFORM_VERSION_BUILD   0U

#define PLATFORM_PRODUCT_NAME    "cmake-clang-h743vi"
#define PLATFORM_VERSION_STRING  "v1.0.0"
#define PLATFORM_GIT_HASH        "unknown"
#define PLATFORM_BUILD_DATE      __DATE__
#define PLATFORM_BUILD_TIME      __TIME__
/* typedef ------------------------------------------------------------------*/

/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif // end of __cplusplus


#endif /* __VERSION_CONFIG_H__ */
