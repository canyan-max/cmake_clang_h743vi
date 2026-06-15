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

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
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
  defaultTaskHandle = osThreadNew(StartDefaultTask, \
                            NULL, \
                            &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  userShellInit();
  // init the at24c02 storage handle.
  storage_handle_instruct(&g_storage_handle, \
                          &g_storage_ops, \
                          &g_at24c02_drv, \
                          0xffU, 8U, \
                          AT24_MEMADD_SIZE_8BIT, \
                          0xA0U);
  led_handle_instruct(&g_led1_handle,&g_led1_adapter_ops, \
                        (void*)&g_led1_drv ,  (void*)&g_led1_ops);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  kfifo_init(&g_kfifo, k_fifo_buffer, sizeof(k_fifo_buffer));
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
    logInfo("StartDefaultTask is running...");

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
    for(uint8_t i = 0; i < ret; i++) 
    {
        logInfo("rdata[%d] = 0x%02X", i, rdata[i]);
    }
  /* Infinite loop */
  for(;;)
  {
    // HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
   uint8_t ret =  led_handle_blink(&g_led1_handle);
   logInfo("blink state of led  %d",ret);
   osDelay(1500);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

