/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   at24cxx_adapter.c
 *
 *@pardependencies    :   at24cxx_adapter.c
 *                        xxxx.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of description.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>          /* stdint lib header file. */
#include "at24cxx_adapter.h" /* at24cxx_adapter lib header file. */
#include "bsp_drv_at24.h"    /* at24 driver header file. */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
extern const iic_ops_t g_at24c02_iic_ops;
extern void            HAL_Delay(uint32_t Delay);
extern uint32_t        HAL_GetTick(void);
/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
/**
 * @brief            :  [drv_write_page]
 * @retval           :  [0 : success,
                         1 : invalid parameter,
                         2 : write failed
                        ]
 * @param[in]        :  [void *p_drv, uint16_t mem_adr, \
                        uint8_t *p_data, uint16_t size, \
                        uint32_t timeout]
 */
uint8_t drv_write_page(void    *p_drv,
                       uint16_t mem_adr,
                       uint8_t *p_data,
                       uint16_t size,
                       uint32_t timeout)
{
    at24_driver_t *p_at24 = (at24_driver_t *)p_drv;
    if(NULL == p_at24 || NULL == p_data || 0 == size)
    {
        return 1;
    }
    at24_state_t ret = AT24_OK;
    ret = p_at24->pf_write_page(p_at24, mem_adr, p_data, size, timeout);
    if(AT24_OK != ret)
    {
        return 2;
    }
    return 0;
}
/**
 * @brief            :  [drv_write_bytes_cross]
 * @retval           :  [0 : success,
                         1 : invalid parameter,
                         2 : write failed
                        ]
 * @param[in]        :  [void *p_drv, uint16_t mem_adr, \
                        uint8_t *p_data, uint16_t size, \
                        uint32_t timeout]
 */
uint8_t drv_write_bytes_cross(void    *p_drv,
                              uint16_t mem_adr,
                              uint8_t *p_data,
                              uint16_t size,
                              uint32_t timeout)
{
    at24_driver_t *p_at24      = (at24_driver_t *)p_drv;
    uint16_t       left_size   = size;
    uint16_t       current_adr = mem_adr;
    uint8_t       *p_cursor    = p_data;
    at24_state_t   ret         = AT24_OK;

    if(NULL == p_at24 || NULL == p_data || 0U == size)
    {
        return 1;
    }

    if((mem_adr >= p_at24->max_byte_addr) ||
       ((uint32_t)mem_adr + (uint32_t)size) > p_at24->max_byte_addr)
    {
        return 1;
    }

    while(0U != left_size)
    {
        uint16_t page_remain = p_at24->page_size;
        uint16_t chunk_size  = 0U;
        page_remain -= (uint16_t)(current_adr % p_at24->page_size);
        chunk_size = (left_size < page_remain) ? left_size : page_remain;

        ret = p_at24->pf_write_page(p_at24, current_adr, p_cursor, chunk_size,
                                    timeout);
        if(AT24_OK != ret)
        {
            return 2;
        }
        current_adr += chunk_size;
        p_cursor += chunk_size;
        left_size -= chunk_size;
        if(0U == left_size)
        {
            break;
        }
        uint32_t start        = HAL_GetTick();
        uint8_t  timeout_flag = 1U;
        while((HAL_GetTick() - start) <= 5U)
        {
            if(AT24_OK ==
               p_at24->iic_ops
                   ->pf_iic_dev_isready(p_at24->iic_ops->p_iic_handle,
                                        p_at24->dev_adr, 1U, 0U))
            {
                timeout_flag = 0U;
                break;
            }
        }
        if(timeout_flag)
        {
            return 3;
        }
    }
    return 0;
}
/**
 * @brief            :  [drv_read_bytes]
 * @retval           :  [0 : success,
                         1 : invalid parameter,
                         2 : read failed
                        ]
 * @param[in]        :  [void *p_drv, uint16_t mem_adr, \
                        uint8_t *p_data, uint16_t size, \
                        uint32_t timeout]
 */
uint8_t drv_read_bytes(void    *p_drv,
                       uint16_t mem_adr,
                       uint8_t *p_data,
                       uint16_t size,
                       uint32_t timeout)
{
    at24_driver_t *p_at24 = (at24_driver_t *)p_drv;
    at24_state_t   ret    = AT24_OK;
    if(NULL == p_drv || NULL == p_data || 0 == size)
    {
        return 1;
    }

    ret = p_at24->pf_read_bytes(p_at24, mem_adr, p_data, size, timeout);
    if(AT24_OK != ret)
    {

        return 2;
    }
    return 0;
}
/**
 * @brief            :  [drv_init]
 * @retval           :  [0 : success,
                         1 : invalid parameter,
                         2 : init failed
                        ]
 * @param[in]        :  [void *p_drv, uint32_t max_byte_addr, \
                        uint32_t page_size, \
                        uint16_t adr_size, \
                        uint8_t dev_adr]
 */
uint8_t drv_init(void    *p_drv,
                 uint32_t max_byte_addr,
                 uint32_t page_size,
                 uint16_t adr_size,
                 uint8_t  dev_adr)
{
    if(NULL == p_drv)
    {
        return 1;
    }
    at24_driver_t *p_at24 = (at24_driver_t *)p_drv;
    at24_state_t   ret    = AT24_OK;
    ret = at24_driver_instruct(p_at24, &g_at24c02_iic_ops, max_byte_addr,
                               page_size, adr_size, dev_adr);
    if(AT24_OK != ret)
    {
        return 2;
    }
    return 0;
}
/* end of  file -------------------------------------------------------------*/
