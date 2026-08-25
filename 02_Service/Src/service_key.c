/**
 ******************************************************************************
 *@file               :   service_key.c
 *@brief              :   Non-blocking key gesture service. Each board key
 *                        runs an independent timestamp-driven state machine
 *                        (debounce -> press -> single/double/long). Nothing
 *                        here waits or sleeps: service_key_poll() only reads
 *                        the level, computes elapsed ticks and advances state.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h> /* NULL */
#include <stdint.h> /* UINT32_MAX for the tick-wraparound calc */
#include "service_key.h"
#include "bsp_key.h"
#include "plat_sys.h" /* plat_tick_get_ms: RTOS-independent tick */

/* define   -----------------------------------------------------------------*/
#define SERVICE_KEY_DEBOUNCE_MS           (20U)  /* stable level before accept */
#define SERVICE_KEY_LONG_PRESS_MS         (1000U) /* hold duration -> long press */
#define SERVICE_KEY_DOUBLE_CLICK_WINDOW_MS (300U) /* window after 1st release */

#define SERVICE_KEY_MAX_COUNT             (4U) /* guard against bsp key growth */

/* typedef ------------------------------------------------------------------*/
typedef enum
{
    KEY_ST_IDLE        = 0U, /* waiting for a stable press */
    KEY_ST_PRESSED     = 1U, /* stable press, timing the hold */
    KEY_ST_LONG_FIRED  = 2U, /* long press reported, waiting release */
    KEY_ST_WAIT_DOUBLE = 3U, /* 1st release done, waiting 2nd press / timeout */
} key_state_t;

typedef struct
{
    uint8_t     raw_level;    /* last sampled level (1 = pressed) */
    uint8_t     stable_level; /* debounced level */
    uint32_t    edge_tick;    /* tick of the last raw-level change */
    uint32_t    press_tick;   /* tick of the accepted press */
    uint32_t    release_tick; /* tick of the accepted release */
    key_state_t state;
} service_key_inst_t;

/* variables ----------------------------------------------------------------*/
static service_key_inst_t      s_keys[SERVICE_KEY_MAX_COUNT];
static uint8_t                 s_key_count;
static service_key_event_cb_t  s_event_cb;

/* private  functions  ------------------------------------------------------*/

/* Elapsed ms between two tick snapshots, safe across uint32_t wraparound. */
static uint32_t key_elapsed_ms(uint32_t from, uint32_t now)
{
    return (now >= from) ? (now - from) : (UINT32_MAX - from + now + 1U);
}

static void key_report(uint8_t key_id, service_key_event_t event)
{
    if(NULL != s_event_cb)
    {
        s_event_cb(key_id, event);
    }
}

/* Stable-level change handler: drives the gesture transitions that are
 * triggered by an edge, not by elapsed time. */
static void key_on_stable_change(service_key_inst_t *p_key, uint8_t key_id,
                                 uint32_t now)
{
    if(1U == p_key->stable_level)
    {
        /* ---- press edge ---- */
        switch(p_key->state)
        {
        case KEY_ST_IDLE:
            p_key->state     = KEY_ST_PRESSED;
            p_key->press_tick = now;
            break;
        case KEY_ST_WAIT_DOUBLE:
            /* second press inside the window -> double click */
            key_report(key_id, SERVICE_KEY_EVENT_DOUBLE_CLICK);
            p_key->state = KEY_ST_IDLE;
            break;
        default:
            break;
        }
    }
    else
    {
        /* ---- release edge ---- */
        switch(p_key->state)
        {
        case KEY_ST_PRESSED:
            /* first release: arm the double-click window */
            p_key->state       = KEY_ST_WAIT_DOUBLE;
            p_key->release_tick = now;
            break;
        case KEY_ST_LONG_FIRED:
            p_key->state = KEY_ST_IDLE;
            break;
        default:
            break;
        }
    }
}

/* exported functions -------------------------------------------------------*/

platform_err_t service_key_init(service_key_event_cb_t cb)
{
    s_event_cb = cb;
    s_key_count = bsp_key_count();
    if(s_key_count > (uint8_t)SERVICE_KEY_MAX_COUNT)
    {
        s_key_count = (uint8_t)SERVICE_KEY_MAX_COUNT;
    }

    for(uint8_t i = 0U; i < s_key_count; i++)
    {
        /* Seed raw/stable with the current level so a key already held at
         * init is not misread as a fresh press edge. */
        s_keys[i].raw_level    = bsp_key_is_pressed(i);
        s_keys[i].stable_level = s_keys[i].raw_level;
        s_keys[i].edge_tick    = plat_tick_get_ms();
        s_keys[i].press_tick   = s_keys[i].edge_tick;
        s_keys[i].release_tick = s_keys[i].edge_tick;
        s_keys[i].state        = KEY_ST_IDLE;
    }
    return PLATFORM_ERR_OK;
}

void service_key_poll(void)
{
    uint32_t now = plat_tick_get_ms();

    for(uint8_t i = 0U; i < s_key_count; i++)
    {
        service_key_inst_t *p_key = &s_keys[i];
        uint8_t raw = bsp_key_is_pressed(i);

        /* Track raw edges for debouncing. */
        if(raw != p_key->raw_level)
        {
            p_key->raw_level = raw;
            p_key->edge_tick = now;
        }

        /* Debounce: accept the new level only once it has been stable for
         * DEBOUNCE_MS. */
        if((raw != p_key->stable_level) &&
           (key_elapsed_ms(p_key->edge_tick, now) >= SERVICE_KEY_DEBOUNCE_MS))
        {
            p_key->stable_level = raw;
            key_on_stable_change(p_key, i, now);
        }

        /* Time-driven transitions (checked every poll, never blocked). */
        switch(p_key->state)
        {
        case KEY_ST_PRESSED:
            if(key_elapsed_ms(p_key->press_tick, now) >=
               SERVICE_KEY_LONG_PRESS_MS)
            {
                key_report(i, SERVICE_KEY_EVENT_LONG_PRESS);
                p_key->state = KEY_ST_LONG_FIRED;
            }
            break;
        case KEY_ST_WAIT_DOUBLE:
            if(key_elapsed_ms(p_key->release_tick, now) >=
               SERVICE_KEY_DOUBLE_CLICK_WINDOW_MS)
            {
                key_report(i, SERVICE_KEY_EVENT_CLICK);
                p_key->state = KEY_ST_IDLE;
            }
            break;
        default:
            break;
        }
    }
}

/* end of file --------------------------------------------------------------*/
