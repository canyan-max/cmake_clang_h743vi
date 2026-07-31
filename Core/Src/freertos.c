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
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "shell_port.h"
#include "log.h"
#include "kfifo.h"            /* kfifo lib header file. */
#include "core_dwt.h"         /* dwt header file. */
#include "service_display.h"   /* service_display header file. */
#include "service_camera.h"    /* service_camera header file. */
#include "service_indicator.h" /* service_indicator header file. */
#include "service_storage.h"   /* service_storage header file. */
#include "service_osd.h"       /* service_osd header file. */
// #define MINIMP3_NO_SIMD
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
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
/* FreeRTOS heap placed in AXI SRAM (512KB) instead of DTCMRAM (128KB) */
__attribute__((section(".ram_dma_buffers"),
               aligned(8))) uint8_t ucHeap[configTOTAL_HEAP_SIZE];
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t         defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name       = "defaultTask",
    .stack_size = 8192 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
uint8_t k_fifo_buffer[16];
kfifo_t g_kfifo;
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
    portTASK_USES_FLOATING_POINT();
    ((void)argument);

    if(PLATFORM_ERR_OK != service_indicator_init())
    {
        logError("indicator init failed");
    }
    if(PLATFORM_ERR_OK != service_storage_init())
    {
        logError("storage init failed");
    }
    if(PLATFORM_ERR_OK != service_display_init())
    {
        logError("display init failed");
    }
    if(PLATFORM_ERR_OK != service_camera_init())
    {
        logError("camera init failed");
    }
    if(PLATFORM_ERR_OK != service_camera_start())
    {
        logError("camera start failed");
    }
    if(PLATFORM_ERR_OK != service_osd_init())
    {
        logError("osd init failed");
    }
    service_osd_set_rec_state(SERVICE_OSD_REC_ACTIVE);

    defaultTaskHandle = xTaskGetCurrentTaskHandle();
    logInfo("run ..");

    /* Idle loop */
    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        service_osd_render();
        service_display_show_frame(service_camera_get_buffer(),
                                    0, OSD_BAR_HEIGHT, 240U, 240U - OSD_BAR_HEIGHT);
        service_indicator_blink(DEVICE_INDICATOR_1);
        service_indicator_blink(DEVICE_INDICATOR_2);
        vTaskDelay(10);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    ((void)hdcmi);
    service_camera_frame_isr();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(defaultTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/* USER CODE END Application */
