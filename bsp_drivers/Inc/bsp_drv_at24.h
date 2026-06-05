/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_drv_at24.h
 *
 *@pardependencies    :
 *                        bsp_drv_at24.h
 *                        st_iic.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of AT24Cxx EEPROM driver.
 *
 *@Processingflow     :
 *
 *@calldirectly       :
 *
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef BSP_DRV_AT24_H
#define BSP_DRV_AT24_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */
#include <stddef.h>                               /* stddef lib header file. */

/* define -------------------------------------------------------------------*/
#define AT24_MEMADD_SIZE_8BIT  (1U)              // 8-bit memory address size.
#define AT24_MEMADD_SIZE_16BIT (2U)              // 16-bit memory address size.
/* typedef ------------------------------------------------------------------*/
typedef struct AT24_DRIVER_T at24_driver_t;

typedef enum AT24_STATE_T
{
    AT24_OK              = 0x00U,    // Operation is OK.
    AT24_ERROR           = 0x01U,    // Operation is error.
    AT24_BUSY            = 0x02U,    // Operation is busy.
    AT24_TIMEOUT         = 0x03U,    // Operation is timeout.
    AT24_PARAM_ERR       = 0x04U,    // Operation is invalid parameter.
} at24_state_t;

typedef struct IIC_OPS_T
{
    void          *p_iic_handle;                                             // I2C HAL handle pointer.
    at24_state_t (*pf_iic_init)       (void *p_handle);                      // I2C init function pointer.
    at24_state_t (*pf_iic_deinit)     (void *p_handle);                      // I2C deinit function pointer.
    at24_state_t (*pf_iic_dev_isready)(void *p_handle,                       // I2C device ready check.
                                        uint8_t  dev_adr,
                                        uint32_t trials,
                                        uint32_t timeout);
    at24_state_t (*pf_mem_read)       (void *p_handle,                       // I2C memory read.
                                        uint16_t dev_adr,
                                        uint16_t mem_adr,
                                        uint16_t mem_adr_size,
                                        uint8_t *p_data,
                                        uint16_t size,
                                        uint32_t timeout);
    at24_state_t (*pf_mem_write)      (void *p_handle,                       // I2C memory write.
                                        uint16_t dev_adr,
                                        uint16_t mem_adr,
                                        uint16_t mem_adr_size,
                                        uint8_t *p_data,
                                        uint16_t size,
                                        uint32_t timeout);
} iic_ops_t;

typedef struct AT24_DRIVER_T
{
    const iic_ops_t *iic_ops;                                               // I2C bus operations.
    at24_state_t (*pf_write_page)(at24_driver_t *p_drv,                      // Write page function pointer.
                                   uint16_t mem_adr,
                                   uint8_t *p_data,
                                   uint16_t size,
                                   uint32_t timeout);
    at24_state_t (*pf_read_bytes)(at24_driver_t *p_drv,                      // Read bytes function pointer.
                                   uint16_t mem_adr,
                                   uint8_t *p_data,
                                   uint16_t size,
                                   uint32_t timeout);
    at24_state_t (*pf_write_byte)(at24_driver_t *p_drv,                      // Write single byte function pointer.
                                   uint16_t mem_adr,
                                   uint8_t  data,
                                   uint32_t timeout);
    uint32_t max_byte_addr;                                                  // Maximum byte address.
    uint32_t page_size;                                                      // Page size in bytes.
    uint16_t adr_size;                                                       // Memory address width (1 or 2 bytes).
    uint8_t  dev_adr;                                                        // I2C device address (8-bit, write form).
    uint8_t  is_inited;                                                      // Driver init flag.
} at24_driver_t;

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
 * @brief            :  [at24_driver_instruct]
                        Initialize the AT24 driver with I2C bus ops and device parameters.
                        If iic_ops->pf_iic_init is not NULL, it will be called first.
                        If iic_ops->pf_iic_dev_isready is not NULL, device readiness is checked.
                        On success, binds read/write function pointers to the driver instance.
 * @retval           :  [   AT24_OK              = 0x00U,
                            AT24_ERROR           = 0x01U,
                            AT24_BUSY            = 0x02U,
                            AT24_TIMEOUT         = 0x03U,
                            AT24_PARAM_ERR       = 0x04U,]
 * @param[in]        :  [at24_driver_t   *p_drv          , \
                         const iic_ops_t *p_iic_ops      , \
                         uint32_t         max_byte_addr  , \
                         uint32_t         page_size      , \
                         uint16_t         adr_size       , \
                         uint8_t          dev_adr        ]
 */
at24_state_t at24_driver_instruct(at24_driver_t   *p_drv,
                                  const iic_ops_t *p_iic_ops,
                                  uint32_t         max_byte_addr,
                                  uint32_t         page_size,
                                  uint16_t         adr_size,
                                  uint8_t          dev_adr);

#endif /* BSP_DRV_AT24_H */
