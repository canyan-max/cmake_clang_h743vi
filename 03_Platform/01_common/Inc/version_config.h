/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file version_config.h
 *
 * @brief Firmware version configuration macros.
 *        Edit these macros before each release build.
 *
 * @version V1.0 2026-07-29
 *
 * @note 1 tab == 4 spaces!
 *
 *****************************************************************************/

#ifndef __VERSION_CONFIG_H__
#define __VERSION_CONFIG_H__

//******************************** Version Macros ***************************//

#define PLATFORM_VERSION_MAJOR   1U
#define PLATFORM_VERSION_MINOR   0U
#define PLATFORM_VERSION_PATCH   0U
#define PLATFORM_VERSION_BUILD   0U

#define PLATFORM_PRODUCT_NAME    "cmake-clang-h743vi"
#define PLATFORM_VERSION_STRING  "v1.0.0"
#define PLATFORM_GIT_HASH        "unknown"
#define PLATFORM_BUILD_DATE      __DATE__
#define PLATFORM_BUILD_TIME      __TIME__

#endif /* __VERSION_CONFIG_H__ */
