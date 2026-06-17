---
name: st7789-driver-status
description: ST7789 LCD 驱动开发状态、架构决策和命名规范
metadata:
  type: project
---

## ST7789 驱动当前状态

**BSP 驱动已完成**，编译通过，烧录测试 R/G/B 循环刷新正常。

### 文件清单

| 文件 | 说明 |
|------|------|
| `Bsp_drivers/Inc/bsp_drv_st7789.h` | 驱动头文件：st7789_spi_ops_t、st7789_driver_t、st7789_driver_instruct() |
| `Bsp_drivers/Src/bsp_drv_st7789.c` | 驱动实现：init/set_window/fill_screen/clear_screen |
| `ST_platform/Inc/st_lcd_spi.h` | 平台层头文件：extern g_st7789_spi_ops |
| `ST_platform/Src/st_lcd_spi.c` | 平台层实现：spi_transmit/backlight_pin/dataorcmd_pin/delay_ms |
| `st7789.md` | 用户提供的规格文件 |

### 架构决策

- **两层架构**：Bsp_drivers（逻辑）+ ST_platform（硬件原语）
- **st7789_spi_ops_t**：4 个纯硬件原语（pf_spi_transmit / pf_backlight_pin / pf_dataorcmd_pin / pf_delay_ms），无 self 指针，直接引用硬件
- **st7789_driver_t**：函数指针挂在结构体里（pf_init / pf_set_window / pf_fill_screen / pf_clear_screen），通过 st7789_driver_instruct() 绑定
- **write_cmd/write_data/write_buf**：static 函数在驱动层组合 DC+SPI，不对外暴露

### 命名规范

- `pf_` 前缀：仅用于结构体的**函数指针字段**（如 `pf_fill_screen`）
- `st7789_` 前缀：用于**实体实现函数**（如 `st7789_fill_screen`）
- `g_` 前缀：全局变量（如 `g_st7789_spi_ops`、`g_st7789_drv`）

### 调用方式

```c
st7789_driver_instruct(&g_st7789_drv, &g_st7789_spi_ops);
g_st7789_drv.pf_fill_screen(&g_st7789_drv, 0xF800U);  // 红
g_st7789_drv.pf_clear_screen(&g_st7789_drv);           // 清屏
```

### 待做

- 后续如果需要，按 LED 模式加 Handle + Adapter 层

**Why:** 总结本次会话所有架构决策，确保下次会话能无缝继续。
**How to apply:** 新会话中说"继续 ST7789 驱动开发"，或参考 st7789.md 规格文件和本 memory 了解当前状态。
