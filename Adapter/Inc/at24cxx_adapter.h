/**
 ******************************************************************************
 *@file               :   at24cxx_adapter.h
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef AT24CXX_ADAPTER_H
#define AT24CXX_ADAPTER_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stdint lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
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
                 uint8_t  dev_adr);
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
                       uint32_t timeout);
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
                              uint32_t timeout);
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
                       uint32_t timeout);
#endif /* AT24CXX_ADAPTER_H */
