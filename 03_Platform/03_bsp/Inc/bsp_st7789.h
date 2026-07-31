/**
 ******************************************************************************
 *@file               :   bsp_st7789.h
 *@brief              :   ST7789 driver 鈥?protocol functions + transport abstraction.
 *                        Transport is the ONLY replaceable part (SPI/DC/backlight/delay).
 *                        Protocol functions are direct calls, not function pointers.
 *@version            :   V2.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_DRV_ST7789_H
#define BSP_DRV_ST7789_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/* ---- forward declarations ------------------------------------------------ */
typedef struct FRONT_DEF_T     front_def_t;
typedef struct ST7789_DEV_T    st7789_dev_t;

/* ---- defines ------------------------------------------------------------- */
#define ST7789_DRIVER_IS_INIT   (0x01U)
#define ST7789_DRIVER_NOT_INIT  (0x00U)
#define ST7789_SCREEN_WIDTH     (240U)
#define ST7789_SCREEN_HEIGHT    (240U)

/* ---- state enum ---------------------------------------------------------- */
typedef enum
{
    ST7789_OK            = 0x00U,
    ST7789_ERROR         = 0x01U,
    ST7789_BUSY          = 0x02U,
    ST7789_TIMEOUT       = 0x03U,
    ST7789_INVALID_PARAM = 0x04U,
} st7789_state_t;

/* ---- transport (the only replaceable part) ------------------------------- */
typedef struct ST7789_SPI_OPS_T
{
    st7789_state_t (*pf_spi_transmit)(uint8_t *p_data, uint32_t len);
    st7789_state_t (*pf_spi_transmit_with_dma)(uint8_t *p_data, uint32_t len);
    st7789_state_t (*pf_dc_pin)(uint8_t on);            /* 1=cmd, 0=data */
    st7789_state_t (*pf_backlight_pin)(uint8_t on);     /* 1=on, 0=off */
    void           (*pf_delay_ms)(uint32_t ms);
} st7789_spi_ops_t;

/* ---- device instance ----------------------------------------------------- */
struct ST7789_DEV_T
{
    const st7789_spi_ops_t *p_transport;
    uint8_t                   is_init;
};

/* ---- protocol functions (direct calls, no ops indirection) --------------- */
st7789_state_t st7789_fill_screen(st7789_dev_t *p_dev, uint16_t color);
st7789_state_t st7789_fill_rect(st7789_dev_t *p_dev,
                                 uint16_t x, uint16_t y,
                                 uint16_t w, uint16_t h, uint16_t color);
st7789_state_t st7789_draw_line(st7789_dev_t *p_dev,
                                 uint16_t x0, uint16_t y0,
                                 uint16_t x1, uint16_t y1, uint16_t color);
st7789_state_t st7789_draw_char(st7789_dev_t     *p_dev,
                                 const front_def_t *p_font,
                                 uint16_t x, uint16_t y, char ch,
                                 uint16_t f_color, uint16_t b_color);
st7789_state_t st7789_draw_string(st7789_dev_t     *p_dev,
                                   const front_def_t *p_font,
                                   uint16_t x, uint16_t y,
                                   const char *p_str,
                                   uint16_t f_color, uint16_t b_color);
st7789_state_t st7789_draw_image(st7789_dev_t   *p_dev,
                                  uint16_t x, uint16_t y,
                                  uint16_t w, uint16_t h,
                                  const uint8_t *p_pixels);
st7789_state_t st7789_draw_dec(st7789_dev_t     *p_dev,
                                const front_def_t *p_font,
                                uint16_t x, uint16_t y,
                                int32_t value,
                                uint16_t f_color, uint16_t b_color);
st7789_state_t st7789_draw_hex(st7789_dev_t     *p_dev,
                                const front_def_t *p_font,
                                uint16_t x, uint16_t y,
                                uint32_t value,
                                uint16_t f_color, uint16_t b_color);
st7789_state_t st7789_draw_float(st7789_dev_t     *p_dev,
                                  const front_def_t *p_font,
                                  uint16_t x, uint16_t y,
                                  float value, uint8_t decimals,
                                  uint16_t f_color, uint16_t b_color);
st7789_state_t st7789_deinit(st7789_dev_t *p_dev);

/* ---- board-level init (wires Impl transport + hw reset) ------------------ */
st7789_state_t bsp_st7789_init(st7789_dev_t *p_dev);

#ifdef __cplusplus
}
#endif

#endif /* BSP_DRV_ST7789_H */
