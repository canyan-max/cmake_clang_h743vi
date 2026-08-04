/**
 ******************************************************************************
 *@file               :   proto_files.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>      /* stdint lib header file. */
#include "proto_files.h" /* proto_files lib header file. */
#include <string.h>      /* stdint lib header file. */
/* define   -----------------------------------------------------------------*/
#define FILE_NAME_LENGTH    (64U)
#define PACKET_DATA_INDEX   (4U)
#define FILE_SIZE_LENGTH    ((uint32_t)16)
#define PROTO_CLEAR(ptr) \
do { \
    memset((ptr), 0, sizeof(*(ptr))); \
} while(0)

#define PROTO_GET_ARR_PACK_TYPE(parser, arr) \
do { \
    (parser)->pack_type = (pack_type_frame_t)(arr)[0]; \
} while(0)

#define PROTO_GET_ARR_RESERVER(parser, arr) \
do { \
    (parser)->reserver = (arr)[1]; \
} while(0)

#define PROTO_GET_ARR_PACK_NUM(parser, arr) \
do { \
    (parser)->package_number = (arr)[2]; \
} while(0)

#define PROTO_GET_ARR_PACK_UN_NUM(parser, arr) \
do { \
    (parser)->package_un_numer = (arr)[3]; \
} while(0)
/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static uint8_t g_file_name[FILE_NAME_LENGTH];
/* private  functions  ------------------------------------------------------*/
uint16_t updata_crc16(uint16_t crc_in, uint8_t byte)
{
  uint32_t crc = crc_in;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}

uint16_t cal_crc16(const uint8_t* p_data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = p_data+size;

  while(p_data < dataEnd)
    crc = updata_crc16(crc, *p_data++);
 
  crc = updata_crc16(crc, 0);
  crc = updata_crc16(crc, 0);

  return crc&0xffffu;
}

static proto_files_ret_t
proto_idle_to_recive_file_info(proto_files_parser_t *p_parse, uint8_t *p_data)
{

    if(p_data[0] == 0x42 && p_data[1] == 0x88)
    {
        p_parse->state = PROTO_FILE_RECEIVE_FILE_INFO;
        return PROTO_FILE_RET_OK;
    }
    return PROTO_FILE_RET_ERR;
}

static proto_files_ret_t
proto_recive_file_info_to_data_info(proto_files_parser_t *p_parser,
                                    uint8_t              *data)
{
    PROTO_GET_ARR_PACK_TYPE(p_parser, data);
    PROTO_GET_ARR_RESERVER(p_parser, data);
    PROTO_GET_ARR_PACK_NUM(p_parser, data);
    PROTO_GET_ARR_PACK_UN_NUM(p_parser, data);

    uint8_t *file_ptr = NULL;
    uint8_t file_size[FILE_SIZE_LENGTH];
    uint8_t i = 0;
    if(PACK_TYPE_128_FRAM != p_parser->pack_type  || \
       p_parser->package_number!=0x5A || \
       p_parser->package_un_numer!=0xA5 
    )  
    {
        return PROTO_FILE_RET_ERR;
    }
    // 

    file_ptr = data + PACKET_DATA_INDEX;
    while ( (*file_ptr != 0) && (i < FILE_NAME_LENGTH))
    {
        g_file_name[i++] = *file_ptr++;
    }

    g_file_name[i++] = '\0';
    i = 0;
    file_ptr ++;
    while ( (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH))
    {
        file_size[i++] = *file_ptr++;
    }
    file_size[i++] = '\0';

    // Str2Int(file_size, &filesize);
    
    //goto  data pack  info index
    p_parser->state = PROTO_FILE_RECEIVE_DATA_INFO;

    return PROTO_FILE_RET_OK;
}

static proto_files_ret_t proto_recive_data_pack(proto_files_parser_t *p_parser,
                                                uint8_t              *data)
{
    PROTO_GET_ARR_PACK_TYPE(p_parser, data);
    PROTO_GET_ARR_RESERVER(p_parser, data);
    PROTO_GET_ARR_PACK_NUM(p_parser, data);
    PROTO_GET_ARR_PACK_UN_NUM(p_parser, data);

    return PROTO_FILE_RET_OK;
}
/* exported functions -------------------------------------------------------*/

proto_files_ret_t proto_files_init(proto_files_parser_t *p_parser)
{
    if(NULL == p_parser)
    {
        return PROTO_FILE_RET_ERR;
    }
    PROTO_CLEAR(p_parser);
    return PROTO_FILE_RET_OK;
}

proto_files_ret_t proto_files_process(proto_files_parser_t *p_parser,
                                      uint8_t              *data)
{
    if(NULL == p_parser || NULL == data)
    {
        return PROTO_FILE_RET_ERR;
    }
    proto_files_ret_t ret = PROTO_FILE_RET_OK;

    switch(p_parser->state)
    {
        case PROTO_FILE_IDLE:
        {

            ret = proto_idle_to_recive_file_info(p_parser, data);
            if(PROTO_FILE_RET_OK != ret)
            {
                PROTO_CLEAR(p_parser);
                return PROTO_FILE_RET_ERR;
            }
            break;
        }

        case PROTO_FILE_RECEIVE_FILE_INFO:
        {
            ret = proto_recive_file_info_to_data_info(p_parser, data);
            if(PROTO_FILE_RET_OK != ret)
            {
                PROTO_CLEAR(p_parser);
                return PROTO_FILE_RET_ERR;
            }
            break;
        }

        case PROTO_FILE_RECEIVE_DATA_INFO:
        {
            ret = proto_recive_data_pack(p_parser, data);
            if(PROTO_FILE_RET_OK != ret)
            {
                PROTO_CLEAR(p_parser);
                return PROTO_FILE_RET_ERR;
            }
            break;
        }

        default:
        {
            PROTO_CLEAR(p_parser);
            return PROTO_FILE_RET_ERR;
            break;
        }
    }

    return PROTO_FILE_RET_OK;
}

/* end of  file -------------------------------------------------------------*/
