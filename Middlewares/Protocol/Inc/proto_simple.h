/**
 ******************************************************************************
 *@file               :   proto_simple.h
 *@brief              :   Minimal test framing protocol: SOF + LEN + PAYLOAD +
 *                        XOR checksum. Pure byte-stream parser, no knowledge
 *                        of the transport it runs over.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef PROTO_SIMPLE_H
#define PROTO_SIMPLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/
#define PROTO_SIMPLE_SOF          (0xAAU)
#define PROTO_SIMPLE_MAX_PAYLOAD  (32U)

/* typedef ------------------------------------------------------------------*/
typedef enum
{
    PROTO_SIMPLE_ST_SOF = 0x00U,
    PROTO_SIMPLE_ST_LEN,
    PROTO_SIMPLE_ST_PAYLOAD,
    PROTO_SIMPLE_ST_CHECKSUM,
} proto_simple_state_t;

typedef struct
{
    uint8_t payload[PROTO_SIMPLE_MAX_PAYLOAD];
    uint8_t len;
} proto_simple_frame_t;

typedef struct
{
    proto_simple_state_t state;
    uint8_t               len;
    uint8_t               idx;
    uint8_t               checksum;
    uint8_t               payload[PROTO_SIMPLE_MAX_PAYLOAD];
} proto_simple_parser_t;

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
void    proto_simple_init(proto_simple_parser_t *p_parser);
uint8_t proto_simple_feed(proto_simple_parser_t *p_parser, uint8_t byte,
                           proto_simple_frame_t *p_out);

#ifdef __cplusplus
}
#endif

#endif /* PROTO_SIMPLE_H */
