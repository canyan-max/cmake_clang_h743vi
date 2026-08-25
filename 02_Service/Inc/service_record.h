/**
 ******************************************************************************
 *@file               :   service_record.h
 *@brief              :   Recording state machine driven by key gestures.
 *                        Maps key events onto the OSD REC/STOP indicator:
 *                          long-press  -> force REC
 *                          single-click-> force STOP
 *                          double-click-> toggle STOP <-> REC
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef SERVICE_RECORD_H
#define SERVICE_RECORD_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"
#include "service_key.h"     /* service_key_event_t */
#include "service_osd.h"     /* service_osd_rec_state_t */

/* functions ----------------------------------------------------------------*/
/**
 * @brief            : [service_record_init] Reset the recorder state to STOP
 *                     and apply it to the OSD.
 * @retval           : PLATFORM_ERR_OK on success.
 */
platform_err_t service_record_init(void);

/**
 * @brief            : [service_record_on_key] Apply a detected key gesture to
 *                     the recorder state machine and refresh the OSD.
 * @param[in]        : [key_id] Board key index (currently only logging).
 * @param[in]        : [event]  Detected key gesture.
 */
void service_record_on_key(uint8_t key_id, service_key_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_RECORD_H */
