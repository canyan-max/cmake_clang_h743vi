/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   kfifo.h
 * 
 *@pardependencies    :
 *                        kfifo.h
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
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef KFIFO_H
#define KFIFO_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */
#include <stddef.h>                               /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct KFIFO_T
{
    uint8_t*    buffer;
    uint32_t    size;  
    uint32_t    mask;  
    uint32_t    in;    
    uint32_t    out;   
}kfifo_t;
/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
 * @brief            :  [kfifo_init]
 * @retval           :  [0 - success, 1 - error]
 * @param[in]        :  [kfifo_t* fifo,uint8_t* buffer,uint32_t size]
 */
uint8_t kfifo_init(kfifo_t* fifo,uint8_t* buffer,uint32_t size);

/**
 * @brief            :  [kfifo_put]
 * @retval           :  [number of bytes written]
 * @param[in]        :  [kfifo_t* fifo,const uint8_t* data,uint32_t len]
 */
uint32_t kfifo_put(kfifo_t* fifo,const uint8_t* data,uint32_t len);

/**
 * @brief            :  [kfifo_get]
 * @retval           :  [number of bytes read]
 * @param[in]        :  [kfifo_t* fifo,uint8_t* data,uint32_t len]
 */
uint32_t kfifo_get(kfifo_t* fifo,uint8_t* data,uint32_t len);
/**
 * @brief            :  [kfifo_len]
 * @retval           :  [number of bytes in the fifo]
 * @param[in]        :  [kfifo_t* fifo]
 */
static inline uint32_t kfifo_len(const kfifo_t *fifo)
{
    return fifo->in - fifo->out;
}
/**
 * @brief            :  [kfifo_avail]
 * @retval           :  [number of bytes available in the fifo]
 * @param[in]        :  [kfifo_t* fifo]
 */
static inline uint32_t kfifo_avail(const kfifo_t *fifo)
{
    return fifo->size - kfifo_len(fifo);
}
/**
 * @brief            :  [kfifo_contig_write_space]
 * @retval           :  [number of bytes available for writing]
 * @param[in]        :  [kfifo_t* fifo]
 */
static inline uint32_t kfifo_contig_write_space(const kfifo_t *fifo)
{
    return fifo->size - (fifo->in & fifo->mask);
}
/**
 * @brief            :  [kfifo_contig_read_data]
 * @retval           :  [number of bytes available for reading]
 * @param[in]        :  [kfifo_t* fifo]
 */
static inline uint32_t kfifo_contig_read_data(const kfifo_t *fifo)
{
    return fifo->size - (fifo->out & fifo->mask);
}
/**
 * @brief            :  [min_u32]
 * @retval           :  [minimum of a and b]
 * @param[in]        :  [uint32_t a, uint32_t b]
 */
static inline uint32_t min_u32(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}
#endif /* KFIFO_H */
