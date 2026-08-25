/**
 ******************************************************************************
 *@file               :   service_osd.c
 *@brief              :   OSD status bar 鈥?REC indicator / battery / FPS.
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
#include "bsp_display.h"
#include "front.h"
#include "plat_sys.h"

/* ---- layout constants ------------------------------------------------------
 */
#define OSD_BG_COLOR     (0x0841U)/* dark gray, readable on black camera bg  */
#define OSD_TEXT_COLOR   (0xFFFFU)/* white*/
#define OSD_REC_COLOR    (0xF800U)/* red*/

#define OSD_REC_DOT_X    (3U)
#define OSD_REC_DOT_Y    (7U)
#define OSD_REC_DOT_SZ   (6U)
#define OSD_REC_TXT_X    (11U)

#define OSD_TXT_Y        (2U)/* text y in 20px bar (16px font + 2px pad) */

#define OSD_BAT_X_DIVISOR       (3U)
#define OSD_BAT_MAX_CHAR_COUNT  (6U)
#define OSD_FPS_MAX_CHAR_COUNT  (8U)
#define OSD_RIGHT_MARGIN        (4U)

#define OSD_FPS_PERIOD   (1000U)/* ms between FPS updates*/
#define OSD_BATTERY_MOCK (85U)/* default mock value for battery*/

/* ---- state ----------------------------------------------------------------
 */
static service_osd_rec_state_t s_rec;
static uint8_t                 s_bat;
static float                   s_fps;

static uint32_t s_frm_cnt;
static uint32_t s_tick;
static uint8_t  s_first; /* first render flag                             */
static uint8_t  s_battery_dirty;
static uint16_t s_display_width;
static uint16_t s_battery_x;
static uint16_t s_fps_x;

/* ---- public API -----------------------------------------------------------
 */

/**
 * @brief            :  [service_osd_init] configure logical layout and reset
 *                     OSD state.
 * @retval           :  PLATFORM_ERR_OK / PLATFORM_ERR_PARAM
 */
platform_err_t service_osd_init(uint16_t display_width)
{
    uint32_t battery_end;
    uint32_t fps_width = (uint32_t)OSD_FPS_MAX_CHAR_COUNT * g_f8x16.char_w;
    if((0U == display_width) ||
       ((uint32_t)display_width <= (fps_width + OSD_RIGHT_MARGIN)))
    {
        return PLATFORM_ERR_PARAM;
    }

    s_display_width = display_width;
    s_battery_x     = (uint16_t)(display_width / OSD_BAT_X_DIVISOR);
    s_fps_x         = (uint16_t)((uint32_t)display_width - fps_width -
                                 OSD_RIGHT_MARGIN);
    battery_end     = (uint32_t)s_battery_x +
                      ((uint32_t)OSD_BAT_MAX_CHAR_COUNT * g_f8x16.char_w);
    if(battery_end >= s_fps_x)
    {
        return PLATFORM_ERR_PARAM;
    }

    s_rec     = SERVICE_OSD_REC_IDLE;
    s_bat     = OSD_BATTERY_MOCK;
    s_fps     = 0.0f;
    s_frm_cnt = 0U;
    s_tick    = plat_tick_get_ms();
    s_first   = 1U;
    s_battery_dirty = 1U;
    return PLATFORM_ERR_OK;
}

/**
 * @brief            :  [service_osd_set_rec_state] toggle REC indicator
 * @retval           :  PLATFORM_ERR_OK
 * @param[in]        :  [state] SERVICE_OSD_REC_IDLE / SERVICE_OSD_REC_ACTIVE
 */
platform_err_t service_osd_set_rec_state(service_osd_rec_state_t state)
{
    if(state != s_rec)
    {
        s_rec   = state;
        s_first = 1U; /* force full redraw on next render */
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
    if(percent > 100U)
    {
        return PLATFORM_ERR_PARAM;
    }
    if(percent != s_bat)
    {
        s_bat           = percent;
        s_battery_dirty = 1U;
    }
    return PLATFORM_ERR_OK;
}

/**
 * @brief            :  [service_osd_render] draw OSD bar on LCD.
 *                      Called once per camera frame.  Updates internal FPS.
 *                      On first frame or REC state change: clears bar + draws
 * REC. Subsequent frames: only redraw battery/FPS text   * the text bg_color
 * naturally covers old content.
 * @retval           :  PLATFORM_ERR_OK / PLATFORM_ERR_HW
 */
platform_err_t service_osd_render(void)
{
    char           buf[16];
    platform_err_t ret;
    uint8_t        redraw_battery = s_battery_dirty;
    uint8_t        redraw_fps     = 0U;
    uint8_t        full_redraw    = s_first;

    /* ------ FPS update (1 Hz refresh) ------ */
    s_frm_cnt++;
    uint32_t now     = plat_tick_get_ms();
    uint32_t elapsed = (now >= s_tick) ? (now - s_tick)
                                       : (UINT32_MAX - s_tick + now + 1U);
    if(elapsed >= OSD_FPS_PERIOD)
    {
        if(0U != elapsed)
        {
            s_fps = (float)s_frm_cnt * 1000.0f / (float)elapsed;
        }
        s_frm_cnt = 0U;
        s_tick    = now;
        redraw_fps = 1U;
    }

    /* ------ background + REC: only on first frame or REC state change ------
     */
    if(s_first)
    {
        ret = bsp_display_fill_rect(0U, OSD_BAR_Y, s_display_width,
                                    OSD_BAR_HEIGHT, OSD_BG_COLOR);
        if(PLATFORM_ERR_OK != ret)
        {
            return ret;
        }

        if(SERVICE_OSD_REC_ACTIVE == s_rec)
        {
            ret = bsp_display_fill_rect(OSD_REC_DOT_X, OSD_REC_DOT_Y,
                                        OSD_REC_DOT_SZ, OSD_REC_DOT_SZ,
                                        OSD_REC_COLOR);
            if(PLATFORM_ERR_OK != ret)
            {
                return ret;
            }

            ret = bsp_display_draw_string(&g_f8x16, OSD_REC_TXT_X, OSD_TXT_Y,
                                          "REC", OSD_REC_COLOR, OSD_BG_COLOR);
            if(PLATFORM_ERR_OK != ret)
            {
                return ret;
            }
        }
        else
        {
            /* STOP state: show text so the recording status is always
             * visible, not just an empty bar. */
            ret = bsp_display_draw_string(&g_f8x16, OSD_REC_TXT_X, OSD_TXT_Y,
                                          "STOP", OSD_TEXT_COLOR,
                                          OSD_BG_COLOR);
            if(PLATFORM_ERR_OK != ret)
            {
                return ret;
            }
        }
        s_first = 0U;
        redraw_battery = 1U;
        redraw_fps     = 1U;
    }

    /* ------ battery: redraw only after a value/layout change ------ */
    if(0U != redraw_battery)
    {
        if(0U == full_redraw)
        {
            ret = bsp_display_fill_rect(
                s_battery_x, OSD_TXT_Y,
                (uint16_t)(OSD_BAT_MAX_CHAR_COUNT * g_f8x16.char_w),
                g_f8x16.char_h, OSD_BG_COLOR);
            if(PLATFORM_ERR_OK != ret)
            {
                return ret;
            }
        }
        (void)snprintf(buf, sizeof(buf), "B:%u%%", (unsigned int)s_bat);
        ret = bsp_display_draw_string(&g_f8x16, s_battery_x, OSD_TXT_Y, buf,
                                      OSD_TEXT_COLOR, OSD_BG_COLOR);
        if(PLATFORM_ERR_OK != ret)
        {
            return ret;
        }
        s_battery_dirty = 0U;
    }

    /* ------ FPS: redraw only when the 1 Hz measurement is updated ------ */
    if(0U != redraw_fps)
    {
        if(0U == full_redraw)
        {
            ret = bsp_display_fill_rect(
                s_fps_x, OSD_TXT_Y,
                (uint16_t)(OSD_FPS_MAX_CHAR_COUNT * g_f8x16.char_w),
                g_f8x16.char_h, OSD_BG_COLOR);
            if(PLATFORM_ERR_OK != ret)
            {
                return ret;
            }
        }
        uint8_t fps_int = (uint8_t)s_fps;
        uint8_t fps_dec = (uint8_t)((s_fps - (float)fps_int) * 10.0f + 0.5f);
        if(fps_dec > 9U)
        {
            fps_dec = 0U;
            if(fps_int < 255U)
            {
                fps_int++;
            }
        }
        (void)snprintf(buf, sizeof(buf), "%u.%uFPS", (unsigned int)fps_int,
                       (unsigned int)fps_dec);
        ret = bsp_display_draw_string(&g_f8x16, s_fps_x, OSD_TXT_Y, buf,
                                      OSD_TEXT_COLOR, OSD_BG_COLOR);
        if(PLATFORM_ERR_OK != ret)
        {
            return ret;
        }
    }

    return PLATFORM_ERR_OK;
}

/* end of  file -------------------------------------------------------------*/
