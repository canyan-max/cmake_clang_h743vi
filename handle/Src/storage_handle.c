/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   storage_handle.c
 * 
 *@pardependencies    :   storage_handle.c
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
#include "storage_handle.h"                      /* storage_lib header file. */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
/**
 * @brief            :  [storage_handle_instruct]
 * @retval           :  [0 successful, 
                         1 invalid parameter
                        ]
 * @param[in]        :  [storage_handle_t *p_handle, \
                                uint32_t max_byte_addr, \
                                uint32_t page_size, \
                                uint16_t adr_size, \
                                uint8_t dev_adr]
 */
uint8_t storage_handle_drv_init(storage_handle_t *p_handle, \
                                uint32_t max_byte_addr, \
                                uint32_t page_size, \
                                uint16_t adr_size, \
                                uint8_t dev_adr)
{
    if(NULL == p_handle || NULL == p_handle->p_ops || \
       NULL == p_handle->p_ops->pf_drv_init)
    {
        return 1;
    }
    return p_handle->p_ops->pf_drv_init(p_handle->p_drv, \
                                        max_byte_addr, \
                                        page_size, \
                                        adr_size, \
                                        dev_adr);
}

/**
 * @brief            :  [storage_handle_instruct]
 * @retval           :  [0 successful, 
                         1 invalid parameter
                        ]
 * @param[in]        :  [storage_handle_t *p_handle, \
                         const storage_ops_t *p_ops, \
                         void *p_drv]
 */
uint8_t storage_handle_write(storage_handle_t *p_handle, \
                            uint16_t mem_adr, \
                            uint8_t *p_data, uint16_t size, \
                            uint32_t timeout)
{
    if(NULL == p_handle || NULL == p_data || 0 == size || \
       NULL == p_handle->p_ops || \
       NULL == p_handle->p_ops->pf_write_page)
    {
        return 1;
    }
    return p_handle->p_ops->pf_write_page(p_handle->p_drv, mem_adr, \
                                p_data, size, \
                                timeout);
                
}

/**
 * @brief            :  [storage_handle_instruct]
 * @retval           :  [0 successful, 
                         1 invalid parameter
                        ]
 * @param[in]        :  [storage_handle_t *p_handle, \
                         const storage_ops_t *p_ops, \
                         void *p_drv]
 */
uint8_t storage_handle_read(storage_handle_t *p_handle, \
                            uint16_t mem_adr, \
                            uint8_t *p_data, uint16_t size, \
                            uint32_t timeout)
{
    if(NULL == p_handle || NULL == p_data || 0 == size || \
       NULL == p_handle->p_ops || \
       NULL == p_handle->p_ops->pf_read_bytes)
    {
        return 1;
    }
    return p_handle->p_ops->pf_read_bytes(p_handle->p_drv, mem_adr, \
                                p_data, size, \
                                timeout);
}

/**
 * @brief            :  [storage_handle_instruct]
 * @retval           :  [0 successful, 
                         1 invalid parameter
                        ]
 * @param[in]        :  [storage_handle_t *p_handle, \
                         const storage_ops_t *p_ops, \
                         void *p_drv]
 */
uint8_t storage_handle_instruct(storage_handle_t *p_handle, \
                                const storage_ops_t *p_ops, \
                                void *p_drv)
{
    if(NULL == p_handle || NULL == p_drv || NULL == p_ops)
    {
        return 1;
    }
    p_handle->p_ops = p_ops;
    p_handle->p_drv = p_drv;
    return 0;
}
/* end of  file -------------------------------------------------------------*/
