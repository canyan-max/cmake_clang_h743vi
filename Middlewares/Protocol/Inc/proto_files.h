/**
 ******************************************************************************
 *@file               :   proto_files.h
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef PROTO_FILES_H
#define PROTO_FILES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */
#include <stddef.h>                               /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef enum
{
    PROTO_FILE_RET_OK          = 0x00U,
    PROTO_FILE_RET_ERR         = 0x01U,
} proto_files_ret_t;

typedef enum
{
    PACK_TYPE_128_FRAM         = 0x01U,
    PACK_TYPE_1024_FRAM        = 0x02U,
} pack_type_frame_t;

typedef enum
{
    PROTO_FILE_IDLE                      = 0x00U,
    PROTO_FILE_RECEIVE_FILE_INFO         = 0x01U,
    PROTO_FILE_RECEIVE_DATA_INFO         = 0x02U,
} proto_files_state_t;

typedef struct
{
    proto_files_state_t  state;
    pack_type_frame_t    pack_type;
    uint8_t              reserver;
    uint8_t              package_number;
    uint8_t              package_un_numer;
    uint8_t              crc_h;
    uint8_t              crc_l;
} proto_files_parser_t;
/* exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PROTO_FILES_H */
