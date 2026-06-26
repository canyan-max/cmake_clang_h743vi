/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   led_handle.c
 *
 *@pardependencies    :   led_handle.c
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
#include <stddef.h>     /* stdint lib header file. */
#include "led_handle.h" /* led_handle lib header file. */

/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
/**
 * @brief            :  [led_handle_on]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [led_handle_t *p_handle]
 */
uint8_t led_handle_on(led_handle_t *p_handle)
{
    return p_handle->p_ops->pf_led_on(p_handle->p_drv);
}
/**
 * @brief            :  [led_handle_off]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [led_handle_t *p_handle]
 */
uint8_t led_handle_off(led_handle_t *p_handle)
{
    return p_handle->p_ops->pf_led_off(p_handle->p_drv);
}
/**
 * @brief            :  [led_handle_blink]
 * @retval           :  [0 successful,
                         1 invalid parameter
                        ]
 * @param[in]        :  [led_handle_t *p_handle]
 */
uint8_t led_handle_blink(led_handle_t *p_handle)
{
    return p_handle->p_ops->pf_led_blink(p_handle->p_drv);
}
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
                            void                   *p_drv_ops)
{
    if(NULL == p_handle || NULL == p_drv || NULL == p_ops)
    {
        return 1;
    }
    p_handle->p_ops = p_ops;
    p_handle->p_drv = p_drv;
    return p_handle->p_ops->pf_drv_init(p_handle->p_drv, p_drv_ops);
}
/* end of  file -------------------------------------------------------------*/
