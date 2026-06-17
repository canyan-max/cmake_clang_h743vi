/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_drv_led.h
 * 
 *@pardependencies    :
 *                        bsp_drv_led.h
 *                        xxx.h   
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
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef BSP_DRV_LED_H
#define BSP_DRV_LED_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */

/* define -------------------------------------------------------------------*/
#define LED_ON_LEVEL_HIGH        (0x01U)
#define LED_ON_LEVEL_LOW         (0x00U)
#define LED_DRIVER_IS_INIT       (0x01U)
#define LED_DRIVER_NOT_INIT      (0x00U)
/* typedef ------------------------------------------------------------------*/
typedef struct LED_DRIVER_T led_driver_t;

typedef enum LED_DRIVER_STATE_T
{
    LED_OK              = 0x00U,    // Operation is OK.
    LED_ERROR           = 0x01U,    // Operation is error.
    LED_BUSY            = 0x02U,    // Operation is busy.
    LED_TIMEOUT         = 0x03U,    // Operation is timeout.
    LED_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.
}led_driver_state_t;

typedef struct LED_OPERATION_T
{
    void *                                             p_port;
    led_driver_state_t (*pf_led_init  )   (led_driver_t*self);
    led_driver_state_t (*pf_led_on    )   (led_driver_t*self);
    led_driver_state_t (*pf_led_off   )   (led_driver_t*self);
    led_driver_state_t (*pf_led_blink )   (led_driver_t*self);
    led_driver_state_t (*pf_led_deinit)   (led_driver_t*self);
    uint16_t                                              pin;
    uint8_t                                      led_on_level;
}led_operation_t;

typedef struct LED_DRIVER_T
{
    const led_operation_t *p_led_ops;
    uint8_t                is_init; 
}led_driver_t;
/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
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
                                       const led_operation_t *p_ops);
#endif /* BSP_DRV_LED_H */
