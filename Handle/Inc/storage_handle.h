/**
 ******************************************************************************
 *@file               :   storage_handle.h
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef STORAGE_HANDLE_H
#define STORAGE_HANDLE_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t (*pf_drv_init)(void    *p_drv,
                           uint32_t max_byte_addr,
                           uint32_t page_size,
                           uint16_t adr_size,
                           uint8_t  dev_adr);
    uint8_t (*pf_write_page)(void    *p_drv,
                             uint16_t mem_adr,
                             uint8_t *p_data,
                             uint16_t size,
                             uint32_t timeout);
    uint8_t (*pf_write_cross_page)(void    *p_drv,
                                   uint16_t mem_adr,
                                   uint8_t *p_data,
                                   uint16_t size,
                                   uint32_t timeout);
    uint8_t (*pf_read_bytes)(void    *p_drv,
                             uint16_t mem_adr,
                             uint8_t *p_data,
                             uint16_t size,
                             uint32_t timeout);
} storage_ops_t;

typedef struct
{
    const storage_ops_t *p_ops;
    void                *p_drv;
} storage_handle_t;
/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
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
uint8_t storage_handle_drv_init(storage_handle_t *p_handle,
                                uint32_t          max_byte_addr,
                                uint32_t          page_size,
                                uint16_t          adr_size,
                                uint8_t           dev_adr);
/**
 * @brief            :  [storage_handle_instruct]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [storage_handle_t *p_handle, \
                         const storage_ops_t *p_ops, \
                         void *p_drv]
 */
uint8_t storage_handle_write(storage_handle_t *p_handle,
                             uint16_t          mem_adr,
                             uint8_t          *p_data,
                             uint16_t          size,
                             uint32_t          timeout);
/**
 * @brief            :  [storage_handle_instruct]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [storage_handle_t *p_handle, \
                         const storage_ops_t *p_ops, \
                         void *p_drv]
 */
uint8_t storage_handle_write_cross_page(storage_handle_t *p_handle,
                                        uint16_t          mem_adr,
                                        uint8_t          *p_data,
                                        uint16_t          size,
                                        uint32_t          timeout);
/**
 * @brief            :  [storage_handle_instruct]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [storage_handle_t *p_handle, \
                         const storage_ops_t *p_ops, \
                         void *p_drv]
 */
uint8_t storage_handle_read(storage_handle_t *p_handle,
                            uint16_t          mem_adr,
                            uint8_t          *p_data,
                            uint16_t          size,
                            uint32_t          timeout);
/**
 * @brief            :  [storage_handle_instruct]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [storage_handle_t *p_handle, \
                                const storage_ops_t *p_ops, \
                                void *p_drv, \
                                uint32_t max_byte_addr, \
                                uint32_t page_size, \
                                uint16_t adr_size, \
                                uint8_t  dev_adr]
 */
uint8_t storage_handle_instruct(storage_handle_t    *p_handle,
                                const storage_ops_t *p_ops,
                                void                *p_drv,
                                uint32_t             max_byte_addr,
                                uint32_t             page_size,
                                uint16_t             adr_size,
                                uint8_t              dev_adr);
#endif /* STORAGE_HANDLE_H */
