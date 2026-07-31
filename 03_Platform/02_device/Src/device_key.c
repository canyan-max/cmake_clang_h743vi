/**
 ******************************************************************************
 *@file               :   device_key.c
 *@brief              :   Device key — debounce state machine + event output.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "device_key.h"
#include "bsp_key.h"
#include "plat_sys.h"

/* define -------------------------------------------------------------------*/
#define KEY_DEBOUNCE_MS    (20U)
#define KEY_LONG_PRESS_MS  (500U)

/* ---- internal state machine ---------------------------------------------- */
typedef enum
{
    KSM_IDLE,
    KSM_DEBOUNCING,
    KSM_PRESSED,
    KSM_LONG_FIRED,
} key_sm_t;

typedef struct
{
    key_sm_t sm;
    uint32_t press_tick;
} key_ctx_t;

/* variables ----------------------------------------------------------------*/
static key_ctx_t g_keys[DEVICE_KEY_NUM];

/* exported functions -------------------------------------------------------*/

platform_err_t device_key_init(device_key_id_t id)
{
    if(id >= DEVICE_KEY_NUM) { return PLATFORM_ERR_PARAM; }
    g_keys[id].sm         = KSM_IDLE;
    g_keys[id].press_tick = 0U;
    return PLATFORM_ERR_OK;
}

/**
  * @brief            : [device_key_get_event] run debounce state machine and
  *                     return a one-shot event. Call periodically (e.g. 50ms).
  * @retval           : DEVICE_KEY_EVT_NONE / SHORT_PRESS / LONG_PRESS
  * @param[in]        : id  key index
  */
device_key_event_t device_key_get_event(device_key_id_t id)
{
    if(id >= DEVICE_KEY_NUM) { return DEVICE_KEY_EVT_NONE; }

    key_ctx_t *p      = &g_keys[id];
    uint8_t    pressed = bsp_key_is_pressed((uint8_t)id);
    uint32_t   now     = plat_tick_get_ms();
    uint32_t   elapsed = now - p->press_tick;

    switch(p->sm)
    {
        case KSM_IDLE:
            if(1U == pressed)
            {
                p->press_tick = now;
                p->sm         = KSM_DEBOUNCING;
            }
            break;

        case KSM_DEBOUNCING:
            if(0U == pressed)
            {
                p->sm = KSM_IDLE;
            }
            else if(elapsed >= KEY_DEBOUNCE_MS)
            {
                p->sm = KSM_PRESSED;
            }
            else { /* waiting */ }
            break;

        case KSM_PRESSED:
            if(0U == pressed)
            {
                p->sm = KSM_IDLE;
                return DEVICE_KEY_EVT_SHORT_PRESS;
            }
            if(elapsed >= KEY_LONG_PRESS_MS)
            {
                p->sm = KSM_LONG_FIRED;
                return DEVICE_KEY_EVT_LONG_PRESS;
            }
            break;

        case KSM_LONG_FIRED:
            if(0U == pressed) { p->sm = KSM_IDLE; }
            break;

        default:
            p->sm = KSM_IDLE;
            break;
    }
    return DEVICE_KEY_EVT_NONE;
}

/* end of file --------------------------------------------------------------*/
