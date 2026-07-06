/**
 ******************************************************************************
 *@file               :   led_adapter.c
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 * 
 *@note               :   1 tab == 4 spaces!  2026
 * 
 *@pardependencies    :   led_adapter.c
 ******************************************************************************
 */
/* Includes -----------------------------------------------------------------*/
#include "led_adapter.h" /* led_adapter lib header file. */
#include "bsp_drv_led.h" /* bsp_drv_led lib header file. */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* private  functions  ------------------------------------------------------*/

/* exported functions -------------------------------------------------------*/
/**
 * @brief            :  [drv_led_on] drv open the led operation
 * @retval           :  [DRV_LED_OK=0, DRV_LED_ERR_PARAM=1, DRV_LED_ERR_HW=2]
 * @param[in]        :  [p_drv]
 */
uint8_t drv_led_on(void *p_drv)
{
    if(NULL == p_drv)
    {
        return DRV_LED_ERR_PARAM;
    }

    led_driver_t *p_led = (led_driver_t *)p_drv;

    if(LED_DRIVER_IS_INIT != p_led->is_init)
    {
        return DRV_LED_ERR_PARAM;
    }

    if(NULL == p_led->p_led_ops || NULL == p_led->p_led_ops->pf_led_on)
    {
        return DRV_LED_ERR_PARAM;
    }

    led_driver_state_t ret = p_led->p_led_ops->pf_led_on(p_led);
    if(LED_OK != ret)
    {
        return DRV_LED_ERR_HW;
    }

    return DRV_LED_OK;
}
/**
 * @brief            :  [drv_led_off] drv close the led operation
 * @retval           :  [DRV_LED_OK=0, DRV_LED_ERR_PARAM=1, DRV_LED_ERR_HW=2]
 * @param[in]        :  [p_drv]
 */
uint8_t drv_led_off(void *p_drv)
{
    if(NULL == p_drv)
    {
        return DRV_LED_ERR_PARAM;
    }

    led_driver_t *p_led = (led_driver_t *)p_drv;

    if(LED_DRIVER_IS_INIT != p_led->is_init)
    {
        return DRV_LED_ERR_PARAM;
    }

    if(NULL == p_led->p_led_ops || NULL == p_led->p_led_ops->pf_led_off)
    {
        return DRV_LED_ERR_PARAM;
    }

    led_driver_state_t ret = p_led->p_led_ops->pf_led_off(p_led);
    if(LED_OK != ret)
    {
        return DRV_LED_ERR_HW;
    }

    return DRV_LED_OK;
}
/**
 * @brief            :  [drv_led_blink] drv blink the led operation
 * @retval           :  [DRV_LED_OK=0, DRV_LED_ERR_PARAM=1, DRV_LED_ERR_HW=2]
 * @param[in]        :  [p_drv]
 */
uint8_t drv_led_blink(void *p_drv)
{
    if(NULL == p_drv)
    {
        return DRV_LED_ERR_PARAM;
    }

    led_driver_t *p_led = (led_driver_t *)p_drv;

    if(LED_DRIVER_IS_INIT != p_led->is_init)
    {
        return DRV_LED_ERR_PARAM;
    }

    if(NULL == p_led->p_led_ops || NULL == p_led->p_led_ops->pf_led_blink)
    {
        return DRV_LED_ERR_PARAM;
    }

    led_driver_state_t ret = p_led->p_led_ops->pf_led_blink(p_led);
    if(LED_OK != ret)
    {
        return DRV_LED_ERR_HW;
    }

    return DRV_LED_OK;
}
/**
 * @brief            :  [drv_led_init] drv init the led operation
 * @retval           :  [DRV_LED_OK=0, DRV_LED_ERR_PARAM=1, DRV_LED_ERR_HW=2]
 * @param[in]        :  [p_drv, drv_ops]
 */
uint8_t drv_led_init(void *p_drv, void *drv_ops)
{
    if(NULL == p_drv || NULL == drv_ops)
    {
        return DRV_LED_ERR_PARAM;
    }

    led_driver_t *p_led = (led_driver_t *)p_drv;

    if(LED_DRIVER_IS_INIT == p_led->is_init)
    {
        return DRV_LED_OK;
    }

    led_driver_state_t ret = bsp_driver_led_init(p_led,
                                                 (const led_operation_t *)
                                                     drv_ops);
    if(LED_OK != ret)
    {
        return DRV_LED_ERR_HW;
    }

    return DRV_LED_OK;
}
/* end of file --------------------------------------------------------------*/
