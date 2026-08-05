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
#define FILE_NAME_LENGTH            (64U)
#define FILE_SIZE_LENGTH            ((uint32_t)16)
#define PROTO_FILES_HANDSHAKE_LEN   (7U)
#define PROTO_FILES_INFO_FRAME_LEN  (PACKET_DATA_INDEX + PACK_FRAME_SIZE_128B + 2U)

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
static uint8_t g_file_buffer[PROTO_FILES_INFO_FRAME_LEN];
/* private  functions  ------------------------------------------------------*/
static uint16_t updata_crc16(uint16_t crc_in, uint8_t byte)
{
    uint32_t crc = crc_in;
    uint32_t in  = byte | 0x100;

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

static uint16_t cal_crc16(const uint8_t *p_data, uint32_t size)
{
    uint32_t       crc     = 0;
    const uint8_t *dataEnd = p_data + size;

    while(p_data < dataEnd)
        crc = updata_crc16(crc, *p_data++);

    crc = updata_crc16(crc, 0);
    crc = updata_crc16(crc, 0);

    return crc & 0xffffu;
}

/* Accumulate the 2-byte handshake (0x42 0x88). Byte-at-a-time so it works
 * no matter how the transport chunks delivery. */
static proto_files_ret_t proto_files_feed_idle(proto_files_parser_t *p_parser,
                                               uint8_t               byte)
{
    p_parser->buf[p_parser->idx] = byte;
    p_parser->idx++;

    if(p_parser->idx < PROTO_FILES_HANDSHAKE_LEN)
    {
        return PROTO_FILE_RET_OK; /* still waiting for the 2nd byte */
    }
    /*I->0x49 S->0x53 R->0x52 E->0x45 A->0x41 D->0x44 Y->0x59 */
    if((0x49U == p_parser->buf[0]) && (0x53U == p_parser->buf[1]) &&
       (0x52U == p_parser->buf[2]) && (0x45U == p_parser->buf[3]) &&
       (0x41U == p_parser->buf[4]) && (0x44U == p_parser->buf[5]) &&
       (0x59U == p_parser->buf[6]))
    {
        p_parser->idx   = 0U;
        p_parser->state = PROTO_FILE_RECEIVE_FILE_INFO;
        return PROTO_FILE_RET_OK;
    }
    return PROTO_FILE_RET_ERR;
}

/* Accumulate the fixed-size (134B) file-info frame, then validate + parse
 * it exactly like the previous whole-buffer version did. */
static proto_files_ret_t
proto_files_feed_file_info(proto_files_parser_t *p_parser, uint8_t byte)
{
    p_parser->buf[p_parser->idx] = byte;
    p_parser->idx++;

    if(p_parser->idx < PROTO_FILES_INFO_FRAME_LEN)
    {
        return PROTO_FILE_RET_OK; /* still accumulating */
    }

    // check file frame and crc code is valid
    uint8_t *data = p_parser->buf;
    PROTO_GET_ARR_PACK_TYPE(p_parser, data);
    uint8_t  file_pack_num    = data[2];
    uint8_t  file_pack_un_num = data[3];
    uint8_t *file_ptr         = NULL;
    uint8_t  file_size[FILE_SIZE_LENGTH];
    uint8_t  i           = 0;
    uint16_t packet_size = PACK_FRAME_SIZE_128B;
    uint32_t crc         = data[packet_size + PACKET_DATA_INDEX] << 8;
    crc += data[packet_size + PACKET_DATA_INDEX + 1];

    if(PACK_TYPE_128_FRAM != p_parser->pack_type || 0x5A != file_pack_num ||
       0xA5 != file_pack_un_num)
    {
        return PROTO_FILE_RET_PACKNUM_ERR;
    }
    if(cal_crc16(&data[PACKET_DATA_INDEX], packet_size) != crc)
    {
        return PROTO_FILE_RET_CRC_ERR;
    }

    file_ptr = data + PACKET_DATA_INDEX;
    while((*file_ptr != 0) && (i < FILE_NAME_LENGTH))
    {
        g_file_name[i++] = *file_ptr++;
    }

    g_file_name[i++] = '\0';
    i                = 0;
    file_ptr++;
    while((*file_ptr != ' ') && (i < FILE_SIZE_LENGTH))
    {
        file_size[i++] = *file_ptr++;
    }
    file_size[i++] = '\0';

    // Str2Int(file_size, &filesize);

    p_parser->idx   = 0U;
    p_parser->state = PROTO_FILE_RECEIVE_DATA_INFO;

    return PROTO_FILE_RET_OK;
}

/* Accumulate one data pack. Its total length isn't known until the very
 * first byte (pack_type) arrives, so that byte decides frame_len before
 * we know how many more bytes to wait for. */
static proto_files_ret_t proto_files_feed_data_info(
    proto_files_parser_t *p_parser, uint8_t byte, uint32_t *crc_ret)
{
    p_parser->buf[p_parser->idx] = byte;
    p_parser->idx++;
    if(1U == p_parser->idx)
    {
        p_parser->pack_type = (pack_type_frame_t)byte;
        p_parser->frame_len = PACKET_DATA_INDEX + 2U +
                              ((PACK_TYPE_128_FRAM == p_parser->pack_type)
                                   ? PACK_FRAME_SIZE_128B
                                   : PACK_FRAME_SIZE_1K);
    }

    if(p_parser->idx < p_parser->frame_len)
    {
        return PROTO_FILE_RET_OK; /* still accumulating */
    }

    uint8_t *data                    = p_parser->buf;
    uint8_t  master_send_pack_num    = data[2];
    uint8_t  master_send_pack_un_num = data[3];
    uint8_t  self_next_pack_num      = p_parser->package_number + 1;
    uint8_t  self_next_pack_un_num   = ~self_next_pack_num;
    uint16_t packet_size = (PACK_TYPE_128_FRAM == p_parser->pack_type)
                               ? PACK_FRAME_SIZE_128B
                               : PACK_FRAME_SIZE_1K;
    uint32_t crc         = 0U;

    if(master_send_pack_num != self_next_pack_num ||
       master_send_pack_un_num != self_next_pack_un_num)
    {
        return PROTO_FILE_RET_PACKNUM_ERR;
    }

    crc = data[packet_size + PACKET_DATA_INDEX] << 8;
    crc += data[packet_size + PACKET_DATA_INDEX + 1];
    if(cal_crc16(&data[PACKET_DATA_INDEX], packet_size) != crc)
    {
        *crc_ret = crc;
        return PROTO_FILE_RET_CRC_ERR;
    }

    PROTO_GET_ARR_RESERVER(p_parser, data);
    PROTO_GET_ARR_PACK_NUM(p_parser, data);
    PROTO_GET_ARR_PACK_UN_NUM(p_parser, data);

    p_parser->idx = 0U;
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
    p_parser->buf = g_file_buffer;
    return PROTO_FILE_RET_OK;
}

proto_files_ret_t
proto_files_feed(proto_files_parser_t *p_parser, uint8_t byte, uint32_t *crc)
{
    if(NULL == p_parser || NULL == crc)
    {
        return PROTO_FILE_RET_ERR;
    }
    proto_files_ret_t ret = PROTO_FILE_RET_OK;

    switch(p_parser->state)
    {
        case PROTO_FILE_IDLE:
        {
            ret = proto_files_feed_idle(p_parser, byte);
            break;
        }

        case PROTO_FILE_RECEIVE_FILE_INFO:
        {
            ret = proto_files_feed_file_info(p_parser, byte);
            break;
        }

        case PROTO_FILE_RECEIVE_DATA_INFO:
        {
            ret = proto_files_feed_data_info(p_parser, byte, crc);
            break;
        }

        default:
        {
            ret = PROTO_FILE_RET_ERR;
            break;
        }
    }

    if(PROTO_FILE_RET_OK != ret)
    {
        PROTO_CLEAR(p_parser);
    }
    return ret;
}

/* end of  file -------------------------------------------------------------*/
