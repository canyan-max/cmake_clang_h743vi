/**
 ******************************************************************************
 *@file               :   bsp_drv_led.c
 *@brief              :   Provide the HAL APIs of description.
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include <stddef.h>      /* stddef lib header file. */
#include "bsp_drv_led.h" /* bsp_drv_led lib header file. */
#include "st_led.h"      /* Impl — board LED ops instances */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* private  functions  ------------------------------------------------------*/

/* exported functions -------------------------------------------------------*/
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
led_driver_state_t bsp_driver_led_init(led_driver_t          *p_drv,
                                       const led_operation_t *p_ops)
{

    if((NULL == p_drv) || (NULL == p_ops) ||
       (LED_DRIVER_IS_INIT == p_drv->is_init))
    {
        return LED_INVALID_PARAM;
    }
    p_drv->p_led_ops = p_ops;
    p_drv->is_init   = LED_DRIVER_IS_INIT;
    return LED_OK;
}
/* ---- board-level wrappers ------------------------------------------------ */

extern const led_operation_t g_led1_ops;
extern const led_operation_t g_led2_ops;

led_driver_state_t bsp_led_init_by_id(led_driver_t *p_drv, uint8_t id)
{
    const led_operation_t *ops = NULL;

    if (0U == id)
    {
        ops = &g_led1_ops;
    }
    else if (1U == id)
    {
        ops = &g_led2_ops;
    }
    else
    {
        return LED_INVALID_PARAM;
    }
    return bsp_driver_led_init(p_drv, ops);
}

/* end of  file -------------------------------------------------------------*/
