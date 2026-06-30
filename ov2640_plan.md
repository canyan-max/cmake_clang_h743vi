# OV2640 RGB565 硬件验证计划

> **目标**：在现有 STM32H743 + DCMI + DMA 基础上，驱动 OV2640 以 240×240 RGB565
> 格式实时预览到 ST7789（240×240）屏幕。
>
> **策略**：全程平坦代码，跑通后再迁移四层架构。

---

## 当前已知条件

| 项目 | 状态 |
|------|------|
| OV2640 时钟 | 模块板载晶振，无需 STM32 提供 XCLK |
| I2C / SCCB | 通路通，设备 ID 可读（PID_H=0x26，PID_L=0x42） |
| DCMI | CubeMX 已配置（8bit，硬件同步，非JPEG） |
| DMA | DMA2_Stream7，循环模式，字对齐 |
| 帧缓冲区域 | 链接脚本已有 `.ram_dma_buffers` 段 → AXI SRAM 0x24000000 |
| 输出目标 | ST7789 240×240，已有 `pf_draw_image` |

---

## 新增文件清单

| 文件 | 说明 |
|------|------|
| `Core/Inc/ov2640.h` | 寄存器地址宏 + 初始化函数声明 |
| `Core/Src/ov2640.c` | 三张寄存器表 + `ov2640_init()` 实现 |
| `Core/Inc/camera.h` | 帧缓冲定义、尺寸宏、标志位声明 |
| `Core/Src/camera.c` | DCMI 启动、帧回调、缓存处理 |

已有文件改动：

| 文件 | 改动 |
|------|------|
| `Core/Src/main.c` | 调用 `ov2640_init()` + 临时显示循环 |
| `Core/Src/freertos.c` | 阶段5：新增 camera_task |

---

## 阶段 0：写好寄存器初始化代码（纯软件，无需硬件）

### 任务

在 `ov2640.c` 中按顺序实现三张寄存器表：

```
表 A  传感器初始化（Sensor Bank，0xFF=0x01）
      把图像传感器内核配置为 SVGA（800×600）工作模式
      约 90 条寄存器

表 B  DSP + QVGA 输出（DSP Bank，0xFF=0x00）
      开启 ISP 缩放，将输出降至 320×240
      约 40 条寄存器

表 C  RGB565 格式
      切换像素格式为 RGB565（大端，高字节先出）
      约 10 条寄存器
```

表格条目类型定义：

```c
/* ov2640.h */
typedef struct
{
    uint8_t reg;
    uint8_t val;
} ov2640_reg_t;

void ov2640_init(void);
```

`ov2640_init()` 执行顺序：

```
1. 切换到 Sensor Bank（写 0xFF=0x01）
2. 软件复位（写 0x12=0x80）
3. HAL_Delay(10)
4. 逐条写入表 A
5. 切换到 DSP Bank（写 0xFF=0x00）
6. 逐条写入表 B
7. 逐条写入表 C
```

### 验收标准

- [ ] 代码编译通过，零警告

---

## 阶段 1：验证 SCCB 写入与摄像头初始化

### 任务

在 `main.c` 的 `MX_I2C1_Init()` 之后、FreeRTOS 启动之前调用：

```c
/* 确保 PWDN 拉低（设备上电） */
HAL_GPIO_WritePin(DCMI_PWDN_GPIO_Port, DCMI_PWDN_Pin, GPIO_PIN_RESET);
HAL_Delay(10U);

ov2640_init();

/* 读回 PID 验证初始化后 Bank 状态 */
uint8_t pid_h = 0U;
OV2640_WriteReg(0xFFU, 0x01U);   /* 切回 Sensor Bank */
HAL_Delay(1U);
OV2640_ReadReg(0x0AU, &pid_h);   /* 期望值：0x26 */
```

### 注意点

- 每次切换 Bank 后至少等 1ms 再操作寄存器
- 软件复位后至少等 10ms
- `PWDN` 在整个工作期间保持 LOW

### 验收标准

- [ ] `ov2640_init()` 内所有 `OV2640_WriteReg` 返回 `HAL_OK`
- [ ] 读回 `pid_h == 0x26`

---

## 阶段 2：DCMI + DMA 抓取首帧（QVGA 320×240）

### 2.1 帧缓冲定义（camera.h / camera.c）

```c
/* camera.h */
#define CAM_W          (320U)
#define CAM_H          (240U)
#define CAM_BUF_BYTES  (CAM_W * CAM_H * 2U)        /* 153600 字节 */
#define CAM_BUF_WORDS  (CAM_BUF_BYTES / 4U)         /* 38400  个32bit字 */

extern volatile uint8_t g_frame_ready;
extern uint8_t          g_frame_buf[CAM_BUF_BYTES];

void camera_start(void);
```

```c
/* camera.c */
/* 强制放入 AXI SRAM（链接脚本已有 .ram_dma_buffers → RAM 0x24000000） */
/* DMA2 可访问；若放 DTCM 则 DMA 无法工作 */
uint8_t          g_frame_buf[CAM_BUF_BYTES] __attribute__((section(".ram_dma_buffers")));
volatile uint8_t g_frame_ready = 0U;
```

### 2.2 启动 DCMI

```c
/* camera.c */
void camera_start(void)
{
    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS,
                       (uint32_t)g_frame_buf, CAM_BUF_WORDS);
}
```

DMA 已配置为循环模式（CIRCULAR），DCMI 持续推帧到缓冲区，
每完成一帧硬件产生 FRAME 中断，HAL 调用下方回调。

### 2.3 帧完成回调

```c
/* camera.c — 重写 HAL 弱函数 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    /* D-Cache 已开启：DMA 写完后 CPU 读之前必须 Invalidate */
    /* 否则 CPU 读到的是 Cache 里的旧数据（图像静止或花屏） */
    SCB_InvalidateDCache_by_Addr((uint32_t *)g_frame_buf, (int32_t)CAM_BUF_BYTES);
    g_frame_ready = 1U;
}
```

### 2.4 在 main.c 中调用

```c
/* ov2640_init() 之后 */
camera_start();
```

### 验收标准

- [ ] `HAL_DCMI_FrameEventCallback` 断点能被命中（证明 DCMI 在工作）
- [ ] 调试器查看 `g_frame_buf` 内容不全为 `0x00` 或 `0xFF`

> **如果回调从未触发**，先用示波器量 PA6（PIXCLK），
> 无信号说明 OV2640 没有正常工作，回阶段 1 排查。

---

## 阶段 3：ST7789 临时显示 QVGA 图像

此阶段的目的是**验证图像内容**，不要求显示完整。
ST7789 是 240×240，QVGA 宽 320，水平方向会超出边界——属正常现象。

### 临时显示循环（main.c while 循环内）

```c
while (1)
{
    if (g_frame_ready)
    {
        g_frame_ready = 0U;
        /* 只取每行前 240 像素（左侧部分），临时验证用 */
        p_lcd->pf_draw_image(p_lcd, 0, 0, 240U, 240U, g_frame_buf);
    }
}
```

### 验收标准

- [ ] 屏幕有图像，能分辨画面内容（人、物体等）
- [ ] 颜色基本正确

> **常见问题：红蓝互换（图像偏蓝或偏红）**
> OV2640 输出的 RGB565 可能字节序与 ST7789 不匹配。
> 解决：在表 C 格式寄存器中设置字节序翻转位，或在 `pf_draw_image` 显示时逐像素交换。
> 此阶段记录现象即可，后续统一处理。

---

## 阶段 4：DCMI 硬件裁剪到 240×240

### 原理

STM32H7 DCMI 内置裁剪窗口，可在 DMA 传输前丢弃不需要的像素，
无需 CPU 参与，帧缓冲直接收到裁剪后的数据。

从 QVGA（320×240）中取水平居中的 240 列：

```
左侧跳过：(320 - 240) / 2 = 40 像素 = 80 字节 = 20 个 32bit 字
裁剪宽度：240 像素       = 480 字节 = 120 个 32bit 字
垂直方向：全部 240 行（不裁剪）
```

> DCMI 裁剪寄存器的水平参数单位是 **32bit 字**（4字节），
> 因为 DCMI 内部以 32bit 为单位打包 8bit 像素数据。

### 4.1 修改 camera.h 尺寸宏

```c
#define CAM_W          (240U)          /* 裁剪后宽度 */
#define CAM_H          (240U)
#define CAM_BUF_BYTES  (CAM_W * CAM_H * 2U)   /* 115200 字节 */
#define CAM_BUF_WORDS  (CAM_BUF_BYTES / 4U)   /* 28800  个32bit字 */
```

### 4.2 在 camera_start() 中添加裁剪配置

```c
void camera_start(void)
{
    HAL_DCMI_ConfigCrop(&hdcmi,
        20U,    /* X0    : 跳过 20 字 = 40 像素（左侧跳过） */
        0U,     /* Y0    : 从第 0 行开始 */
        120U,   /* XSize : 120 字 = 240 像素 */
        240U);  /* YSize : 240 行 */
    HAL_DCMI_EnableCrop(&hdcmi);

    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS,
                       (uint32_t)g_frame_buf, CAM_BUF_WORDS);
}
```

### 4.3 更新显示调用

```c
p_lcd->pf_draw_image(p_lcd, 0, 0, 240U, 240U, g_frame_buf);
```

### 验收标准

- [ ] 屏幕图像铺满 240×240，无黑边
- [ ] 画面居中（左右各被裁掉约 40 像素）

> **如果图像偏移**：X0 每加减 1 对应平移 2 像素，微调直到居中。

---

## 阶段 5：搬入 FreeRTOS 任务

### camera_task（freertos.c 或单独 camera_task.c）

```c
void camera_task(void *argument)
{
    camera_start();

    for (;;)
    {
        if (1U == g_frame_ready)
        {
            g_frame_ready = 0U;
            p_lcd->pf_draw_image(p_lcd, 0, 0, 240U, 240U, g_frame_buf);
        }
        osDelay(1U);
    }
}
```

任务参数：

| 参数 | 值 | 说明 |
|------|----|------|
| 栈大小 | 2048 字（8KB） | 图像处理比普通任务耗栈 |
| 优先级 | osPriorityNormal | 与其他任务平级 |

### 验收标准

- [ ] 预览连续，画面流畅更新
- [ ] 其他 FreeRTOS 任务（LetterShell 等）不受影响

---

## 快速排查指南

| 现象 | 最可能原因 | 检查 / 解决 |
|------|-----------|------------|
| FrameEventCallback 从不触发 | PIXCLK 没有信号 | 示波器量 PA6 |
| 回调触发但 buf 全 0x00 | 帧缓冲不在 AXI SRAM | 确认 section `.ram_dma_buffers` |
| 图像花屏 / 静止 | D-Cache 未 Invalidate | 检查 `SCB_InvalidateDCache_by_Addr` |
| 图像上下滚动 | VSYNC 极性不匹配 | 尝试改为 `DCMI_VSPOLARITY_HIGH` |
| 颜色红蓝互换 | RGB/BGR 字节序 | 修改 OV2640 表 C 字节序寄存器 |
| 图像裁剪偏移 | X0 参数偏差 | X0 ±1 步调（对应 ±2 像素）微调 |

---

## 后续：迁移四层架构

硬件链路全部跑通后，按以下对应关系拆分：

| 现有平坦代码 | 迁移目标 |
|-------------|---------|
| `ov2640.h/c`（寄存器表） | `Bsp_drivers/bsp_drv_ov2640.h/c` |
| `ov2640.c`（I2C 写寄存器） | `ST_platform/st_ov2640.c` |
| `camera.h/c`（DCMI 接口） | `Bsp_drivers/bsp_drv_dcmi.h/c` |
| `camera.c`（HAL 实现） | `ST_platform/st_dcmi.c` |
| 类型适配 | `Adapter/camera_adapter.c` |
| 任务层控制 | `Handle/camera_handle.h/c` |
