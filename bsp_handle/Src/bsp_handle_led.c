/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_handle_led.c
 * 
 *@pardependencies    :
 *                        bsp_handle_led.c
 *                        xxx.h   
 *                        xxxx.h
 * 
 *@author             :   null
 * 
 *@brief              :   Provide the HAL APIs of led drivers .
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
#include <stddef.h>                               /* stdint lib header file. */
#include "bsp_driver_led.h"               /* bsp_driver_led lib header file. */
#include "bsp_handle_led.h"               /* bsp_handle_led lib header file. */
/* define   -----------------------------------------------------------------*/
#define LED_HANDLE_IS_INIT            (0x01U)
#define LED_HANDLE_NOT_INIT           (0x00U)
#define LED_DECLARE_DRV(drv, index) \
            led_driver_t *drv = (led_driver_t *)(g_led_handle.p_led_drv[index])
#define LED_DRV_IS_NULL(index) \
    (NULL == g_led_handle.p_led_drv[index])

#define LED_HANDLE_IS_NOT_INIT \
    (LED_HANDLE_NOT_INIT == g_led_handle_init)

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static uint8_t g_led_drv_count = 0U;            // Led handle is inited or not.
static led_handle_t g_led_handle = {0U};                  // Led handle struct.
// Led handle is inited or not.
static uint8_t g_led_handle_init = LED_HANDLE_NOT_INIT;   
/* Private  functions  ------------------------------------------------------*/
/**
  * @brief            :  [led_handle_init]
            init the handle struct,set the handle struct to default value.    
 */
static void led_handle_init(void)
{
    for(uint8_t i = 0U; i < LED_INDEX_MAX; i++)
    {
        g_led_handle.p_led_drv[i] = NULL;
    }
    g_led_handle_init = LED_HANDLE_IS_INIT;
}
/* Exported functions -------------------------------------------------------*/
/**
  * @brief            :  [led_handle_on]
  * @retval           :  [ LED_HANDLE_OK              = 0x00U,    // Operation is OK.
                           LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
                           LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
                           LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
                           LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.]
  * @param[in]        :  [led_handle_devindex_t index]
 */
led_handle_state_t led_handle_on(led_handle_devindex_t index)
{
    if( LED_HANDLE_IS_NOT_INIT || \
        index >= LED_INDEX_MAX || \
        LED_DRV_IS_NULL(index) )
    { 
        return LED_HANDLE_INVALID_PARAM;
    }
    LED_DECLARE_DRV(p_drv, index);
    if (p_drv != NULL)
    {
        p_drv->p_led_ops->pf_led_on(p_drv);
    }
    return LED_HANDLE_OK;
}
/**
  * @brief            :  [led_handle_off]
  * @retval           :  [ LED_HANDLE_OK              = 0x00U,    // Operation is OK.
                           LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
                           LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
                           LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
                           LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.]
  * @param[in]        :  [led_handle_devindex_t index]
 */
led_handle_state_t led_handle_off(led_handle_devindex_t index)
{
    if( LED_HANDLE_IS_NOT_INIT || \
        index >= LED_INDEX_MAX || \
        LED_DRV_IS_NULL(index) )
    { 
        return LED_HANDLE_INVALID_PARAM;
    }
    
    LED_DECLARE_DRV(p_drv, index);
    p_drv->p_led_ops->pf_led_off(p_drv);
    
    return LED_HANDLE_OK;
}
/**
  * @brief            :  [led_handle_blink]
  * @retval           :  [ LED_HANDLE_OK              = 0x00U,    // Operation is OK.
                           LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
                           LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
                           LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
                           LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.]
  * @param[in]        :  [led_handle_devindex_t index]
 */
led_handle_state_t led_handle_blink(led_handle_devindex_t index)
{
    if( LED_HANDLE_IS_NOT_INIT || \
        index >= LED_INDEX_MAX || \
        LED_DRV_IS_NULL(index) )
    { 
        return LED_HANDLE_INVALID_PARAM;
    }
    LED_DECLARE_DRV(p_drv, index);
    if (p_drv != NULL)
    {
        p_drv->p_led_ops->pf_led_blink(p_drv);
    }
    return LED_HANDLE_OK;
}
/**
  * @brief            :  [led_handle_instuct]
  * @retval           :  [ LED_HANDLE_OK              = 0x00U,    // Operation is OK.
                           LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
                           LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
                           LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
                           LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.]
  * @param[in]        :  [led_handle_t *p_handle , \
                          void * p_drv]
 */
led_handle_state_t led_handle_instuct(led_handle_t *p_handle , \
                                      void * p_drv)
{

    if( NULL == p_drv || g_led_drv_count >= LED_INDEX_MAX )
    {
        return LED_HANDLE_INVALID_PARAM;
    }

    if(LED_HANDLE_IS_NOT_INIT)
    {
        led_handle_init();
    }

    p_handle->p_led_drv[g_led_drv_count] = p_drv;
    g_led_drv_count+=1;
    return LED_HANDLE_OK;
}
/* end of  file -------------------------------------------------------------*/
