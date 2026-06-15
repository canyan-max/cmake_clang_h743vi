/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_drv_led.c
 * 
 *@pardependencies    :
 *                        bsp_drv_led.c
 *                        xxx.h   
 *                        xxxx.h
 * 
 *@author             :   null
 * 
 *@brief              :   Provide the HAL APIs of led drivers .
 * 
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>                               /* stddef lib header file. */
#include "bsp_drv_led.h"                     /* bsp_drv_led lib header file. */
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
                         const led_operation_t *p_ops]
 */
led_driver_state_t bsp_driver_led_init(led_driver_t   *p_drv , \
                                       const led_operation_t *p_ops)
{

    if((NULL == p_drv) || (NULL == p_ops) || \
       (LED_DRIVER_IS_INIT == p_drv->is_init)
      )
    {
        return LED_INVALID_PARAM;
    }
    p_drv->p_led_ops =              p_ops;
    p_drv->is_init   = LED_DRIVER_IS_INIT;
    return LED_OK;
}                                       
/* end of  file -------------------------------------------------------------*/
