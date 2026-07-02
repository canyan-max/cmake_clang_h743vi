# OV2640 + ST7789 联动适配计划

> 状态：**已完成（稳定运行 29fps）**
> 目标：将 `freertos.c` 中的 OV2640 临时代码重构为项目四层架构，并修复已发现的 Bug。

---

## 1. 当前状态（最终）

### 1.1 已实现功能

| 功能 | 状态 |
|------|------|
| 四层架构重构（ST_platform / Bsp_drivers / Adapter / Handle） | ✅ 已完成 |
| OV2640 SCCB（I2C）寄存器读写 | ✅ 已完成 |
| SVGA 模式初始化（800×600 传感器，400×300 DSP 输出） | ✅ 已完成 |
| DCMI 裁剪：400×300 → 中心 240×240 | ✅ 已完成 |
| DCMI + DMA 连续采集 | ✅ 已完成 |
| `HAL_DCMI_FrameEventCallback` → TaskNotify → `draw_image` | ✅ 已完成 |
| DWT 帧率 / 绘制耗时测量（logInfo 每秒打印） | ✅ 已完成 |

### 1.2 数据流

```
OV2640 传感器（SVGA 800×600，CLKRC=0x80 → 24MHz）
    ↓ DSP 缩放
  400×300 RGB565 输出
    ↓ DCMI 硬件裁剪（中心区域）
  240×240 RGB565（= 115200 字节）
    ↓ DMA → g_camera_data_buffer (.ram_dma_buffers, 32-byte aligned)
  HAL_DCMI_FrameEventCallback → vTaskNotifyGiveFromISR
    ↓
  defaultTask → draw_handle_draw_image()
    ↓
  ST7789 LCD（240×240）
```

### 1.3 关键寄存器配置（当前稳定值）

| 寄存器 | 地址 | 值 | 说明 |
|--------|------|----|------|
| CLKRC  | 0x11 | 0x80 | bit7=1 旁路分频器，传感器时钟 = XVCLK = 24MHz |
| FLL    | 0x46 | 0x00 | 无额外 dummy 行，帧率最大化 |
| R_DVP_SP | 0x2A | 0x80 | bit7=1 自动 PCLK |
| COM7   | 0x12 | 0x40 | bits[6:4]=100 → SVGA 模式 |

**实测帧率：~29fps，LCD 绘制耗时：~17.5ms/帧**

---

## 2. 已修复的 Bug

### 2.1 ✅ DMA 缓冲区大小溢出（已修复）

`HAL_DCMI_Start_DMA` 第四个参数单位是 **32-bit word**，不是字节。

```c
// 修复后（freertos.c）
__attribute__((section(".ram_dma_buffers"), aligned(32)))
uint8_t g_camera_data_buffer[115200];   // 240×240×2 字节

// 驱动调用（OV2640_DMA_LEN_WORDS = 240×240×2/4 = 28800）
camera_handle_start(&g_cam_handle, (uint32_t *)g_camera_data_buffer, OV2640_DMA_LEN_WORDS);
```

### 2.2 ✅ DCache 无效化范围错误（已修复）

```c
// 修复后（freertos.c HAL_DCMI_FrameEventCallback）
SCB_InvalidateDCache_by_Addr((uint32_t *)g_camera_data_buffer, sizeof(g_camera_data_buffer));
```

### 2.3 verify_list 不一致（已随重构删除）

重构时已删除原 verify_list，不再保留。

---

## 3. 架构（已落地）

### 3.1 文件清单

```
ST_platform/
  Inc/st_ov2640.h        ← g_ov2640_hw_ops 声明
  Src/st_ov2640.c        ← HAL_I2C_* / HAL_DCMI_* 实现

Bsp_drivers/
  Inc/bsp_drv_ov2640.h   ← ov2640_driver_t、ov2640_hw_ops_t、模式宏
  Src/bsp_drv_ov2640.c   ← ov2640_svga_cfg 配置表、ov2640_driver_instruct

Adapter/
  Inc/ov2640_adapter.h
  Src/ov2640_adapter.c   ← drv_cam_init / drv_cam_start / drv_cam_stop

Handle/
  Inc/camera_handle.h
  Src/camera_handle.c    ← camera_handle_instruct / camera_handle_start / stop
```

### 3.2 调用链

```
camera_handle_start(&g_cam_handle, buf, len)
  → p_handle->p_ops->pf_start(p_handle->p_drv, buf, len)   [Handle]
    → drv_cam_start(p_drv, buf, len)                        [Adapter]
      → p_drv->pf_start(p_drv, buf, len)                   [Bsp_drivers]
        → p_hw_ops->pf_dcmi_start_dma(buf, len, mode)      [ST_platform]
          → HAL_DCMI_Start_DMA()
```

---

## 4. 实施步骤

| 步骤 | 内容 | 状态 |
|------|------|------|
| S1 | 修复 Bug 2.1：缓冲区声明为 `uint8_t[115200]` | ✅ 已完成 |
| S2 | 修复 Bug 2.2：DCache invalidate 范围 115200 字节 | ✅ 已完成 |
| S3 | 清理 verify_list | ✅ 已完成（重构时删除） |
| S4 | 提取 `st_ov2640.c/h` | ✅ 已完成 |
| S5 | 实现 `bsp_drv_ov2640.c/h`（配置表 + instruct） | ✅ 已完成 |
| S6 | 实现 `ov2640_adapter.c/h` | ✅ 已完成 |
| S7 | 实现 `camera_handle.c/h` | ✅ 已完成 |
| S8 | `freertos.c` 改为通过 camera_handle 调用 | ✅ 已完成 |

---

## 5. 帧率优化过程记录

### 5.1 诊断过程

初始现象：14fps。怀疑 LCD SPI 是瓶颈。

用 DWT 周期计数器测量绘制耗时：

```c
uint32_t t0 = DWT->CYCCNT;
draw_handle_draw_image(...);
uint32_t t1 = DWT->CYCCNT;
draw_us = (t1 - t0) / (SystemCoreClock / 1000000U);
```

实测 draw=17555us（约17.5ms），理论上限 1000/17.5 ≈ 57fps，LCD 不是瓶颈。

**根因：CLKRC=0x00 → 传感器 PCLK 分频，实际只有 12MHz，帧率减半。**

### 5.2 关键寄存器修正

| 修改 | 之前 | 之后 | 效果 |
|------|------|------|------|
| CLKRC (0x11) | 0x00（分频模式，12MHz） | 0x80（旁路分频，24MHz） | 14fps → 29fps ✅ |
| FLL (0x46) | 实验 0x3F（增加 dummy 行） | 0x00（无额外行） | 27fps → 29fps（0x3F 是反向操作，FLL 值越大帧率越低）|

### 5.3 CIF 模式探索（已放弃）

**尝试目标：** COM7 bits[6:4]=001（CIF 模式）→ 期望提升到 ~60fps。

**尝试过程：**

1. 完整 CIF 配置表（含 COM7=0x10）→ DCMI 中断不触发（VEND/REG32 等参数错误导致传感器时序异常）
2. 修正为与 SVGA 相同的 VEND/HREFEND/REG32 → 中断触发，但图像下半屏黑、帧率反而降至 17fps
3. 根因分析：SVGA 和 CIF 的传感器扫描窗口寄存器（HREFEND/VEND/REG32）值相同，说明两模式感光阵列扫描范围一致，帧率提升依赖 DSP 内部子采样而非窗口缩小。DSP 尺寸寄存器（0xC0/0xC1/0x51/0x52）组合未能正确收敛。

**结论：放弃 CIF 模式，维持 SVGA 29fps。**

---

## 6. 待讨论事项

| # | 问题 | 状态 | 决策 |
|---|------|------|------|
| Q1 | 缓冲区类型 | ✅ 已定 | `uint8_t[115200]`，section `.ram_dma_buffers` |
| Q2 | verify_list 去留 | ✅ 已定 | 重构时已删除 |
| Q3 | 是否支持多分辨率 | ✅ 已定 | 固定 240×240，宏 `OV2640_OUT_W/H` 统一管理 |
| Q4 | 是否双缓冲（ping-pong） | ✅ 已定 | 单缓冲 |
| Q5 | camera_handle 与 draw_handle 关系 | ✅ 已定 | 分离，task 层自己调 draw_handle |
| Q6 | 帧通知机制 | ✅ 已定 | TaskNotify（vTaskNotifyGiveFromISR） |

---

## 7. 内存占用

| 区域 | 变量 | 大小 |
|------|------|------|
| RAM_D2（DMA 缓冲，`.ram_d2_dma_buffers`） | `g_camera_data_buffer` | **112.5 KB** |
| Flash | `ov2640_svga_cfg` 配置表 | ~200 字节 |
| RAM（驱动实例） | `g_ov2640_drv` + handle | < 200 字节 |

112.5 KB 在 RAM_D2  288K  **39%**。链接脚本 `.ram_d2_dma_buffers` 映射到 0x30000000（D2 域）。

---

## 8. 寄存器说明（摘自 OV2640 手册）

### COM7 (0x12) — Common Control 7

| Bit | 说明 |
|-----|------|
| [7] | SRST：1 = 系统复位，复位后恢复正常运行 |
| [6:4] | 分辨率选择：000=UXGA，**001=CIF**，**100=SVGA** |
| [2] | Zoom 模式 |
| [1] | Color bar 测试图案（0=关，1=开） |

### 窗口寄存器

| 地址 | 名称 | 默认值 | 说明 |
|------|------|--------|------|
| 0x1A | VEND | 0x97 | 垂直窗口结束 MSB 8 位（SVGA 和 CIF 相同） |
| 0x17 | HREFST | 0x11 | 水平窗口起始 MSB（3 LSB 在 REG32[2:0]） |
| 0x18 | HREFEND | 0x75(UXGA) / **0x43(SVGA,CIF)** | 水平窗口结束 MSB（SVGA 和 CIF 相同） |
| 0x32 | REG32 | 0x36(UXGA) / **0x09(SVGA,CIF)** | PCLK 分频 + 窗口位置低位（SVGA 和 CIF 相同） |

### DSP 尺寸寄存器

| 地址 | 名称 | 说明 |
|------|------|------|
| 0xC0 | HSIZE8 | Image H Size [10:3]（实际尺寸 / 8） |
| 0xC1 | VSIZE8 | Image V Size [10:3]（实际尺寸 / 8） |
| 0x51 | HSIZE | H_SIZE[7:0]（实际尺寸 / 4） |
| 0x52 | VSIZE | V_SIZE[7:0]（实际尺寸 / 4） |

SVGA DSP 输出 400×300 对应：0xC0=0x32, 0xC1=0x25, 0x51=0x64, 0x52=0x4B

---

*最后更新：2026-07-02*
