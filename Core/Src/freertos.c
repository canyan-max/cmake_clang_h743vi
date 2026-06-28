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
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "shell_port.h"
#include "log.h"
#include "kfifo.h"               /* kfifo lib header file. */
#include "storage_handle.h"      /* storage_lib header file. */
#include "at24cxx_adapter.h"     /* at24cxx_adapter lib header file. */
#include "bsp_drv_at24.h"        /* at24 driver header file. */
#include "core_dwt.h"            /* dwt header file. */
#include "led_adapter.h"         /* led_adapter lib header file. */
#include "bsp_drv_led.h"         /* bsp_drv_led lib header file. */
#include "led_handle.h"          /* led_handle lib header file. */
#include "st_lcd_spi.h"          /* st_lcd_spi lib header file. */
#include "bsp_drv_st7789.h"      /* bsp_drv_st7789 lib header file. */
#include "front.h"               /* front lib header file. */
#include "direct_cur_resistan.h" /* direct_cur_resistan lib header file. */
#include "draw_adapter.h"        /* draw_adapter lib header file. */
#include "draw_handle.h"         /* draw_handle lib header file. */
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
/* USER CODE END Variables */
/* Definitions for defaultTask */
const storage_ops_t g_storage_ops = {

    .pf_drv_init         = drv_init,
    .pf_write_page       = drv_write_page,
    .pf_write_cross_page = drv_write_bytes_cross,
    .pf_read_bytes       = drv_read_bytes

};
const osThreadAttr_t defaultTask_attributes = {

    .name       = "defaultTask",
    .stack_size = 512 * 4,
    .priority   = (osPriority_t)osPriorityNormal,

};
osThreadId_t defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

uint8_t          k_fifo_buffer[16];
kfifo_t          g_kfifo;
at24_driver_t    g_at24c02_drv;
storage_handle_t g_storage_handle;

// led handle
const led_handle_ops_t g_led1_adapter_ops = {

    .pf_drv_init  = drv_led_init,
    .pf_led_on    = drv_led_on,
    .pf_led_off   = drv_led_off,
    .pf_led_blink = drv_led_blink

};

extern const led_operation_t g_led1_ops;
extern const led_operation_t g_led2_ops;
st7789_driver_t              g_st7789_drv;
led_driver_t                 g_led1_drv;
led_handle_t                 g_led1_handle;

/* draw handle */
const draw_handle_ops_t g_draw_adapter_ops = {

    .pf_drv_init    = drv_draw_init,
    .pf_fill_screen = drv_draw_fill_screen,
    .pf_fill_rect   = drv_draw_fill_rect,
    .pf_draw_line   = drv_draw_line,
    .pf_draw_string = drv_draw_string,
    .pf_draw_image  = drv_draw_image,
    .pf_draw_dec    = drv_draw_dec,
    .pf_draw_hex    = drv_draw_hex,
    .pf_draw_float  = drv_draw_float

};
draw_handle_t g_draw_handle;
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
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

    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL,
                                    &defaultTask_attributes);
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

    /* draw handle init (replaces direct st7789_driver_instruct) */
    {
        uint8_t draw_ret = draw_handle_instruct(&g_draw_handle,
                                                &g_draw_adapter_ops,
                                                (void *)&g_st7789_drv,
                                                (void *)&g_st7789_spi_ops);
        logInfo("draw_handle instruct ret: %d", draw_ret);
    }
    draw_handle_fill_screen(&g_draw_handle, 0x0000U);
    /* Build DeepSeek whale icon: 32x32 @ (0,0), head right, tail left */
    /* Idle loop */
    for(;;)
    {
        float internal_re = 0.0f;
        internal_re       = update_bat_resistance(25, 5.21f, 15);
        draw_handle_draw_float(&g_draw_handle, &g_f8x16, 20, 20, internal_re, 2,
                               RGB565_GREEN, RGB565_BLACK);
    }

    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
