/**
 ******************************************************************************
 *@file               :   service_key.c
 *@brief              :   Non-blocking key gesture service. Each board key
 *                        adapts fitted BSP keys and platform time to reusable
 *                        Button component instances. Nothing here waits or
 *                        sleeps; service_key_poll() runs in task context.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h> /* NULL */
#include "service_key.h"
#include "bsp_key.h"
#include "button.h"
#include "plat_sys.h" /* plat_tick_get_ms: RTOS-independent tick */

/* define   -----------------------------------------------------------------*/
#define SERVICE_KEY_DEBOUNCE_MS           (20U)  /* stable level before accept */
#define SERVICE_KEY_LONG_PRESS_MS         (1000U) /* hold duration -> long press */
#define SERVICE_KEY_DOUBLE_CLICK_WINDOW_MS (300U) /* window after 1st release */

#define SERVICE_KEY_MAX_COUNT             (4U) /* guard against bsp key growth */

/* variables ----------------------------------------------------------------*/
static button_t                s_keys[SERVICE_KEY_MAX_COUNT];
static uint8_t                 s_key_count;
static service_key_event_cb_t  s_event_cb;

static const button_timing_t s_button_timing = {
    .debounce_ms     = SERVICE_KEY_DEBOUNCE_MS,
    .long_press_ms   = SERVICE_KEY_LONG_PRESS_MS,
    .double_click_ms = SERVICE_KEY_DOUBLE_CLICK_WINDOW_MS,
};

/* private  functions  ------------------------------------------------------*/
static void key_report(uint8_t key_id, service_key_event_t event)
{
    if(NULL != s_event_cb)
    {
        s_event_cb(key_id, event);
    }
}

static platform_err_t service_key_convert_button_status(button_status_t status)
{
    switch(status)
    {
        case BUTTON_STATUS_OK:
            return PLATFORM_ERR_OK;

        case BUTTON_STATUS_PARAM:
            return PLATFORM_ERR_PARAM;

        case BUTTON_STATUS_NOT_INITIALIZED:
        default:
            return PLATFORM_ERR_HW;
    }
}

static void service_key_report_button_event(uint8_t        key_id,
                                            button_event_t event)
{
    switch(event)
    {
        case BUTTON_EVENT_CLICK:
            key_report(key_id, SERVICE_KEY_EVENT_CLICK);
            break;

        case BUTTON_EVENT_DOUBLE_CLICK:
            key_report(key_id, SERVICE_KEY_EVENT_DOUBLE_CLICK);
            break;

        case BUTTON_EVENT_LONG_PRESS:
            key_report(key_id, SERVICE_KEY_EVENT_LONG_PRESS);
            break;

        case BUTTON_EVENT_NONE:
        case BUTTON_EVENT_PRESS:
        case BUTTON_EVENT_RELEASE:
        default:
            break;
    }
}

/* exported functions -------------------------------------------------------*/

platform_err_t service_key_init(service_key_event_cb_t cb)
{
    uint8_t key_count = bsp_key_count();
    s_key_count       = 0U;
    s_event_cb        = NULL;

    if(key_count > (uint8_t)SERVICE_KEY_MAX_COUNT)
    {
        return PLATFORM_ERR_PARAM;
    }

    uint32_t now_ms = plat_tick_get_ms();
    for(uint8_t i = 0U; i < key_count; i++)
    {
        uint8_t pressed;
        platform_err_t error = bsp_key_is_pressed(i, &pressed);
        if(PLATFORM_ERR_OK != error)
        {
            return error;
        }

        button_status_t status = button_init(&s_keys[i], &s_button_timing,
                                             pressed, now_ms);
        if(BUTTON_STATUS_OK != status)
        {
            return service_key_convert_button_status(status);
        }
    }

    s_event_cb  = cb;
    s_key_count = key_count;
    return PLATFORM_ERR_OK;
}

platform_err_t service_key_poll(void)
{
    uint32_t now = plat_tick_get_ms();

    for(uint8_t i = 0U; i < s_key_count; i++)
    {
        uint8_t pressed;
        platform_err_t error = bsp_key_is_pressed(i, &pressed);
        if(PLATFORM_ERR_OK != error)
        {
            return error;
        }

        button_event_t event;
        button_status_t status = button_update(&s_keys[i], pressed, now,
                                                &event);
        if(BUTTON_STATUS_OK != status)
        {
            return service_key_convert_button_status(status);
        }
        service_key_report_button_event(i, event);
    }

    return PLATFORM_ERR_OK;
}

/* end of file --------------------------------------------------------------*/
