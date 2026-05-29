/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_driver_at24.h
 * 
 *@pardependencies    :
 *                        bsp_driver_at24.h
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
#ifndef BSP_DRIVER_AT24_H
#define BSP_DRIVER_AT24_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */
#include <stddef.h>                               /* stdint lib header file. */

/* define -------------------------------------------------------------------*/
#define AT24_MEMADD_SIZE_8BIT            (0x00000001U)
#define AT24_MEMADD_SIZE_16BIT           (0x00000002U)
/* typedef ------------------------------------------------------------------*/
typedef enum 
{
    AT24_OK        = 0x00U,
    AT24_ERROR     = 0x01U,
    AT24_BUSY      = 0x02U,
    AT24_TIMEOUT   = 0x03U,
    AT24_PARAM_ERR = 0x04U,

} at24_state_t;

typedef struct HAL_IIC_OPS_T
{
    void     *p_iic_handle;
    at24_state_t (*pf_iic_init)        (void*);
    at24_state_t (*pf_iic_deinit)      (void*);
    at24_state_t (*pf_iic_dev_isready) (void*hi2c, \
                                        uint8_t dev_adr, \
                                        uint32_t tails, \
                                        uint32_t timeout);
    at24_state_t (*pf_mem_read)(void*hi2c, uint16_t dev_adr , \
                                uint16_t mem_adr, uint16_t mem_adr_size, \
                                uint8_t *p_data, uint16_t size, \
                                uint32_t timeout);
    at24_state_t (*pf_mem_write)(void*hi2c, uint16_t dev_adr , \
                                uint16_t mem_adr, uint16_t mem_adr_size, \
                                uint8_t *p_data, uint16_t size, \
                                uint32_t timeout);
}iic_ops_t;

typedef struct AT24_DRIVER_T
{
    // iic ops
    const iic_ops_t *iic_ops;
    // function pointer
    at24_state_t (*pf_write_page)(struct AT24_DRIVER_T *p_drv, uint16_t mem_adr, \
                                  uint8_t *p_data, uint16_t size, \
                                  uint32_t timeout);
    at24_state_t (*pf_read_bytes)(struct AT24_DRIVER_T *p_drv, uint16_t mem_adr, \
                                 uint8_t *p_data, uint16_t size, \
                                 uint32_t timeout);
    at24_state_t (*pf_write_byte)(struct AT24_DRIVER_T *p_drv, uint16_t mem_adr, \
                                  uint8_t data,uint32_t timeout);
    uint32_t max_byte_addr;
    uint32_t page_size;
    uint16_t adr_size;
    uint8_t  dev_adr;
    uint8_t  is_inited;
}at24_driver_t;
/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
 * @brief            :  [at24_driver_instruct 
                         init the at24 driver and iic ops.
                         if the iic ops is not null. 
                         then call the iic init function]
 * @retval           :  [   AT24_OK        = 0x00U,
                            AT24_ERROR     = 0x01U,
                            AT24_BUSY      = 0x02U,
                            AT24_TIMEOUT   = 0x03U,
                            AT24_PARAM_ERR = 0x04U,]
 * @param[in]        :  [at24_driver_t *p_drv , \
                         const iic_ops_t *p_iic_ops , \
                         uint32_t max_byte_addr    , \
                         uint32_t page_size   , \
                         uint16_t adr_size 
                         AT24_MEMADD_SIZE_8BIT
                         AT24_MEMADD_SIZE_16BIT , \

                         uint8_t  dev_adr]
 */
at24_state_t at24_driver_instruct(at24_driver_t *p_drv , \
                                  const iic_ops_t *p_iic_ops , \
                                  uint32_t max_byte_addr    , \
                                  uint32_t page_size   , \
                                  uint16_t adr_size , \
                                  uint8_t  dev_adr);
#endif /* BSP_DRIVER_AT24_H */
