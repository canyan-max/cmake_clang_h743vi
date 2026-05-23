/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_driver_led.c
 * 
 *@pardependencies    :
 *                        bsp_driver_led.c
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
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
/**
 * @brief            :  [bsp_driver_led_init]
                        initialize the led driver
 * @retval           :  [   LED_OK              = 0x00U,
                            LED_ERROR           = 0x01U,
                            LED_BUSY            = 0x02U,
                            LED_TIMEOUT         = 0x03U,
                            LED_INVALID_PARAM   = 0x04U,]
 * @param[in]        :  [led_driver_t   *p_drv , \
                         const led_hardware_t *p_hw , \
                         const led_operation_t *p_ops]
 */
led_driver_state_t bsp_driver_led_init(led_driver_t   *p_drv , \
                                       const led_hardware_t *p_hw , \
                                       const led_operation_t *p_ops)

{

    if((NULL == p_drv) || (NULL == p_hw) || (NULL == p_ops) || \
       (LED_DRIVER_IS_INIT == p_drv->is_init)
      )
    {
        return LED_INVALID_PARAM;
    }

    p_drv->p_led_hw  =               p_hw;
    p_drv->p_led_ops =              p_ops;
    p_drv->is_init   = LED_DRIVER_IS_INIT;
    return LED_OK;
}                                       
/* end of  file -------------------------------------------------------------*/
