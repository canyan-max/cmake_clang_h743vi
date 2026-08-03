/**
 ******************************************************************************
 *@file               :   proto_simple.c
 *@brief              :   Minimal test framing protocol parser.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include "proto_simple.h"

/* exported functions -------------------------------------------------------*/

void proto_simple_init(proto_simple_parser_t *p_parser)
{
    if(NULL == p_parser)
    {
        return;
    }
    p_parser->state = PROTO_SIMPLE_ST_SOF;
}

/**
  * @brief            : [proto_simple_feed] feed one received byte into the
  *                     parser state machine.
  * @retval           : [1 - a full frame is ready in *p_out, 0 - not yet]
  * @param[in]        : [p_parser] parser instance
  * @param[in]        : [byte] next byte from the stream
  * @param[out]       : [p_out] filled with the parsed frame when return is 1
  */
uint8_t proto_simple_feed(proto_simple_parser_t *p_parser, uint8_t byte,
                           proto_simple_frame_t *p_out)
{
    if((NULL == p_parser) || (NULL == p_out))
    {
        return 0U;
    }

    switch(p_parser->state)
    {
        case PROTO_SIMPLE_ST_SOF:
            if(PROTO_SIMPLE_SOF == byte)
            {
                p_parser->state = PROTO_SIMPLE_ST_LEN;
            }
            break;

        case PROTO_SIMPLE_ST_LEN:
            if(byte > PROTO_SIMPLE_MAX_PAYLOAD)
            {
                p_parser->state = PROTO_SIMPLE_ST_SOF; /* bad length, resync */
            }
            else
            {
                p_parser->len      = byte;
                p_parser->idx      = 0U;
                p_parser->checksum = byte;
                p_parser->state    = (0U == byte) ? PROTO_SIMPLE_ST_CHECKSUM
                                                   : PROTO_SIMPLE_ST_PAYLOAD;
            }
            break;

        case PROTO_SIMPLE_ST_PAYLOAD:
            p_parser->payload[p_parser->idx] = byte;
            p_parser->checksum ^= byte;
            p_parser->idx++;
            if(p_parser->idx >= p_parser->len)
            {
                p_parser->state = PROTO_SIMPLE_ST_CHECKSUM;
            }
            break;

        case PROTO_SIMPLE_ST_CHECKSUM:
            p_parser->state = PROTO_SIMPLE_ST_SOF;
            if(byte == p_parser->checksum)
            {
                uint8_t i;
                p_out->len = p_parser->len;
                for(i = 0U; i < p_parser->len; i++)
                {
                    p_out->payload[i] = p_parser->payload[i];
                }
                return 1U;
            }
            break;

        default:
            p_parser->state = PROTO_SIMPLE_ST_SOF;
            break;
    }
    return 0U;
}

/* end of file --------------------------------------------------------------*/
