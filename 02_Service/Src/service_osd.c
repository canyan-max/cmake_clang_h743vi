/**
 ******************************************************************************
 *@file               :   service_osd.c
 *@brief              :   OSD status bar — REC indicator / battery / FPS.
 *                        Renders a 20px top-bar on the LCD after each frame.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include "service_osd.h"
#include "device_display.h"
#include "front.h"
#include "plat_tick.h"

/* ---- layout constants ------------------------------------------------------ */
#define OSD_BAR_WIDTH    (240U)
#define OSD_BG_COLOR     (0x0841U)   /* dark gray, readable on black camera bg  */
#define OSD_TEXT_COLOR   (0xFFFFU)   /* white                                    */
#define OSD_REC_COLOR    (0xF800U)   /* red                                      */

#define OSD_REC_DOT_X    (3U)
#define OSD_REC_DOT_Y    (7U)
#define OSD_REC_DOT_SZ   (6U)
#define OSD_REC_TXT_X    (11U)

#define OSD_BAT_X        (86U)
#define OSD_FPS_X        (168U)
#define OSD_TXT_Y        (2U)        /* text y in 20px bar (16px font + 2px pad) */

#define OSD_FPS_PERIOD   (1000U)     /* ms between FPS updates                   */
#define OSD_BATTERY_MOCK (85U)       /* default mock value for battery            */

/* ---- state ---------------------------------------------------------------- */
static service_osd_rec_state_t s_rec;
static uint8_t                  s_bat;
static float                    s_fps;

static uint32_t s_frm_cnt;
static uint32_t s_tick;
static uint8_t  s_first;       /* first render flag                             */

/* ---- public API ----------------------------------------------------------- */

/**
  * @brief            :  [service_osd_init] reset OSD state, set battery mock
  * @retval           :  PLATFORM_ERR_OK
  */
platform_err_t service_osd_init(void)
{
    s_rec     = SERVICE_OSD_REC_IDLE;
    s_bat     = OSD_BATTERY_MOCK;
    s_fps     = 0.0f;
    s_frm_cnt = 0U;
    s_tick    = plat_tick_get_ms();
    s_first   = 1U;
    return PLATFORM_ERR_OK;
}

/**
  * @brief            :  [service_osd_set_rec_state] toggle REC indicator
  * @retval           :  PLATFORM_ERR_OK
  * @param[in]        :  [state] SERVICE_OSD_REC_IDLE / SERVICE_OSD_REC_ACTIVE
  */
platform_err_t service_osd_set_rec_state(service_osd_rec_state_t state)
{
    if (state != s_rec)
    {
        s_rec   = state;
        s_first = 1U;  /* force full redraw on next render */
    }
    return PLATFORM_ERR_OK;
}

/**
  * @brief            :  [service_osd_set_battery] set battery percentage
  * @retval           :  PLATFORM_ERR_OK / PLATFORM_ERR_PARAM
  * @param[in]        :  [percent] 0~100
  */
platform_err_t service_osd_set_battery(uint8_t percent)
{
    if (percent > 100U)
    {
        return PLATFORM_ERR_PARAM;
    }
    s_bat = percent;
    return PLATFORM_ERR_OK;
}

/**
  * @brief            :  [service_osd_render] draw OSD bar on LCD.
  *                      Called once per camera frame.  Updates internal FPS.
  *                      On first frame or REC state change: clears bar + draws REC.
  *                      Subsequent frames: only redraw battery/FPS text —
  *                      the text bg_color naturally covers old content.
  * @retval           :  PLATFORM_ERR_OK / PLATFORM_ERR_HW
  */
platform_err_t service_osd_render(void)
{
    char           buf[16];
    platform_err_t ret;

    /* ------ FPS update (1 Hz refresh) ------ */
    s_frm_cnt++;
    uint32_t now     = plat_tick_get_ms();
    uint32_t elapsed = (now >= s_tick)
                           ? (now - s_tick)
                           : (UINT32_MAX - s_tick + now + 1U);
    if (elapsed >= OSD_FPS_PERIOD)
    {
        if (0U != elapsed)
        {
            s_fps = (float)s_frm_cnt * 1000.0f / (float)elapsed;
        }
        s_frm_cnt = 0U;
        s_tick    = now;
    }

    /* ------ background + REC: only on first frame or REC state change ------ */
    if (s_first)
    {
        ret = device_display_fill_rect(0, OSD_BAR_Y,
                                         OSD_BAR_WIDTH, OSD_BAR_HEIGHT,
                                         OSD_BG_COLOR);
        if (PLATFORM_ERR_OK != ret) { return ret; }

        if (SERVICE_OSD_REC_ACTIVE == s_rec)
        {
            ret = device_display_fill_rect(OSD_REC_DOT_X, OSD_REC_DOT_Y,
                                             OSD_REC_DOT_SZ, OSD_REC_DOT_SZ,
                                             OSD_REC_COLOR);
            if (PLATFORM_ERR_OK != ret) { return ret; }

            ret = device_display_draw_string(&g_f8x16, OSD_REC_TXT_X, OSD_TXT_Y,
                                               "REC", OSD_REC_COLOR, OSD_BG_COLOR);
            if (PLATFORM_ERR_OK != ret) { return ret; }
        }
        s_first = 0U;
    }

    /* ------ battery (white, redrawn every frame, bg covers old text) ------ */
    (void)snprintf(buf, sizeof(buf), "B:%u%%", (unsigned int)s_bat);
    ret = device_display_draw_string(&g_f8x16, OSD_BAT_X, OSD_TXT_Y,
                                       buf, OSD_TEXT_COLOR, OSD_BG_COLOR);
    if (PLATFORM_ERR_OK != ret) { return ret; }

    /* ------ FPS (white, redrawn every frame, bg covers old text) ------ */
    {
        uint8_t fps_int = (uint8_t)s_fps;
        uint8_t fps_dec = (uint8_t)((s_fps - (float)fps_int) * 10.0f + 0.5f);
        if (fps_dec > 9U)
        {
            fps_dec = 0U;
            if (fps_int < 255U) { fps_int++; }
        }
        (void)snprintf(buf, sizeof(buf), "%u.%uFPS",
                        (unsigned int)fps_int, (unsigned int)fps_dec);
        ret = device_display_draw_string(&g_f8x16, OSD_FPS_X, OSD_TXT_Y,
                                           buf, OSD_TEXT_COLOR, OSD_BG_COLOR);
        if (PLATFORM_ERR_OK != ret) { return ret; }
    }

    return PLATFORM_ERR_OK;
}

/* end of  file -------------------------------------------------------------*/
