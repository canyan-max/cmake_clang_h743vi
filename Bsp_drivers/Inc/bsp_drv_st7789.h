/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *                        2026
 *@file               :   bsp_drv_st7789.h
 *
 *@pardependencies    :
 *                        bsp_drv_st7789.h
 *                        xxx.h
 *                        xxxx.h
 *
 *@author             :   null
 *
 *@brief              :   Provide the HAL APIs of ST7789 LCD driver.
 *
 *@version            :   V1.0
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef BSP_DRV_ST7789_H
#define BSP_DRV_ST7789_H

/* Includes -----------------------------------------------------------------*/
#include <stdint.h> /* stdint lib header file. */

/* forward declarations -----------------------------------------------------*/
typedef struct FRONT_DEF_T front_def_t;

/* define -------------------------------------------------------------------*/
#define ST7789_DRIVER_IS_INIT       (0x01U)
#define ST7789_DRIVER_NOT_INIT      (0x00U)

#define ST7789_SCREEN_WIDTH         (240U)
#define ST7789_SCREEN_HEIGHT        (240U)

/* typedef ------------------------------------------------------------------*/
// befor to declar
typedef struct ST7789_SPI_OPS_T st7789_spi_ops_t;
typedef struct ST7789_DRIVER_T  st7789_driver_t;

typedef enum ST7789_STATE_T
{
    ST7789_OK            = 0x00U, // Operation is OK.
    ST7789_ERROR         = 0x01U, // Operation is error.
    ST7789_BUSY          = 0x02U, // Operation is busy.
    ST7789_TIMEOUT       = 0x03U, // Operation is timeout.
    ST7789_INVALID_PARAM = 0x04U, // Operation is invalid parameter.
} st7789_state_t;

typedef struct ST7789_SPI_OPS_T
{
    // SPI transmit raw bytes.
    st7789_state_t (*pf_spi_transmit)(uint8_t *p_data, uint32_t lenth);
    st7789_state_t (*pf_spi_transmit_with_dma)(uint8_t *p_data, uint32_t lenth);
    // Backlight pin: 1=on, 0=off.
    st7789_state_t (*pf_backlight_pin)(uint8_t on);
    // DC pin: 1=command, 0=data.
    st7789_state_t (*pf_dc_pin)(uint8_t on);
    // Delay milliseconds.
    void (*pf_delay_ms)(uint32_t ms);
} st7789_spi_ops_t;

typedef struct ST7789_DRIVER_T
{
    // SPI platform ops.
    const st7789_spi_ops_t *p_spi_ops;
    // Set column/row address window.
    st7789_state_t (*pf_deinit)(st7789_driver_t *p_drv);
    st7789_state_t (*pf_fill_screen)(st7789_driver_t *p_drv, uint16_t color);
    // // Clear screen (fill black).
    // st7789_state_t (*pf_clear_screen)(st7789_driver_t *p_drv);
    // Fill an arbitrary rectangle with a single RGB565 color.
    st7789_state_t (*pf_fill_rect)(st7789_driver_t *p_drv,
                                   uint16_t         x,
                                   uint16_t         y,
                                   uint16_t         w,
                                   uint16_t         h,
                                   uint16_t         color);
    // Draw a single ASCII character.
    st7789_state_t (*pf_draw_char)(st7789_driver_t   *p_drv,
                                   const front_def_t *p_font,
                                   uint16_t           x,
                                   uint16_t           y,
                                   char               ch,
                                   uint16_t           f_color,
                                   uint16_t           b_color);
    // Draw a null-terminated ASCII string (stops at screen edge).
    st7789_state_t (*pf_draw_string)(st7789_driver_t   *p_drv,
                                     const front_def_t *p_font,
                                     uint16_t           x,
                                     uint16_t           y,
                                     const char        *p_str,
                                     uint16_t           f_color,
                                     uint16_t           b_color);
    // Draw a w*h RGB565 big-endian image at (x, y).
    st7789_state_t (*pf_draw_image)(st7789_driver_t *p_drv,
                                    uint16_t         x,
                                    uint16_t         y,
                                    uint16_t         w,
                                    uint16_t         h,
                                    const uint8_t   *p_pixels);
    // Display a signed decimal integer.
    st7789_state_t (*pf_draw_dec)(st7789_driver_t   *p_drv,
                                  const front_def_t *p_font,
                                  uint16_t           x,
                                  uint16_t           y,
                                  int32_t            value,
                                  uint16_t           f_color,
                                  uint16_t           b_color);
    // Display an unsigned hex integer (e.g. "0xFF").
    st7789_state_t (*pf_draw_hex)(st7789_driver_t   *p_drv,
                                  const front_def_t *p_font,
                                  uint16_t           x,
                                  uint16_t           y,
                                  uint32_t           value,
                                  uint16_t           f_color,
                                  uint16_t           b_color);
    // Display a float with `decimals` decimal places.
    st7789_state_t (*pf_draw_float)(st7789_driver_t   *p_drv,
                                    const front_def_t *p_font,
                                    uint16_t           x,
                                    uint16_t           y,
                                    float              value,
                                    uint8_t            decimals,
                                    uint16_t           f_color,
                                    uint16_t           b_color);
    // Draw a line from (x0, y0) to (x1, y1) in RGB565 color.
    st7789_state_t (*pf_draw_line)(st7789_driver_t *p_drv,
                                   uint16_t         x0,
                                   uint16_t         y0,
                                   uint16_t         x1,
                                   uint16_t         y1,
                                   uint16_t         color);
    uint8_t is_init;
} st7789_driver_t;

/* Exported types -----------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* functions ----------------------------------------------------------------*/
/**
  * @brief            :  [st7789_driver_instruct]
                         Initialize the ST7789 driver with SPI ops.
                         Binds all function pointers to the driver instance
                         and executes the hardware init sequence.
  * @retval           :  [   ST7789_OK              = 0x00U,
                             ST7789_ERROR           = 0x01U,
                             ST7789_BUSY            = 0x02U,
                             ST7789_TIMEOUT         = 0x03U,
                             ST7789_INVALID_PARAM   = 0x04U,]
  * @param[in]        :  [st7789_driver_t        *p_drv,
                            const st7789_spi_ops_t *p_ops]
  */
st7789_state_t st7789_driver_instruct(st7789_driver_t        *p_drv,
                                      const st7789_spi_ops_t *p_ops);

#endif /* BSP_DRV_ST7789_H */
