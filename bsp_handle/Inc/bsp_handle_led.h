/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_handle_led.h
 * 
 *@pardependencies    :
 *                        bsp_handle_led.h
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
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef BSP_HANDLE_LED_H
#define BSP_HANDLE_LED_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>                               /* stdint lib header file. */
/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef enum LED_HANDLE_STATE_T
{
    LED_HANDLE_OK              = 0x00U,    // Operation is OK.
    LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
    LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
    LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
    LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.
}led_handle_state_t;

typedef enum LED_HANDLE_DEVINDEX_T
{
    LED_INDEX_1              = 0x00U,    // LED index 1.
    LED_INDEX_2              = 0x01U,    // LED index 2.
    LED_INDEX_MAX
}led_handle_devindex_t;

typedef struct LED_HANDLE_T
{
    void *p_led_drv[LED_INDEX_MAX];
}led_handle_t;
/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
  * @brief            :  [led_drv_register_handle]
  * @retval           :  [ LED_HANDLE_OK              = 0x00U,    // Operation is OK.
                           LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
                           LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
                           LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
                           LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.]
  * @param[in]        :  [led_handle_t *p_handle , \
                          void * p_drv]
 */
led_handle_state_t led_drv_register_handle(void * p_drv);
/**
  * @brief            :  [led_handle_on]
  * @retval           :  [ LED_HANDLE_OK              = 0x00U,    // Operation is OK.
                           LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
                           LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
                           LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
                           LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.]
  * @param[in]        :  [led_handle_devindex_t index]
 */
led_handle_state_t led_handle_on(led_handle_devindex_t index);
/**
  * @brief            :  [led_handle_off]
  * @retval           :  [ LED_HANDLE_OK              = 0x00U,    // Operation is OK.
                           LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
                           LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
                           LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
                           LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.]
  * @param[in]        :  [led_handle_devindex_t index]
 */
led_handle_state_t led_handle_off(led_handle_devindex_t index);
/**
  * @brief            :  [led_handle_blink]
  * @retval           :  [ LED_HANDLE_OK              = 0x00U,    // Operation is OK.
                           LED_HANDLE_ERROR           = 0x01U,    // Operation is error.
                           LED_HANDLE_BUSY            = 0x02U,    // Operation is busy.
                           LED_HANDLE_TIMEOUT         = 0x03U,    // Operation is timeout.
                           LED_HANDLE_INVALID_PARAM   = 0x04U,    // Operation is invalid parameter.]
  * @param[in]        :  [led_handle_devindex_t index]
 */
led_handle_state_t led_handle_blink(led_handle_devindex_t index);
#endif /* BSP_HANDLE_LED_H */
