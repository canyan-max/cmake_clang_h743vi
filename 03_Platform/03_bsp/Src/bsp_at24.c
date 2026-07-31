/**
 ******************************************************************************
 *@file               :   bsp_drv_at24.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>       /* stdint lib header file. */
#include "bsp_at24.h" /* bsp_drv_at24 lib header file. */

/* define   -----------------------------------------------------------------*/
#define AT24_DRIVER_IS_INITED      (0x01U)
#define AT24_DRIVER_NOT_INITED     (0x00U)

#define DRV_IS_NULL(p_drv)         (NULL == p_drv)

#define DRV_IS_NOT_INIT(p_drv)     \
        (AT24_DRIVER_NOT_INITED == p_drv->is_inited)

#define AT24_MEM_WRITE(p_drv, mem_adr, p_data, size, timeout) \
        ((p_drv)->iic_ops->pf_mem_write((p_drv)->iic_ops->p_iic_handle, \
                                        (p_drv)->dev_adr, \
                                        (mem_adr), \
                                        ((p_drv)->adr_size), \
                                        (p_data), \
                                        (size), \
                                        (timeout)))
#define AT24_MEM_READ(p_drv, mem_adr, p_data, size, timeout) \
        ((p_drv)->iic_ops->pf_mem_read((p_drv)->iic_ops->p_iic_handle, \
                                        ((p_drv)->dev_adr|0x01), \
                                        (mem_adr), \
                                        ((p_drv)->adr_size), \
                                        (p_data), \
                                        (size), \
                                        (timeout)))

#define AT24_DEBUG_LOG
#ifdef AT24_DEBUG_LOG
#include "log.h"
#define AT_LOG(fmt, ...)  \
        logFormat(INFO_TEXT, LOG_INFO, fmt, ##__VA_ARGS__)
#else
#define AT_LOG(fmt, ...)  \
        do{ } while(0)
#endif // end of AT24_DEBUG_LOG
/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* private  functions  ------------------------------------------------------*/
/**
  * @brief            :  [write_page]
  * @retval           :  [      AT24_OK        = 0x00U,
                                AT24_ERROR     = 0x01U,
                                AT24_BUSY      = 0x02U,
                                AT24_TIMEOUT   = 0x03U,
                                AT24_PARAM_ERR = 0x04U,]
  * @param[in]        :  [struct AT24_DRIVER_T *p_drv,
                          uint16_t mem_adr, \
                          uint8_t *p_data, uint16_t size, \
                          uint32_t timeout]
 */
at24_state_t at24_write_page(at24_dev_t *p_drv,
                               uint16_t       mem_adr,
                               uint8_t       *p_data,
                               uint16_t       size,
                               uint32_t       timeout)
{
    at24_state_t ret = AT24_OK;
    if(DRV_IS_NULL(p_drv) || DRV_IS_NOT_INIT(p_drv) || NULL == p_data)
    {
        return AT24_ERROR;
    }
    if(mem_adr >= p_drv->max_byte_addr ||
       (mem_adr + size) > p_drv->max_byte_addr || size > p_drv->page_size)
    {
        return AT24_PARAM_ERR;
    }
    ret = AT24_MEM_WRITE(p_drv, mem_adr, p_data, size, timeout);
    if(AT24_OK != ret)
    {
        return ret;
    }
    return AT24_OK;
}
/**
  * @brief            :  [read_page]
  * @retval           :  [      AT24_OK        = 0x00U,
                                AT24_ERROR     = 0x01U,
                                AT24_BUSY      = 0x02U,
                                AT24_TIMEOUT   = 0x03U,
                                AT24_PARAM_ERR = 0x04U,]
  * @param[in]        :  [     struct AT24_DRIVER_T *p_drv, uint16_t mem_adr, \
                               uint8_t *p_data, uint16_t size, \
                               uint32_t timeout]
 */
at24_state_t at24_read_bytes(at24_dev_t *p_drv,
                              uint16_t       mem_adr,
                              uint8_t       *p_data,
                              uint16_t       size,
                              uint32_t       timeout)
{
    at24_state_t ret = AT24_OK;
    if(DRV_IS_NULL(p_drv) || DRV_IS_NOT_INIT(p_drv) || NULL == p_data)
    {
        return AT24_ERROR;
    }

    if(mem_adr >= p_drv->max_byte_addr ||
       ((uint32_t)mem_adr + (uint32_t)size) > p_drv->max_byte_addr)
    {
        return AT24_PARAM_ERR;
    }

    ret = AT24_MEM_READ(p_drv, mem_adr, p_data, size, timeout);
    if(AT24_OK != ret)
    {
        return ret;
    }
    return AT24_OK;
}
/**
  * @brief            :  [write_byte]
  * @retval           :  [      AT24_OK        = 0x00U,
                                AT24_ERROR     = 0x01U,
                                AT24_BUSY      = 0x02U,
                                AT24_TIMEOUT   = 0x03U,
                                AT24_PARAM_ERR = 0x04U,]
  * @param[in]        :  [    struct AT24_DRIVER_T *p_drv, \
                              uint16_t mem_adr, \
                              uint8_t  data, \
                              uint32_t timeout]
 */
at24_state_t at24_write_byte(at24_dev_t *p_drv,
                               uint16_t       mem_adr,
                               uint8_t        data,
                               uint32_t       timeout)
{
    at24_state_t ret = AT24_OK;
    if(DRV_IS_NULL(p_drv) || DRV_IS_NOT_INIT(p_drv))
    {
        return AT24_ERROR;
    }
    if(mem_adr >= p_drv->max_byte_addr ||
       ((uint32_t)mem_adr + 1U) > p_drv->max_byte_addr)
    {
        return AT24_PARAM_ERR;
    }
    ret = AT24_MEM_WRITE(p_drv, mem_adr, &data, 1, timeout);
    if(AT24_OK != ret)
    {
        return ret;
    }
    return AT24_OK;
}

/* exported functions -------------------------------------------------------*/
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
 * @param[in]        :  [at24_dev_t *p_driver , \
                         iic_ops_t *iic_ops , \
                         uint32_t max_byte_addr , \
                         uint32_t page_size , \
                         uint16_t adr_size , \
                         uint8_t  dev_adr)]
 */
/* ---- board-level init ---------------------------------------------------- */

extern const iic_ops_t g_at24c02_iic_ops;

at24_state_t bsp_at24_init(at24_dev_t *p_drv,
                             uint32_t       max_byte_addr,
                             uint32_t       page_size,
                             uint16_t       adr_size,
                             uint8_t        dev_adr)
{
    const iic_ops_t *ops = &g_at24c02_iic_ops;
    at24_state_t ret = AT24_OK;

    if (NULL == p_drv) { return AT24_ERROR; }
    if (!DRV_IS_NOT_INIT(p_drv)) { return AT24_OK; }

#ifdef AT24_DEBUG_LOG
    AT_LOG("AT24 driver initialized start.\n");
#endif

    p_drv->iic_ops = ops;

    if (NULL != ops->pf_iic_init)
    {
        ret = ops->pf_iic_init(ops->p_iic_handle);
        if (AT24_OK != ret)
        {
#ifdef AT24_DEBUG_LOG
            AT_LOG("AT24 driver iic init err.\n");
#endif
            p_drv->iic_ops = NULL;
            return ret;
        }
    }

    if (NULL != ops->pf_iic_dev_isready)
    {
        ret = ops->pf_iic_dev_isready(ops->p_iic_handle, dev_adr, 1, 100);
        if (AT24_OK != ret)
        {
#ifdef AT24_DEBUG_LOG
            AT_LOG("AT24 driver iic dev is ready err.\n");
#endif
            p_drv->iic_ops = NULL;
            return ret;
        }
    }

    p_drv->max_byte_addr = max_byte_addr;
    p_drv->page_size     = page_size;
    p_drv->dev_adr       = dev_adr;
    p_drv->adr_size      = adr_size;
    p_drv->is_inited     = AT24_DRIVER_IS_INITED;

#ifdef AT24_DEBUG_LOG
    AT_LOG("AT24 driver initialized successfully.\n");
    AT_LOG("dev=%d", p_drv->dev_adr);
#endif
    return AT24_OK;
}

/* end of  file -------------------------------------------------------------*/
