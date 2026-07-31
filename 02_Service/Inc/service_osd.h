/**
 ******************************************************************************
 *@file               :   service_osd.h
 *@brief              :   OSD (On-Screen Display) status bar service.
 *                        Renders a top-bar overlay with REC indicator,
 *                        battery level, and real-time FPS.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef SERVICE_OSD_H
#define SERVICE_OSD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "plat_error.h"

/* ---- OSD layout constants ------------------------------------------------- */
#define OSD_BAR_HEIGHT   (20U)
#define OSD_BAR_Y        (0U)

/* ---- REC state ------------------------------------------------------------ */
typedef enum
{
    SERVICE_OSD_REC_IDLE   = 0x00U,
    SERVICE_OSD_REC_ACTIVE = 0x01U,
} service_osd_rec_state_t;

/* ---- API ------------------------------------------------------------------ */
platform_err_t service_osd_init(void);
platform_err_t service_osd_set_rec_state(service_osd_rec_state_t state);
platform_err_t service_osd_set_battery(uint8_t percent);
platform_err_t service_osd_render(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_OSD_H */
