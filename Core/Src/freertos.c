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
#include "i2c.h"
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
osThreadId_t         defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name       = "defaultTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

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
#define OV2640_I2C_ADDR    (0x60U)
#define I2C_TIMEOUT        (100U)

/*  SVGA 分辨率为800*600，最高支持30帧 */
const uint8_t OV2640_SVGA_Config[][2] = {

    {0xff, 0x01}, // 设置 sensor 寄存器组
    {0x12, 0x80}, // 复位全部寄存

    {0xff, 0x00}, // 设置DSP寄存器租

    {0x2c, 0xff}, // 手册里没有说明该寄存器的功能

    {0x2e, 0xdf},

    {0xff, 0x01}, // 设置 sensor 寄存器组

    // 手册里没有说明这些寄存器的作用,这里直接保留官方给的设置参数
    {0x3c, 0x32},
    {0x2c, 0x0c},
    {0x33, 0x78},
    {0x3a, 0x33},
    {0x3b, 0xfB},
    {0x3e, 0x00},
    {0x43, 0x11},
    {0x16, 0x10},
    {0x39, 0x92},
    {0x35, 0xda},
    {0x22, 0x1a},
    {0x37, 0xc3},
    {0x36, 0x1a},
    {0x4c, 0x00},
    {0x5B, 0x00},
    {0x42, 0x03},
    {0x4a, 0x81},
    {0x21, 0x99},
    {0x5c, 0x00},
    {0x63, 0x00},
    {0x7c, 0x05},
    {0x6c, 0x00},
    {0x6d, 0x80},
    {0x6e, 0x00},
    {0x70, 0x02},
    {0x71, 0x94},
    {0x73, 0xc1},
    {0x20, 0x80},
    {0x28, 0x30},
    {0x37, 0xc0},
    {0x3d, 0x38},
    {0x6d, 0x00},
    {0x23, 0x00},
    {0x06, 0x88},
    {0x07, 0xc0},
    {0x0d, 0x87},
    {0x0e, 0x41},

    // 该寄存器用于控制OV2640的系统时钟是否倍频或者分频
    // Bit[7]：  设置为1时,开启PLL倍频,系统时钟 = 2倍的 XVCLK
    // Bit[5:0]：分频系数, 系统时钟 = XVCLK / (Bit[5:0] + 1)
    // 我们的模块使用的 XVCLK 是24M,是OV2640默认的时钟,因此不用调整,系统时钟 =
    // XVCLK =24M
    {0x11, 0x00}, // CLKRC,时钟分频控制

    {0x09, 0x02}, // COM2,公共控制,输出驱动能力选择
    {0x04, 0x28}, // REG04,寄存器组4,可设置摄像头扫描方向等
    // COM7,公共控制,系统复位摄像头分辨率选择、缩放模式、颜色彩条设置
    {0x12, 0x40},
    {0x14, 0x48}, // COM9,公共控制,增益设置
    {0x15, 0x00}, // COM10,公共控制,PCLK、HS、VS输出极性控制
    // REG32,寄存器组32,像素时钟分频以及水平起始、终止像素的（低3位）
    {0x32, 0x09},
    {0x03, 0x8a}, // COM1,公共控制,无效帧设置、垂直窗口起始、结束行（低2位）
    {0x46, 0x00}, // FLL,帧率长度调整,通过插入空行来降低帧率,也可以通过
                  // 0x2a/0x2b/0x47等寄存器去调整
    {0x24, 0x40}, // AEW,环境平均亮度大于AEW(7:0)时,AEC/AGC值将降低
    {0x25, 0x38}, // AEB,环境平均亮度小于AEB(7:0)时,AEC/AGC值将增加
    {0x26, 0x82}, // VV,快速模式步进阈值
    {0x34, 0xc0}, // ARCOM2,缩放窗口水平起始像素
    {0x61, 0x70}, // HISTO_LOW ,低等级直方图算法
    {0x62, 0x80}, // HISTO_HIGH,高等级直方图算法

    {0x17, 0x11}, // HREFST,水平窗口起始像素（高8位）,默认值0x11
    {0x18, 0x43}, // HREFEND,水平窗口终止像素（高8位）,UXGA默认值 0x75,
                  // SVGA和CIF默认值0x43
    // VSTRT,垂直窗口起始行（高8位）,数据手册建议的配置是：UXGA为 0x01,
    // SVGA和CIF模式为 0x00 在OpenMV的配置中,不管什么模式都建议配置成
    // 0x01,代码的解释是解决垃圾像素的问题。
    // 在笔者实际的测试中,如果配置成0x00,发现在图像垂直翻转的时候会有一行显示不对,应该就是openMV所说的垃圾像素
    // 因此这里也直接配置成 0x01,问题解决
    {0x19, 0x01}, // VSTRT,垂直窗口起始行（高8位）
    {0x1a, 0x97}, // VEND, 垂直窗口结束行（高8位）,默认值 0x97

    // 以下5个寄存器，共同决定了光带滤除的效果（室内照明灯具开关频率是50HZ，对于传感器而言，会捕捉到明暗交错的光带）
    // 用户可以结合手册，根据实际场景去配置，以达到最佳的光带滤除效果
    {0x13, 0xe5}, // COM8,公共控制,曝光、自动增益、滤波设置
    {0x0c, 0x3a}, // COM3,公共控制,自动或手动设置带宽、快照和视频输出配置
    {0x4f, 0xbb}, // BD50,50Hz带宽 AEC低8位
    {0x50, 0x9c}, // BD60,60HZ带宽 AEC低8位
    {0x5a, 0x23}, // 手册没有说明该寄存器的作用，参考手册进行配置

    {0xff, 0x00}, // 设置DSP寄存器租

    // 手册里没有说明这些寄存器的作用,这里直接保留官方给的设置参数  LXB
    {0xe5, 0x7f},
    {0x41, 0x24},
    {0x76, 0xff},
    {0x33, 0xa0},
    {0x42, 0x20},
    {0x43, 0x18},
    {0x4c, 0x00},
    {0xd7, 0x03},
    {0xd9, 0x10},
    {0x88, 0x3f},
    {0xc8, 0x08},
    {0xc9, 0x80},
    {0x7c, 0x00},
    {0x7d, 0x00},
    {0x7c, 0x03},
    {0x7d, 0x48},
    {0x7d, 0x48},
    {0x7c, 0x08},
    {0x7d, 0x20},
    {0x7d, 0x10},
    {0x7d, 0x0e},
    {0x90, 0x00},
    {0x91, 0x0e},
    {0x91, 0x1a},
    {0x91, 0x31},
    {0x91, 0x5a},
    {0x91, 0x69},
    {0x91, 0x75},
    {0x91, 0x7e},
    {0x91, 0x88},
    {0x91, 0x8f},
    {0x91, 0x96},
    {0x91, 0xa3},
    {0x91, 0xaf},
    {0x91, 0xc4},
    {0x91, 0xd7},
    {0x91, 0xe8},
    {0x91, 0x20},
    {0x92, 0x00},
    {0x93, 0x06},
    {0x93, 0xe3},
    {0x93, 0x05},
    {0x93, 0x05},
    {0x93, 0x00},
    {0x93, 0x04},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x96, 0x00},
    {0x97, 0x08},
    {0x97, 0x19},
    {0x97, 0x02},
    {0x97, 0x0c},
    {0x97, 0x24},
    {0x97, 0x30},
    {0x97, 0x28},
    {0x97, 0x26},
    {0x97, 0x02},
    {0x97, 0x98},
    {0x97, 0x80},
    {0x97, 0x00},
    {0x97, 0x00},
    {0xa4, 0x00},
    {0xa8, 0x00},
    {0xc5, 0x11},
    {0xc6, 0x51},
    {0xbf, 0x80},
    {0xc7, 0x10},
    {0xb6, 0x66},
    {0xb8, 0xA5},
    {0xb7, 0x64},
    {0xb9, 0x7C},
    {0xb3, 0xaf},
    {0xb4, 0x97},
    {0xb5, 0xFF},
    {0xb0, 0xC5},
    {0xb1, 0x94},
    {0xb2, 0x0f},
    {0xc4, 0x5c},
    {0x7f, 0x00},

    {0xf9, 0xc0}, // MC_BIST,控制器复位、ROM选择
    {0xe0, 0x14}, // RESET,可选择复位 控制器、SCCB单元、JPEG单元、DVP接口单元等
    {0x87, 0xd0}, // CTRL3,使能芯片内部的指定的模块
    {0xc3, 0xed}, // CTRL1,使能芯片内部的指定的模块
    {0xc2, 0x0e}, // CTRL0,使能YUV422、YUV_EN、RGB_EN
    {0x86, 0x3d}, // CTRL2,使能芯片内部的指定的模块
    // 图像输出模式,可设置JPEG输出、RGB565等,可设置是否翻转DVP接口的输出
    {0xda, 0x09},

    // 此处设置的是传感器的图像尺寸,与配置的模式有关,例如SVGA需要设置成800*480,XVGA要设置成1600*1200
    {0xc0, 0x64}, // 图像的水平尺寸,10~3 bit
    {0xc1, 0x4b}, // 图像的垂直尺寸,10~3 bit
    {0x8c, 0x00}, // 图像水平尺寸的第 11 bit以及2~0bit,图像垂直尺寸的 2~0bit

    {0x50, 0x00}, // CTRLI,设置 水平和垂直分频器
    {0x51, 0xc8}, // 水平尺寸,7~0 bit,必须要能被4整除
    {0x52, 0x96}, // 垂直尺寸,7~0 bit,必须要能被4整除
    {0x53, 0x00}, // 水平偏移,7~0 bit,
    {0x54, 0x00}, // 垂直偏移,7~0 bit,
    {0x55, 0x00}, // 水平、垂直尺寸的第 8 bit, 水平、垂直偏移的 第 10~8 bit
    {0x57, 0x00}, // 水平尺寸的第 9 bit

    // 0xd3 寄存器用于设置像素驱动时钟,即 PCLK 的输出频率，有自动模式和手动模式
    //	在自动模式下，PCLK的频率会非常高，适用于高分辨率或者有高速缓存的场合，用户可根据实际工况选择最合适的模式
    //
    // Bit[7]：  设置为1时,开启自动模式,此时 PCLK
    // 由OV2640自动控制,该模式下的PCLK频率非常高，
    //           适用于高分辨率或者有高速缓存的场合
    //
    // Bit[6:0]：手动设置分频系数,当设置成YUV模式或RGB565模式时, PCLK = sysclk /
    // Bit[6:0] ,
    //           当 时钟控制寄存器 0x11（CLKRC）设置为 不分频
    //           且外部输入时钟为24M时， 在SVGA模式下， PCLK = 2*24M / Bit[6:0]
    //	LXB
    // 如果要手动配置,用户需要根据实际输出的图像尺寸去计算具体的 PCLK ,这里以
    // 480*360 分辨率为例： 一帧 RGB565（16位色） 图像的数据量为：480*360*2 =
    // 345.6 KB , OV2640在 SVGA模式下,帧率为30左右,则每秒的数据量在 345.6 * 30
    // = 10.4 MB 左右, 因为模块是8位的接口,则PCLK最少要设置为 10.4
    // MHz,才能满足图像传输的需求,不然会导致花屏  ,
    // 加上OV2640的帧率是可以微调的,因此实际的 PCLK 要稍微大些,此处设置为 12M，
    // 即  PCLK = 48M / Bit[6:0] = 48 / 0x04 = 12M
    {0xd3, 0x04}, // R_DVP_SP, 设置 PCLK 引脚的时钟

    // 手册里没有说明这些寄存器的作用,这里直接保留官方给的设置参数
    {0xe5, 0x1f},
    {0xe1, 0x67},
    {0xdd, 0x7f},

    {0xe0, 0x00}, // RESET,可选择复位 控制器、SCCB单元、JPEG单元、DVP接口单元等
    {0x05, 0x00}, // 使能DSP

    // 以下为OpenMV增加的设置,0x0f寄存器在数据手册里没有说明,但是在编程手册4.2小节有提到,
    // 按照编程手册给的代码,
    // 0x0f这个寄存器是自动控制帧率用的,但是手册里只有在夜晚模式时才进行配置
    // {0xff, 0x01},   // 设置 sensor 寄存器组
    //	{0x0f, 0x4b},
    // LXB

    {0, 0}, // 结束
};
HAL_StatusTypeDef ov2640_write_reg(uint8_t reg_addr, uint8_t reg_val)
{
    uint8_t           arr[2] = {reg_addr, reg_val};
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, OV2640_I2C_ADDR,
                                                       arr, 2, I2C_TIMEOUT);
    if(status != HAL_OK)
    {
        return status;
    }
    return status;
}

HAL_StatusTypeDef ov2640_read_reg(uint8_t reg_addr, uint8_t *p_reg_val)
{

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, OV2640_I2C_ADDR,
                                                       &reg_addr, 1,
                                                       I2C_TIMEOUT);
    if(status != HAL_OK)
        return status;

    return HAL_I2C_Master_Receive(&hi2c1, OV2640_I2C_ADDR, p_reg_val, 1,
                                  I2C_TIMEOUT);
}
uint8_t ov2640_Config(const uint8_t (*ConfigData)[2])
{
    uint32_t i;

    for(i = 0; ConfigData[i][0]; i++)
    {
        HAL_StatusTypeDef ret = ov2640_write_reg(ConfigData[i][0],
                                                 ConfigData[i][1]);
        if(HAL_OK != ret)
        {
            return 1;
        }
    }
    return 0;
}
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
    ov2640_write_reg(0xffU, 0x01U);
    ov2640_write_reg(0x12U, 0x80U);
    HAL_Delay(20);
    ov2640_write_reg(0xffU, 0x01U);
    uint8_t id_l = 0, id_h = 0;
    ov2640_read_reg(0x0AU, &id_h);
    ov2640_read_reg(0x0BU, &id_l);
    uint16_t id = (id_h << 8) | id_l;
    // ov2640_Config(OV2640_SVGA_Config);
    // HAL_StatusTypeDef ret = ov2640_write_reg(0xffU, 0x00U);
    // if(HAL_OK != ret)
    // {
    //     while(1)
    //     {
    //     }
    // }
    // ov2640_write_reg(0x12U, 0x40U);
    // if(HAL_OK != ret)
    // {
    //     while(1)
    //     {
    //     }
    // }
    uint8_t d3_reg = 55;
    ov2640_read_reg(0x2c, &d3_reg);
    /* Build DeepSeek whale icon: 32x32 @ (0,0), head right, tail left */
    /* Idle loop */
    for(;;)
    {

        float internal_re = 0.0f;
        internal_re       = update_bat_resistance(25, 5.21f, 15);
        draw_handle_draw_hex(&g_draw_handle, &g_f8x16, 20, 20, (uint32_t)id,
                             RGB565_GREEN, RGB565_BLACK);
        draw_handle_draw_hex(&g_draw_handle, &g_f8x16, 60, 60, (uint32_t)d3_reg,
                             RGB565_GREEN, RGB565_BLACK);
        draw_handle_draw_float(&g_draw_handle, &g_f8x16, 40, 40, internal_re, 2,
                               RGB565_GREEN, RGB565_BLACK);
        osDelay(500);
    }

    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
