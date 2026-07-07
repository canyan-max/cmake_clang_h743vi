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
#include <alloca.h>
#include <stdint.h>
#include <stdio.h>
#include "shell_port.h"
#include "log.h"
#include "kfifo.h"               /* kfifo lib header file. */
#include "storage_handle.h"      /* storage_lib header file. */
#include "bsp_drv_at24.h"        /* at24 driver header file. */
#include "core_dwt.h"            /* dwt header file. */
#include "led_adapter.h"         /* led_adapter lib header file. */
#include "bsp_drv_led.h"         /* bsp_drv_led lib header file. */
#include "led_handle.h"          /* led_handle lib header file. */
#include "st_lcd_spi.h"          /* st_lcd_spi lib header file. */
#include "bsp_drv_st7789.h"      /* bsp_drv_st7789 lib header file. */
#include "draw_adapter.h"        /* draw_adapter lib header file. */
#include "draw_handle.h"         /* draw_handle lib header file. */
#include "st_ov2640.h"           /* st_ov2640 platform ops */
#include "bsp_drv_ov2640.h"      /* ov2640 driver header file. */
#include "ov2640_adapter.h"      /* ov2640_adapter lib header file. */
#include "camera_handle.h"       /* camera_handle lib header file. */
#define MINIMP3_NO_SIMD
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
__attribute__((section(".ram_d2_dma_buffers"),
               aligned(32))) uint8_t g_camera_data_buffer[115200];
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
uint8_t          k_fifo_buffer[16];
kfifo_t          g_kfifo;
at24_driver_t    g_at24c02_drv;
storage_handle_t g_storage_handle;

/* led handle */
const led_handle_ops_t g_led1_adapter_ops = {
    .pf_drv_init  = drv_led_init,
    .pf_led_on    = drv_led_on,
    .pf_led_off   = drv_led_off,
    .pf_led_blink = drv_led_blink
};
led_driver_t                 g_led1_drv;
led_handle_t                 g_led1_handle;
extern const led_operation_t g_led1_ops;
extern const led_operation_t g_led2_ops;

/* draw handle */
st7789_driver_t         g_st7789_drv;
draw_handle_t           g_draw_handle;
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


/* camera handle */
ov2640_driver_t           g_ov2640_drv;
const camera_handle_ops_t g_cam_adapter_ops = {
    .pf_drv_init = drv_cam_init,
    .pf_start    = drv_cam_start,
    .pf_stop     = drv_cam_stop,
};
camera_handle_t g_cam_handle;

mp3dec_t mp3d;
mp3d_sample_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
extern uint8_t Array[];
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
      userShellInit();
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */

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
    // /* draw handle init */
    // {
    //     uint8_t draw_ret = draw_handle_instruct(&g_draw_handle,
    //                                             &g_draw_adapter_ops,
    //                                             (void *)&g_st7789_drv,
    //                                             (void *)&g_st7789_spi_ops);
    //     logInfo("draw_handle instruct ret: %d", draw_ret);
    // }
    // draw_handle_fill_screen(&g_draw_handle, 0x0000U);

    // /* camera handle init: sensor config + DCMI crop setup */
    // {
    //     uint8_t cam_ret = camera_handle_instruct(&g_cam_handle,
    //                                              &g_cam_adapter_ops,
    //                                              (void *)&g_ov2640_drv,
    //                                              (void *)&g_ov2640_hw_ops,
    //                                              OV2640_MODE_SVGA);
    //     logInfo("camera_handle instruct ret: %d", cam_ret);
    // }

    // /* start DCMI DMA continuous capture */
    // {
    //     uint8_t cam_ret = camera_handle_start(&g_cam_handle,
    //                                           (uint32_t *)g_camera_data_buffer,
    //                                           OV2640_DMA_LEN_WORDS);
    //     logInfo("camera_handle start ret: %d", cam_ret);
    // }
    // defaultTaskHandle = xTaskGetCurrentTaskHandle();
    // logInfo("run ..");
    // memcpy(mp3_buffer, Array, sizeof(Array));
    


// logInfo("解码完成，共 %d 帧", total_frames);
// taskEXIT_CRITICAL();
    /* Idle loop */
    for(;;)
    {
mp3dec_init(&mp3d);
    mp3dec_frame_info_t info = {0};
    const uint8_t *input = Array;
    int buf_size = 1024;
    int total_frames = 0;
    
      // __disable_irq();   // ★ 禁用任务切换
    
    while (buf_size > 0) {
        int samples = mp3dec_decode_frame(&mp3d, input, buf_size, pcm, &info);
        if (samples > 0) {
            total_frames++;
            input += info.frame_bytes;
            buf_size -= info.frame_bytes;
        } else if (info.frame_bytes > 0) {
            input += info.frame_bytes;
            buf_size -= info.frame_bytes;
        } else {
            break;
        }
    }
    // __enable_irq();
    
    // logInfo("解码完成，共 %d 帧", total_frames);
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // draw_handle_draw_image(&g_draw_handle, 0U, 0U,
        //                        OV2640_OUT_W, OV2640_OUT_H, g_camera_data_buffer);                             
        logInfo("run ..");
        osDelay(1000);
    }

  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    ((void)hdcmi);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    SCB_InvalidateDCache_by_Addr((uint32_t *)g_camera_data_buffer, 
                                 sizeof(g_camera_data_buffer));
    vTaskNotifyGiveFromISR(defaultTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/* USER CODE END Application */

