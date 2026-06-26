/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   led_handle.h
 *
 *@pardependencies    :
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
#ifndef LED_HANDLE_H
#define LED_HANDLE_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */
#include <stddef.h> /* stddef lib header file. */

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t (*pf_drv_init)(void *p_drv, void *drv_ops);
    uint8_t (*pf_led_on)(void *p_drv);
    uint8_t (*pf_led_off)(void *p_drv);
    uint8_t (*pf_led_blink)(void *p_drv);
} led_handle_ops_t;

typedef struct
{
    const led_handle_ops_t *p_ops;
    void                   *p_drv;
} led_handle_t;
/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
 * @brief            :  [led_handle_on]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [led_handle_t *p_handle]
 */
uint8_t led_handle_on(led_handle_t *p_handle);
/**
 * @brief            :  [led_handle_off]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [led_handle_t *p_handle]
 */
uint8_t led_handle_off(led_handle_t *p_handle);
/**
 * @brief            :  [led_handle_blink]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [led_handle_t *p_handle]
 */
uint8_t led_handle_blink(led_handle_t *p_handle);
/**
 * @brief            :  [led_handle_instruct]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [led_handle_t *p_handle, \
                                const led_ops_t *p_ops, \
                                void *p_drv, \
                                void *p_drv_ops]
 */
uint8_t led_handle_instruct(led_handle_t           *p_handle,
                            const led_handle_ops_t *p_ops,
                            void                   *p_drv,
                            void                   *p_drv_ops);
#endif /* LED_HANDLE_H */
