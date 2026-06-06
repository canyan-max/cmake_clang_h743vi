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
#include <stddef.h>                               /* stdint lib header file. */
#include "at24cxx_adapter.h"             /* at24cxx_adapter lib header file. */
#include "bsp_drv_at24.h"                        /* at24 driver header file. */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
extern const iic_ops_t g_at24c02_iic_ops;
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
uint8_t drv_write_page( void *p_drv, uint16_t mem_adr, \
                        uint8_t *p_data, uint16_t size, \
                        uint32_t timeout)
{
    at24_driver_t * p_at24 = (at24_driver_t * )p_drv;
    if(NULL == p_at24 || NULL == p_data || 0 == size)
    {
        return 1;
    }
    at24_state_t ret = AT24_OK;
    ret = p_at24->pf_write_page(p_at24, mem_adr, \
                                p_data, size, \
                                timeout);
    if(AT24_OK != ret)
    {

        return 2;
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
uint8_t drv_read_bytes( void *p_drv,     uint16_t mem_adr, \
                        uint8_t *p_data, uint16_t size, \
                        uint32_t timeout)
{

    if(NULL == p_drv || NULL == p_data || 0 == size)
    {
        return 1;
    }
    at24_driver_t * p_at24 = (at24_driver_t * )p_drv;
    at24_state_t ret = AT24_OK;
    ret = p_at24->pf_read_bytes(p_at24, mem_adr, \
                                p_data, size, timeout);
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
uint8_t drv_init(void *p_drv, \
                 uint32_t max_byte_addr, \
                 uint32_t page_size, \
                 uint16_t adr_size, \
                 uint8_t dev_adr)
{
    if(NULL == p_drv)
    {
        return 1;
    }
    at24_driver_t * p_at24 = (at24_driver_t * )p_drv;
    at24_state_t ret = AT24_OK;
    ret = at24_driver_instruct(p_at24, \
                     &g_at24c02_iic_ops, \
                     max_byte_addr, page_size,\
                     adr_size, \
                     dev_adr);
    if(AT24_OK != ret)
    {
        return 2;
    }
    return 0;
}
/* end of  file -------------------------------------------------------------*/
