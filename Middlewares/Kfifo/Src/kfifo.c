/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   kfifo.c
 * 
 *@pardependencies    :
 *                        kfifo.c
 *                        xxx.h   
 *                        xxxx.h
 * 
 *@author             :   null
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@Processingflow     :
 * 
 *@calldirectly       :
 * 
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!
 *                        global variable format 
 *                        uint8_t  g_xxxx_xxx = 0U;
 *                        uint8_t *p_xxxx_xxx = 0U;
 *                        
 *                        enumeration format 
 *                        typedef enum XXX_XXXX_T
 *                        {
 *                          XXXXX_XXXX              = 0x00U,
 *                        }xxxx_xxxx_t;
 *
 *                        struct format 
 *                        typedef struct XXXXXX_T
 *                        {
 *                          uint8_t xxxx_xxxx;
 *                          uint8_t (*pf_iic_init)(void*);
 *                        }xxxxx_t;
 *
 *                        macro definition format
 *                        #define XXXX_XXXX
 *                        #define XXXX_XXXX         (0U)
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <string.h>                               /* stdint lib header file. */
#include "kfifo.h"                                 /* kfifo lib header file. */
/* define   -----------------------------------------------------------------*/
// #define MIN(a, b) \
//     ({ typeof(a) _a = (a); typeof(b) _b = (b); _a < _b ? _a : _b; })
/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/
static uint8_t check_fifo_size_power_of_2(uint32_t size)
{
    return (size != 0) && ((size & (size - 1)) == 0);
}
/* Exported functions -------------------------------------------------------*/
/**
 * @brief            :  [kfifo_init]
 * @retval           :  [0 - success, 1 - error]
 * @param[in]        :  [kfifo_t* fifo,uint8_t* buffer,uint32_t size]
 */
uint8_t kfifo_init(kfifo_t* fifo,uint8_t* buffer,uint32_t size)
{
    if (!check_fifo_size_power_of_2(size))
    {
        return 1; /* Error: size is not a power of 2 */
    }
    fifo->buffer =   buffer;
    fifo->size   =     size;
    fifo->mask   = size - 1;
    fifo->in     =        0;
    fifo->out    =        0;
    return 0;
}

/**
 * @brief            :  [kfifo_put]
 * @retval           :  [number of bytes written]
 * @param[in]        :  [kfifo_t* fifo,const uint8_t* data,uint32_t len]
 */
uint32_t kfifo_put(kfifo_t* fifo,const uint8_t* data,uint32_t len)
{
    len = min_u32(len, kfifo_avail(fifo));
    uint32_t l = min_u32(len, kfifo_contig_write_space(fifo));
    memcpy(fifo->buffer + (fifo->in & fifo->mask), data, l);
    memcpy(fifo->buffer, data + l, len - l);
    fifo->in += len;
    return len;
}

/**
 * @brief            :  [kfifo_get]
 * @retval           :  [number of bytes read]
 * @param[in]        :  [kfifo_t* fifo,uint8_t* data,uint32_t len]
 */
uint32_t kfifo_get(kfifo_t* fifo,uint8_t* data,uint32_t len)
{
    
    len         = min_u32(len, kfifo_len(fifo));
    uint32_t l  = min_u32(len, kfifo_contig_read_data(fifo));

    memcpy(data, fifo->buffer + (fifo->out & fifo->mask), l);
    memcpy(data + l, fifo->buffer, len - l);

    fifo->out += len;
    return len;
}


/* end of  file -------------------------------------------------------------*/
