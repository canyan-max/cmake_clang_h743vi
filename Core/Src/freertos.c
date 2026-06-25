/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>
#include "shell_port.h"
#include "log.h"
#include "kfifo.h"                                 /* kfifo lib header file. */
#include "storage_handle.h"                      /* storage_lib header file. */
#include "at24cxx_adapter.h"             /* at24cxx_adapter lib header file. */
#include "bsp_drv_at24.h"                        /* at24 driver header file. */
#include "core_dwt.h"                                    /* dwt header file. */
#include "led_adapter.h"                     /* led_adapter lib header file. */
#include "bsp_drv_led.h"                     /* bsp_drv_led lib header file. */
#include "led_handle.h"                       /* led_handle lib header file. */
#include "st_lcd_spi.h"                       /* st_lcd_spi lib header file. */
#include "bsp_drv_st7789.h"               /* bsp_drv_st7789 lib header file. */
#include "front.h"                                 /* front lib header file. */
#include "direct_cur_resistan.h"     /* direct_cur_resistan lib header file. */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* 32x32 RGB565 test image */
static uint8_t sg_test_img[32U * 32U * 2U];
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
uint8_t k_fifo_buffer[16];
kfifo_t g_kfifo;
at24_driver_t g_at24c02_drv;
const storage_ops_t g_storage_ops={
    .pf_drv_init   = drv_init,
    .pf_write_page = drv_write_page,
    .pf_write_cross_page = drv_write_bytes_cross,
    .pf_read_bytes = drv_read_bytes
};
storage_handle_t g_storage_handle;

// led handle
const led_handle_ops_t g_led1_adapter_ops ={
  .pf_drv_init  = drv_led_init,
  .pf_led_on    = drv_led_on,
  .pf_led_off   = drv_led_off,
  .pf_led_blink = drv_led_blink
};
led_driver_t    g_led1_drv;
led_handle_t    g_led1_handle;
extern const led_operation_t g_led1_ops;
extern const led_operation_t g_led2_ops;
st7789_driver_t g_st7789_drv;
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  dwt_init();

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  userShellInit();
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  ((void)argument);
  /* ST7789 LCD test */
  st7789_state_t st7789_ret = st7789_driver_instruct(&g_st7789_drv, \
                                                     &g_st7789_spi_ops);
  storage_handle_instruct(&g_storage_handle, \
                          &g_storage_ops, \
                          &g_at24c02_drv, \
                          0xffU, 8U, \
                          AT24_MEMADD_SIZE_8BIT, \
                          0xA0U);
  led_handle_instruct(&g_led1_handle,&g_led1_adapter_ops, \
                        (void*)&g_led1_drv ,  (void*)&g_led1_ops);
  logInfo("st7789 instruct ret: %d", st7789_ret);
  g_st7789_drv.pf_clear_screen(&g_st7789_drv);
  kfifo_init(&g_kfifo, k_fifo_buffer, sizeof(k_fifo_buffer));
  logInfo("kfifo len %d" , kfifo_len(&g_kfifo));
  logInfo("kfifo avail %d" , kfifo_avail(&g_kfifo));
  uint8_t wdata1[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  uint32_t ret = kfifo_put(&g_kfifo, wdata1, 5);
  logInfo("kfifo_put ret %d" , ret);
  logInfo("kfifo len %d" , kfifo_len(&g_kfifo));
  logInfo("kfifo avail %d" , kfifo_avail(&g_kfifo));
  uint8_t rdata1[5] = {0};
  ret = kfifo_get(&g_kfifo, rdata1, 5);
  logInfo("kfifo_get ret %d" , ret);
  logInfo("kfifo len %d" , kfifo_len(&g_kfifo));
  logInfo("kfifo avail %d" , kfifo_avail(&g_kfifo));
  // write 12 bytes, which is more than the fifo size, to test the boundary condition.
  uint8_t wdata2[] = {0x11, 0x12, 0x13, 0x14, 0x15, \
                      0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C};
  ret = kfifo_put(&g_kfifo, wdata2, 12);
  logInfo("kfifo_put ret %d" , ret);
  logInfo("kfifo len %d" , kfifo_len(&g_kfifo));
  logInfo("kfifo avail %d" , kfifo_avail(&g_kfifo));
  uint8_t rdata[16] = {0};
  ret = kfifo_get(&g_kfifo, rdata, 14);
  logInfo("kfifo_get ret %d" , ret);
  logInfo("kfifo len %d" , kfifo_len(&g_kfifo));
  logInfo("kfifo avail %d" , kfifo_avail(&g_kfifo));
  logInfo("StartDefaultTask is running...");

  /* Title drawn once */
  g_st7789_drv.pf_draw_string(&g_st7789_drv, &g_f8x16,
                                10U, 10U, "ST7789 TEST",
                                0xFFFFU, 0x0000U);

  /* Each frame rotates quadrant colors: TL/TR/BL/BR */
  static const uint16_t sc_pal[4U][4U] = {
      {0xF800U, 0x07E0U, 0x001FU, 0xFFFFU},   /* R lead */
      {0x07E0U, 0x001FU, 0xFFFFU, 0xF800U},   /* G lead */
      {0x001FU, 0xFFFFU, 0xF800U, 0x07E0U},   /* B lead */
      {0xFFFFU, 0xF800U, 0x07E0U, 0x001FU},   /* W lead */
  };
  static const char * const sc_labels[4U] = {
      "RED  ", "GREEN", "BLUE ", "WHITE"
  };
  static uint8_t  s_fi  = 0U;
  static int32_t  s_dec = 0;
  static uint32_t s_hex = 0U;
  static float    s_flt = 0.0f;

  for(;;)
  {
    /* Rebuild image with current frame's palette */
    s_flt = update_bat_resistance(25,1.32f,15);
    for (uint16_t r = 0U; r < 32U; r++)
    {
      for (uint16_t c = 0U; c < 32U; c++)
      {
        uint8_t  q  = (uint8_t)((r / 16U) * 2U + (c / 16U));
        uint16_t px = sc_pal[s_fi][q];
        uint32_t i  = ((uint32_t)r * 32U + c) * 2U;
        sg_test_img[i]      = (uint8_t)(px >> 8U);
        sg_test_img[i + 1U] = (uint8_t)(px & 0x00FFU);
      }
    }
    g_st7789_drv.pf_draw_image(&g_st7789_drv, 10U, 50U, 32U, 32U, sg_test_img);
    g_st7789_drv.pf_draw_string(&g_st7789_drv, &g_f8x16,
                                  10U, 100U, sc_labels[s_fi],
                                  sc_pal[s_fi][0U], 0x0000U);

    /* Decimal — clear 160px wide area then draw */
    // g_st7789_drv.pf_fill_rect(&g_st7789_drv, 10U, 130U, 160U, 16U, 0x0000U);
    g_st7789_drv.pf_draw_dec(&g_st7789_drv, &g_f8x16,
                               10U, 130U, s_dec, 0xFFFFU, 0x0000U);

    /* Hex — yellow */
    // g_st7789_drv.pf_fill_rect(&g_st7789_drv, 10U, 150U, 160U, 16U, 0x0000U);
    g_st7789_drv.pf_draw_hex(&g_st7789_drv, &g_f8x16,
                               10U, 150U, s_hex, 0xFFE0U, 0x0000U);

    /* Float — cyan */
    // g_st7789_drv.pf_fill_rect(&g_st7789_drv, 10U, 170U, 160U, 16U, 0x0000U);
    g_st7789_drv.pf_draw_float(&g_st7789_drv, &g_f8x16,
                                 10U, 170U, s_flt, 2U, 0x07FFU, 0x0000U);

    /* Advance state */
    s_fi  = (uint8_t)((s_fi  + 1U) % 4U);
    s_dec += 137;
    s_hex += 0x1111U;
    

    osDelay(1000U);
    logInfo("dec=%ld hex=0x%lX", (long)s_dec, (unsigned long)s_hex);
  }
  
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

