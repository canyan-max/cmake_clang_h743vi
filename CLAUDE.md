# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

> 本文件融合了通用 LLM 编码行为准则（第 0 章）与嵌入式项目特定规范（第 1-5 章）。

## 0. 核心行为准则（针对 LLM 的通用要求）

以下准则适用于所有代码生成与修改任务，优先级高于具体编码规范。

**Tradeoff:** 这些准则偏向谨慎而非速度。对于琐碎任务，可自行判断。

### 0.1 先思后写（Think Before Coding）

**不假设、不隐藏困惑、暴露权衡。**

实现之前：
- 明确列出你的假设。如果不确定，**必须问**，不要猜。
- 如果存在多种合理解释（如轮询 vs 中断、阻塞 vs 非阻塞），全部列出并说明选择理由。
- 如果有更简单的方案，直接提出。合理时推翻原要求。
- 如果某处不清楚，停下来。指出哪里混乱，然后提问。

### 0.2 简洁优先（Simplicity First）

**最小代码解决问题，不写投机性代码。**

- 不实现超出要求的功能。
- 不为单次使用的代码创建抽象层。
- 不添加未要求的"灵活性"或"可配置性"。
- **嵌入式例外**：数组越界检查、超时机制、硬件防御性检查虽然"看似多余"，但必须保留（参见第 3.3 节防御性编程）。

自问："资深工程师会觉得这个过度复杂吗？" 如果是，简化。

### 0.3 手术式修改（Surgical Changes）

**只改必须改的，只清理自己制造的垃圾。**

修改已有代码时：
- 不"顺便改进"相邻代码、注释或格式。
- 不重构没坏的东西。
- 匹配现有风格（即使你不喜欢）。
- 如果发现无关的死代码，**仅提及**，不要删除。

当你的改动产生孤儿代码时：
- 删除**因你改动而变得未使用**的导入/变量/函数。
- 不要删除之前就存在的死代码，除非特别要求。

检验标准：每一行改动都应直接追溯到用户的需求。

### 0.4 目标驱动执行（Goal-Driven Execution）

**定义成功标准，循环验证直到通过。**

将任务转化为可验证的目标：
- "添加校验" → "编写针对无效输入的测试，然后让测试通过"
- "修复 bug" → "编写复现 bug 的测试，然后让测试通过"
- "重构 X" → "确保重构前后测试全部通过"

对于多步骤任务，先给出简要计划（在实现前）。

强成功标准让你可以独立迭代。弱标准（"让它工作"）需要不断澄清。

---

## 1. 项目环境

| 项目     | 值                                                    |
| -------- | ----------------------------------------------------- |
| MCU      | STM32H743VIT6 (Cortex-M7, 400MHz, FPU, 双 Bank Flash) |
| 工具链   | starm-clang (基于 LLVM/clang) + CMake + Ninja         |
| RTOS     | FreeRTOS Kernel V10.6.2 (CMSIS-OS2 接口)              |
| 调试器   | J-Link (JLinkGDBServer / JLinkExe)                    |
| 构建系统 | CMake Presets (Debug / Release)                       |

### 构建与烧录

```bash
./runbuid_and_flash.sh -b        # 仅编译（Debug preset）
./runbuid_and_flash.sh           # 编译 + 烧录
./runbuid_and_flash.sh -c        # 清理后重新编译（不烧录）
./runbuid_and_flash.sh -f        # 仅烧录（使用已有 hex）
./runbuid_and_flash.sh -p Release -b  # Release 模式编译
./runbuid_and_flash.sh -h        # 查看所有参数
```

**每次改动完成必须编译并确保无警告和错误。**

输出产物：`build/Debug/cmake-clang-h743vi.hex` / `.bin` / `.elf`

---

## 2. 关键约束（内存/存储）

以链接脚本 `STM32H743XX_FLASH.ld` 实际值为准：

| 区域    | 起始地址   | 大小   | 说明                              |
| ------- | ---------- | ------ | --------------------------------- |
| Flash   | 0x08000000 | 2048K  | 代码 + 只读数据                   |
| DTCM    | 0x20000000 | 128K   | 紧耦合数据内存；栈顶 `_estack` 在此 |
| RAM     | 0x24000000 | 512K   | 通用 AXI SRAM（主堆/数据）        |
| RAM_D2  | 0x30000000 | 288K   | DMA 外设缓冲区常用区域            |
| RAM_D3  | 0x38000000 | 64K    | 低功耗域 SRAM                     |
| ITCM    | 0x00000000 | 64K    | 紧耦合指令内存（可选）            |

链接脚本定义：`_Min_Stack_Size = 0x800`，`_Min_Heap_Size = 0x200`。  
FreeRTOS 堆由 `heap_4.c` + `configTOTAL_HEAP_SIZE` 控制（放在 AXI SRAM）。

### 运行时约束

- 栈大小：每个 FreeRTOS 任务建议 1024 字 (4KB)
- 禁止 `malloc`/`free`；动态分配只能用 `pvPortMalloc`，且须注释说明
- 关键时序：UART 中断响应 < 10us；控制环路周期 1kHz
- I-Cache / D-Cache 已在 `main()` 启动时开启；DMA 缓冲区须放在非缓存区域或手动维护一致性

---

## 3. 驱动架构（四层模型）

项目采用四层解耦架构，**新增外设驱动必须严格遵循此分层**：

```
Handle        ← 应用逻辑，void* 接口，平台无关
   ↕ (Adapter 桥接)
Adapter       ← 类型转换层，将 Bsp 类型 API 适配为 Handle 期望的 void* 签名
   ↕
Bsp_drivers   ← 设备无关驱动接口（纯函数指针结构体 + 状态枚举）
   ↕
ST_platform   ← STM32 具体硬件实现，填充 Bsp_drivers 的 ops 结构体
```

### 各层职责说明

| 层          | 目录           | 职责                                                      |
| ----------- | -------------- | --------------------------------------------------------- |
| Handle      | `Handle/`      | 调用 Adapter 完成业务逻辑，不感知底层硬件类型             |
| Adapter     | `Adapter/`     | 将 `led_driver_t*` 等强类型转换为 `void*`，供 Handle 使用 |
| Bsp_drivers | `Bsp_drivers/` | 定义驱动接口结构体（ops）和状态枚举，执行驱动初始化逻辑   |
| ST_platform | `ST_platform/` | 实现 ops 中的每个函数指针，直接调用 HAL / LL 库           |

### 数据流示例（LED）

```
led_handle_on(p_handle)
  → p_handle->p_ops->pf_led_on(p_handle->p_drv)      [Handle]
    → drv_led_on(p_drv)                               [Adapter]
      → p_drv->p_led_ops->pf_led_on(p_drv)           [Bsp_drivers]
        → st_led_on(self)  →  HAL_GPIO_WritePin()     [ST_platform]
```

### 现有驱动一览

| 外设       | Bsp_drivers         | ST_platform     |
| ---------- | ------------------- | --------------- |
| LED        | `bsp_drv_led`       | `st_led`        |
| AT24 EEPROM| `bsp_drv_at24`      | `st_iic`        |
| ST7789 LCD | `bsp_drv_st7789`    | `st_lcd_spi`    |

---

## 4. 中间件

| 中间件        | 目录                        | 说明                                          |
| ------------- | --------------------------- | --------------------------------------------- |
| FreeRTOS      | `Middlewares/Third_Party/FreeRTOS/` | RTOS 内核，通过 CMSIS-OS2 接口使用     |
| LetterShell   | `Middlewares/LetterShell/`  | UART 调试 Shell，命令注册在 `Core/Src/cmds.c` |
| Kfifo         | `Middlewares/Kfifo/`        | 通用环形缓冲区                                |
| Soc           | `Middlewares/Soc/`          | 电池 SOC 算法（OCV + 安时积分 + 内阻学习）    |
| Dwt           | `Middlewares/Dwt/`          | DWT 周期计数器（精确微秒延时/计时）           |
| Front         | `Middlewares/Front/`        | LCD 字体渲染（与 ST7789 驱动配合）            |

---

## 5. 编码规范

### 5.1 类型与宽度

- 必须包含 `<stdint.h>`，使用 `uint8_t`、`uint32_t` 等。
- 常量后缀 `U`（如 `0U`），禁止魔术数字。
- 禁止使用 `int`、`long`（除非与 HAL 库交互时显式转换）。

### 5.2 命名规则

- 全局变量：`g_` 前缀（如 `g_led_handle`）
- 静态全局变量：无前缀，文件作用域
- 类型名：`_t` 后缀，结构体/枚举名全大写（如 `LED_DRIVER_T`）
- 宏定义：全大写 + 下划线
- 函数名：小写 + 下划线（如 `led_handle_on`）

### 5.3 防御性编程

- 函数入口检查参数：`NULL`、范围、初始化标志（`is_init`）。
- 数组/缓冲区访问必须带上界检查。
- 循环等待硬件标志必须有超时退出机制。
- 不使用 `malloc`/`free`。
- 所有 `switch` 必须有 `default`。

### 5.4 注释与文档

函数注释风格如下：

```c
/**
  * @brief            : [函数名称] 简单描述一下这个函数的功能（必须添加）
  * @retval           : [返回值说明] 如果是枚举类型需要列出内部能返回的值，无需全部列出。没有此项可以不添加
  * @param[in]        : [参数名 参数说明] 没有此项可以不添加
  * @param[out]       : [参数名 参数说明] 没有此项可以不添加
  */
```
### 5.5 函数测量时间

```c
可以使用 Middlewares/Dwt/下的函数
需要使用的时候必须在系统初始化之前调用    dwt_init(); 
目前已经在MX_FREERTOS_Init 内调用了
void test ()
{
    uint32_t time = get_dwt_us();
    function();
    time =  get_dwt_us() - time;
}
```
