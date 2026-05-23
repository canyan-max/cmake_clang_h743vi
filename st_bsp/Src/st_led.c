/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   st_led.c
 * 
 *@pardependencies    :
 *                        st_led.c
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

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>                               /* stdint lib header file. */
#include "st_led.h"                               /* st_led lib header file. */
#include "bsp_driver_led.h"               /* bsp_driver_led lib header file. */
#include "gpio.h"                                   /* gpio lib header file. */
#include "stm32h7xx_hal_gpio.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, const GPIO_InitTypeDef *GPIO_Init)
/* Private  functions  ------------------------------------------------------*/
static led_driver_state_t st_led_init(led_driver_t*self)
{
    /* led init code. */
    if(NULL == self || LED_DRIVER_NOT_INIT == self->is_init)
    {
        return LED_ERROR;
    }

    return LED_OK;
}
static led_driver_state_t st_led_on(led_driver_t*self)
{
    /* led on code. */
    if(NULL == self || LED_DRIVER_NOT_INIT == self->is_init)
    {
        return LED_ERROR;
    }
    GPIO_TypeDef *p_hw = (GPIO_TypeDef*)(self->p_led_hw->p_hw);
    uint16_t pin = self->p_led_hw->pin;
    GPIO_PinState pin_state = GPIO_PIN_RESET;
    if(LED_ON_LEVEL_HIGH == self->p_led_hw->led_on_level )
    {
        pin_state = GPIO_PIN_SET;
    }
    HAL_GPIO_WritePin(p_hw, pin, pin_state);
    return LED_OK;
}
static led_driver_state_t st_led_off(led_driver_t*self)
{
    /* led off code. */
    if(NULL == self || LED_DRIVER_NOT_INIT == self->is_init)
    {
        return LED_ERROR;
    }
    GPIO_TypeDef *p_hw = (GPIO_TypeDef*)(self->p_led_hw->p_hw);
    uint16_t pin = self->p_led_hw->pin;
    GPIO_PinState pin_state = GPIO_PIN_RESET;
    if(LED_ON_LEVEL_HIGH != self->p_led_hw->led_on_level )
    {
        pin_state = GPIO_PIN_SET;
    }
    HAL_GPIO_WritePin(p_hw, pin, pin_state);
    return LED_OK;
}
static led_driver_state_t st_led_blink(led_driver_t*self)
{
    /* led blink code. */
    if(NULL == self || LED_DRIVER_NOT_INIT == self->is_init)
    {
        return LED_ERROR;
    }
    HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
    return LED_OK;
}
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
