/**
 ******************************************************************************
 *@file               :   service_record.c
 *@brief              :   Recording state machine driven by key gestures.
 *                        See service_record.h for the gesture -> state map.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include "service_record.h"
#include "service_osd.h"

/* variables ----------------------------------------------------------------*/
static service_osd_rec_state_t s_state;

/* private  functions  ------------------------------------------------------*/

static void service_record_set(service_osd_rec_state_t state)
{
    if(state != s_state)
    {
        s_state = state;
        (void)service_osd_set_rec_state(s_state);
    }
}

/* exported functions -------------------------------------------------------*/

platform_err_t service_record_init(void)
{
    s_state = SERVICE_OSD_REC_IDLE;
    (void)service_osd_set_rec_state(s_state);
    return PLATFORM_ERR_OK;
}

void service_record_on_key(uint8_t key_id, service_key_event_t event)
{
    (void)key_id; /* single shared recorder; key id kept for future use */

    switch(event)
    {
    case SERVICE_KEY_EVENT_LONG_PRESS:
        service_record_set(SERVICE_OSD_REC_ACTIVE); /* force record */
        break;
    case SERVICE_KEY_EVENT_CLICK:
        service_record_set(SERVICE_OSD_REC_IDLE); /* force stop */
        break;
    case SERVICE_KEY_EVENT_DOUBLE_CLICK:
        /* toggle between stop and record */
        service_record_set((SERVICE_OSD_REC_ACTIVE == s_state)
                               ? SERVICE_OSD_REC_IDLE
                               : SERVICE_OSD_REC_ACTIVE);
        break;
    default:
        break;
    }
}

/* end of file --------------------------------------------------------------*/
