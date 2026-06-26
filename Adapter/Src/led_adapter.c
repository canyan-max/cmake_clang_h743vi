/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   led_adapter.c
 *
 *@pardependencies    :   led_adapter.c
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
#include <stddef.h>      /* stdint lib header file. */
#include "led_adapter.h" /* led_adapter lib header file. */
#include "bsp_drv_led.h" /* bsp_drv_led lib header file. */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* private  functions  ------------------------------------------------------*/

/* exported functions -------------------------------------------------------*/
/**
 * @brief            :  [drv_led_on
                        drv open the led operation ]
 * @retval           :  [0 successfully]
 * @param[in]        :  [p_drv]
 */
uint8_t drv_led_on(void *p_drv)
{
    if(NULL == p_drv)
    {
        return 1;
    }

    led_driver_state_t ret   = LED_OK;
    led_driver_t      *p_led = (led_driver_t *)p_drv;

    ret = p_led->p_led_ops->pf_led_on(p_led);
    if(LED_OK != ret)
    {
        return 2;
    }

    return 0;
}
/**
 * @brief            :  [drv_led_off
                        drv close the led operation ]
 * @retval           :  [0 successfully]
 * @param[in]        :  [p_drv]
 */
uint8_t drv_led_off(void *p_drv)
{
    if(NULL == p_drv)
    {
        return 1;
    }

    led_driver_state_t ret   = LED_OK;
    led_driver_t      *p_led = (led_driver_t *)p_drv;

    ret = p_led->p_led_ops->pf_led_off(p_led);
    if(LED_OK != ret)
    {
        return 2;
    }

    return 0;
}
/**
 * @brief            :  [drv_led_blink
                        drv blink the led operation ]
 * @retval           :  [0 successfully]
 * @param[in]        :  [p_drv]
 */
uint8_t drv_led_blink(void *p_drv)
{
    if(NULL == p_drv)
    {
        return 1;
    }

    led_driver_state_t ret   = LED_OK;
    led_driver_t      *p_led = (led_driver_t *)p_drv;

    ret = p_led->p_led_ops->pf_led_blink(p_led);
    if(LED_OK != ret)
    {
        return 2;
    }

    return 0;
}
/**
 * @brief            :  [drv_led_init
                        drv init the led operation ]
 * @retval           :  [0 successfully]
 * @param[in]        :  [p_drv]
 */
uint8_t drv_led_init(void *p_drv, void *drv_ops)
{
    if(NULL == p_drv || NULL == drv_ops)
    {
        return 1;
    }

    led_driver_state_t ret = LED_OK;

    ret = bsp_driver_led_init((led_driver_t *)p_drv,
                              (const led_operation_t *)drv_ops);
    if(LED_OK != ret)
    {
        return 2;
    }

    return 0;
}
/* end of file --------------------------------------------------------------*/
