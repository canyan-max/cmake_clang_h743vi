/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   st_led.c
 * 
 *@pardependencies    :
 *                        st_led.c
 *                        bsp_drv_led.h   
 *                        gpio.h
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
#include "st_led.h"                               /* st_led lib header file. */
#include "bsp_drv_led.h"               /* bsp_drv_led lib header file. */
#include "gpio.h"                                   /* gpio lib header file. */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
static led_driver_state_t st_led_init   (led_driver_t*self);
static led_driver_state_t st_led_on     (led_driver_t*self);
static led_driver_state_t st_led_off    (led_driver_t*self);
static led_driver_state_t st_led_blink  (led_driver_t*self);
static led_driver_state_t st_led_deinit (led_driver_t*self);
const led_operation_t g_led1_ops = 
{
    .p_port         = (void*)LED1_GPIO_Port,
    .pin            =              LED1_Pin,
    .led_on_level   =     LED_ON_LEVEL_HIGH,
    .pf_led_init    =           st_led_init,
    .pf_led_on      =             st_led_on,
    .pf_led_off     =            st_led_off,
    .pf_led_blink   =          st_led_blink,
    .pf_led_deinit  =         st_led_deinit
};
const led_operation_t g_led2_ops = 
{
    .p_port         = (void*)LED2_GPIO_Port,
    .pin            =              LED2_Pin,
    .led_on_level   =     LED_ON_LEVEL_HIGH,
    .pf_led_init    =           st_led_init,
    .pf_led_on      =             st_led_on,
    .pf_led_off     =            st_led_off,
    .pf_led_blink   =          st_led_blink,
    .pf_led_deinit  =         st_led_deinit
};
/* Private  functions  ------------------------------------------------------*/
/**
  * @brief            :  [st_led_init]
                         initialize the led hardware
  * @retval           :  [LED_OK              = 0x00U,
                          LED_ERROR           = 0x01U,
                          LED_BUSY            = 0x02U,
                          LED_TIMEOUT         = 0x03U,
                          LED_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [led_driver_t*self]
 */
static led_driver_state_t st_led_init(led_driver_t*self)
{
    /* led init code. */
    if(NULL == self || LED_DRIVER_IS_INIT == self->is_init)
    {
        return LED_ERROR;
    }

    return LED_OK;
}
/**
  * @brief            :  [st_led_init]
                         initialize the led hardware
  * @retval           :  [LED_OK              = 0x00U,
                          LED_ERROR           = 0x01U,
                          LED_BUSY            = 0x02U,
                          LED_TIMEOUT         = 0x03U,
                          LED_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [led_driver_t*self]
 */
static led_driver_state_t st_led_on(led_driver_t*self)
{
    /* led on code. */
    // check parameter. 
    if(NULL == self || LED_DRIVER_NOT_INIT == self->is_init)
    {
        return LED_ERROR;
    }
    // led on operation.
    GPIO_TypeDef *p_hw = (GPIO_TypeDef*)(self->p_led_ops->p_port);
    uint16_t pin = self->p_led_ops->pin;
    GPIO_PinState pin_state = GPIO_PIN_RESET;
    if(LED_ON_LEVEL_HIGH == self->p_led_ops->led_on_level )
    {
        pin_state = GPIO_PIN_SET;
    }
    HAL_GPIO_WritePin(p_hw, pin, pin_state);
    return LED_OK;
}
/**
  * @brief            :  [st_led_init]
                         initialize the led hardware
  * @retval           :  [LED_OK              = 0x00U,
                          LED_ERROR           = 0x01U,
                          LED_BUSY            = 0x02U,
                          LED_TIMEOUT         = 0x03U,
                          LED_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [led_driver_t*self]
 */
static led_driver_state_t st_led_off(led_driver_t*self)
{
    /* led off code. */
    // check parameter.
    if(NULL == self || LED_DRIVER_NOT_INIT == self->is_init)
    {
        return LED_ERROR;
    }
    // led off operation.
    GPIO_TypeDef *p_hw = (GPIO_TypeDef*)(self->p_led_ops->p_port);
    uint16_t pin = self->p_led_ops->pin;
    GPIO_PinState pin_state = GPIO_PIN_RESET;
    if(LED_ON_LEVEL_HIGH != self->p_led_ops->led_on_level )
    {
        pin_state = GPIO_PIN_SET;
    }
    HAL_GPIO_WritePin(p_hw, pin, pin_state);
    return LED_OK;
}
/**
  * @brief            :  [st_led_init]
                         initialize the led hardware
  * @retval           :  [LED_OK              = 0x00U,
                          LED_ERROR           = 0x01U,
                          LED_BUSY            = 0x02U,
                          LED_TIMEOUT         = 0x03U,
                          LED_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [led_driver_t*self]
 */
static led_driver_state_t st_led_blink(led_driver_t*self)
{
    /* led blink code. */   
    // check parameter.
    if(NULL == self || LED_DRIVER_NOT_INIT == self->is_init)
    {
        return LED_ERROR;
    }
    // led blink operation.
    GPIO_TypeDef *p_hw = (GPIO_TypeDef*)(self->p_led_ops->p_port);
    uint16_t pin = self->p_led_ops->pin;
    HAL_GPIO_TogglePin(p_hw, pin);
    return LED_OK;
}
/**
  * @brief            :  [st_led_init]
                         initialize the led hardware
  * @retval           :  [LED_OK              = 0x00U,
                          LED_ERROR           = 0x01U,
                          LED_BUSY            = 0x02U,
                          LED_TIMEOUT         = 0x03U,
                          LED_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [led_driver_t*self]
 */
static led_driver_state_t st_led_deinit(led_driver_t*self)
{
    /* led deinit code. */
    if(NULL == self || LED_DRIVER_NOT_INIT == self->is_init)
    {
        return LED_ERROR;
    }
    return LED_OK;
}
/* Exported functions -------------------------------------------------------*/

/* end of  file -------------------------------------------------------------*/
