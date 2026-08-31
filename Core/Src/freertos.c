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
#include "kfifo.h"              /* kfifo lib header file. */
#include "plat_sys.h"           /* platform time source. */
#include "service_osd.h"        /* service_osd header file. */
#include "service_key.h"        /* key gesture service (single/double/long) */
#include "service_uart_test.h"  /* for DMA+IDLE UART smoke test */
#include "service_rs485_test.h" /* UART7 master/UART8 slave test */
#include "plat_log.h"           /* platform log header file. */
#include "app_main_task.h"      /* default application task */
// #define MINIMP3_NO_SIMD
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "log.h"
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
    .stack_size = 512 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
uint8_t     k_fifo_buffer[16];
kfifo_t     g_kfifo;
static void BatteryTask(void *argument);
static void UartEchoTestTask(void *argument);
static void Rs485TestTask(void *argument);
static void KeyTestTask(void *argument);
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
    platform_err_t time_init_status = plat_time_init();
    if(PLATFORM_ERR_OK == plat_log_init())
    {
        plat_log_i("SYS", "log init OK (build %s %s)", __DATE__, __TIME__);
        if(PLATFORM_ERR_OK != time_init_status)
        {
            plat_log_e("SYS", "DWT time init failed: %d",
                       (int)time_init_status);
        }
    }
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
    userShellInit();

    // static const osThreadAttr_t battery_task_attr = {
    //     .name       = "batteryTask",
    //     .stack_size = 512U,
    //     .priority   = (osPriority_t)osPriorityBelowNormal,
    // };
    // osThreadNew(BatteryTask, NULL, &battery_task_attr);

    static const osThreadAttr_t uart_test_task_attr = {
        .name       = "uartTestTask",
        .stack_size = 512 * 4U,
        .priority   = (osPriority_t)osPriorityBelowNormal,
    };
    // osThreadNew(UartEchoTestTask, NULL, &uart_test_task_attr);

    static const osThreadAttr_t rs485_test_task_attr = {
        .name       = "rs485TestTask",
        .stack_size = 512U * 4U,
        .priority   = (osPriority_t)osPriorityBelowNormal,
    };
    (void)osThreadNew(Rs485TestTask, NULL, &rs485_test_task_attr);

    static const osThreadAttr_t key_test_task_attr = {
        .name       = "keyTestTask",
        .stack_size = 512U * 2U,
        .priority   = (osPriority_t)osPriorityNormal,
    };
    (void)osThreadNew(KeyTestTask, NULL, &key_test_task_attr);
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
    app_main_task(argument);
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
 * @brief  Simulates battery drain: decreases level by 1% every 2 seconds.
 *         Calls service_osd_set_battery() — no display logic here.
 */
static void BatteryTask(void *argument)
{
    ((void)argument);
    uint8_t bat = 100U;
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(5000U));
        bat = (bat > 0U) ? (bat - 1U) : 100U;
        service_osd_set_battery(bat);
    }
}
static TaskHandle_t s_uart_task_handle;
static TaskHandle_t s_rs485_task_handle;

/* service_uart_test's RTOS wake mechanism: forwards its ISR-context wake
 * request to a task notify. Kept here (not in service_uart_test.c) so that
 * module stays free of RTOS types and is portable to bare metal. */
static void uart_wake_isr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(s_uart_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* service_rs485_test's RTOS wake mechanism.  No parsing or blocking work is
 * performed from this ISR callback. */
static void rs485_wake_isr(void)
{
    BaseType_t   higher_priority_task_woken = pdFALSE;
    TaskHandle_t task_handle                = s_rs485_task_handle;
    if(NULL != task_handle)
    {
        vTaskNotifyGiveFromISR(task_handle, &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }
}

/* Mirror RS485 service events to the LetterShell COM port.  This callback is
 * called by the service only from Rs485TestTask context; the service itself
 * remains independent of LetterShell and HAL. */
static void rs485_event_log(const service_rs485_test_event_t *p_event)
{
    if(NULL == p_event)
    {
        return;
    }

    const char *endpoint = (SERVICE_RS485_TEST_ENDPOINT_MASTER ==
                            p_event->endpoint)
                               ? "master"
                               : "slave";
    switch(p_event->event)
    {
        case SERVICE_RS485_TEST_EVENT_INIT_OK:
            logInfo("modbus RTU 03/06 init ok");
            break;
        case SERVICE_RS485_TEST_EVENT_INIT_ERROR:
            logError("modbus %s init error status=%u", endpoint,
                     (unsigned int)p_event->status);
            break;
        case SERVICE_RS485_TEST_EVENT_MASTER_WRITE_REQUEST:
            logInfo("modbus master FC06 write reg=0x%04X value=%u",
                    (unsigned int)p_event->register_address,
                    (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_SLAVE_WRITE_APPLIED:
            logInfo("modbus slave FC06 applied reg=0x%04X value=%u",
                    (unsigned int)p_event->register_address,
                    (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_MASTER_WRITE_OK:
            logInfo("modbus master FC06 response ok value=%u",
                    (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_MASTER_READ_REQUEST:
            logInfo("modbus master FC03 read reg=0x%04X expected=%u",
                    (unsigned int)p_event->register_address,
                    (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_SLAVE_READ_REPLY:
            logInfo("modbus slave FC03 reply reg=0x%04X value=%u",
                    (unsigned int)p_event->register_address,
                    (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_SLAVE_EXCEPTION_REPLY:
            logWarning("modbus slave FC%02X exception reply=%u",
                       (unsigned int)p_event->function,
                       (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_MASTER_READ_OK:
            logInfo("modbus master FC03 readback ok value=%u",
                    (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_RESPONSE_TIMEOUT:
            logWarning("modbus FC%02X response timeout",
                       (unsigned int)p_event->function);
            break;
        case SERVICE_RS485_TEST_EVENT_SEND_ERROR:
            logError("modbus %s FC%02X send error status=%u", endpoint,
                     (unsigned int)p_event->function,
                     (unsigned int)p_event->status);
            break;
        case SERVICE_RS485_TEST_EVENT_PROTOCOL_ERROR:
            logWarning("modbus %s FC%02X protocol error status=%u exception=%u",
                       endpoint, (unsigned int)p_event->function,
                       (unsigned int)p_event->status,
                       (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_VALUE_MISMATCH:
            logError("modbus FC03 mismatch expected=%u actual=%u",
                     (unsigned int)p_event->status,
                     (unsigned int)p_event->value);
            break;
        case SERVICE_RS485_TEST_EVENT_FRAME_TIMEOUT:
            logWarning("modbus %s frame timeout", endpoint);
            break;
        case SERVICE_RS485_TEST_EVENT_RX_OVERRUN:
            logWarning("modbus %s RX overrun dropped=%u", endpoint,
                       (unsigned int)p_event->status);
            break;
        default:
            logWarning("rs485 unknown event=%u", (unsigned int)p_event->event);
            break;
    }
}

/**
 * @brief  DMA+IDLE + proto_simple smoke test: parses framed data received
 *         on BSP_UART_PROTO_1 (bound to hlpuart1 for bring-up) and echoes
 *         the decoded payload back.
 */
static void UartEchoTestTask(void *argument)
{
    ((void)argument);
    /* set before service_uart_test_init() registers uart_wake_isr as the RX
     * notify callback, so it's never called with a still-NULL handle. */
    s_uart_task_handle = xTaskGetCurrentTaskHandle();
    (void)service_uart_test_init(uart_wake_isr);
    for(;;)
    {
        /* bounded wait (not portMAX_DELAY): also wakes with no new data so
         * service_uart_test_poll()'s mid-frame idle-timeout watchdog runs
         * even if the sender stalls and no further RX notify ever comes. */
        ulTaskNotifyTake(pdTRUE,
                         pdMS_TO_TICKS(SERVICE_UART_TEST_IDLE_TIMEOUT_MS));
        service_uart_test_poll();
    }
}

/**
 * @brief Periodically drives the UART7 master/UART8 slave RS485 test.
 * @param argument Unused.
 */
static void Rs485TestTask(void *argument)
{
    ((void)argument);
    s_rs485_task_handle   = xTaskGetCurrentTaskHandle();
    platform_err_t status = service_rs485_test_init(rs485_wake_isr,
                                                    rs485_event_log);
    if(PLATFORM_ERR_OK != status)
    {
        logError("service_rs485_test_init failed: %u", (unsigned int)status);
        vTaskDelete(NULL);
        return;
    }

    for(;;)
    {
        (void)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(
                                           SERVICE_RS485_TEST_POLL_PERIOD_MS));
        service_rs485_test_poll();
    }
}
/**
 * @brief  Forward a detected key gesture to the main application task and log
 *         it for observation. Runs in KeyTestTask context.
 */
static void key_event_log(uint8_t key_id, service_key_event_t event)
{
    app_main_post_status_t status = app_main_post_key_event(key_id, event);
    if(APP_MAIN_POST_OK != status)
    {
        logError("key%u event dropped: %u", key_id, (unsigned int)status);
        return;
    }

    switch(event)
    {
        case SERVICE_KEY_EVENT_CLICK:
            logInfo("key%u click", key_id);
            break;
        case SERVICE_KEY_EVENT_DOUBLE_CLICK:
            logInfo("key%u double-click", key_id);
            break;
        case SERVICE_KEY_EVENT_LONG_PRESS:
            logInfo("key%u long-press", key_id);
            break;
        default:
            break;
    }
}

/**
 * @brief  Non-blocking key gesture test: polls the key service every
 *         SERVICE_KEY_POLL_PERIOD_MS and logs click / double-click /
 * long-press. No blocking waits inside the key service itself.
 */
static void KeyTestTask(void *argument)
{
    ((void)argument);
    platform_err_t error = service_key_init(key_event_log);
    if(PLATFORM_ERR_OK != error)
    {
        logError("service_key_init failed: %u", (unsigned int)error);
        vTaskDelete(NULL);
        return;
    }

    for(;;)
    {
        error = service_key_poll();
        if(PLATFORM_ERR_OK != error)
        {
            logError("service_key_poll failed: %u", (unsigned int)error);
            vTaskDelete(NULL);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(SERVICE_KEY_POLL_PERIOD_MS));
    }
}

/* USER CODE END Application */
