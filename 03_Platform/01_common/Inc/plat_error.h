/******************************************************************************
 * @file plat_error.h
 * @brief Platform layer common error codes.
 * @version V1.0 2026-07-29
 * @note 1 tab == 4 spaces!
 *****************************************************************************/

#ifndef PLATFORM_ERROR_H
#define PLATFORM_ERROR_H

//******************************** Includes *********************************//

#include <stdint.h>

//******************************** Declaring ********************************//

typedef enum
{
    PLATFORM_ERR_OK       = 0x00U, /**< Operation completed successfully. */
    PLATFORM_ERR_PARAM    = 0x01U, /**< Invalid parameter. */
    PLATFORM_ERR_HW       = 0x02U, /**< Hardware error. */
    PLATFORM_ERR_BUSY     = 0x03U, /**< Resource busy. */
    PLATFORM_ERR_TIMEOUT  = 0x04U, /**< Operation timed out. */
} platform_err_t;

#endif /* __PLATFORM_ERROR_H__ */
