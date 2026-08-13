# `plat_mcu` 设计规范

> MCU 外挂芯片的协议、传输适配和板级实例设计见 [`MCU 外挂芯片驱动设计规范`](./external_chip_driver_design.md)。

## 1. 目的

`plat_mcu` 用于隔离上层软件与具体 MCU、厂商 SDK 和 HAL，使 BSP 及其上层代码不因 STM32、NXP 等 MCU 平台变化而修改。

本项目采用以下静态适配方式：

```text
上层/BSP
    |
    | plat_*() + 逻辑资源 ID
    v
04_Platform/03_mcu_interface   公共能力契约
    |
    | 由构建系统选择唯一实现
    v
05_Impl/01_mcu                MCU 适配实现和资源表
    |
    | 逻辑资源 ID -> SDK 资源
    v
STM32 HAL / CubeMX
```

该设计是编译期选择 MCU 后端、运行时按逻辑 ID 查找固定资源，不是动态驱动注册框架。

## 2. 设计结论

当前采用的“Platform 声明接口、MCU 层实现接口、通过静态表索引硬件资源”的总体设计合理，应继续沿用，不需要重构为全局函数指针表或动态注册框架。

这种方式具有以下特点：

- 公共接口稳定，不暴露厂商类型；
- MCU 和板级资源关系集中、可审查；
- 不使用动态内存，存储和时间开销可预测；
- 更换 MCU 时主要替换 `05_Impl/<target>`；
- 逻辑 ID 的范围检查可以在统一边界完成；
- 适合硬件拓扑在编译期确定的单片机产品。

## 3. `plat_mcu` 的职责

`plat_mcu` 回答两个问题：

1. MCU 能提供什么基础能力；
2. 当前 MCU 如何实现这些能力。

适合由 `plat_mcu` 提供的能力包括：

- GPIO 读、写、翻转；
- UART 发送、接收启动和硬件事件上报；
- SPI/I2C 基础传输；
- 定时、延时和时间戳；
- 中断、DMA 和 Cache 维护；
- MCU 复位、临界区等通用底层能力。

`plat_mcu` 可以接触：

- HAL/LL 和 MCU 寄存器；
- CubeMX 生成的外设句柄；
- DMA 通道和中断入口；
- MCU 特有的内存与 Cache 约束；
- `board_config.h` 提供的资源配置。

`plat_mcu` 不应包含：

- LED、按键、摄像头等板级器件语义；
- LED 有效电平、器件 I2C 地址等板级属性；
- 产品协议、业务状态机和任务流程；
- 对上层可见的 HAL/LL 类型或 MCU 寄存器定义。

## 4. 公共接口规则

公共契约位于 `04_Platform/03_mcu_interface/Inc`，并遵守以下规则：

1. API 使用 `plat_*` 前缀。
2. 跨模块数据使用 `<stdint.h>` 定宽类型。
3. 接口返回统一的 `platform_err_t`；查询类接口也必须有明确的错误表达方式。
4. 头文件不得包含或暴露 `UART_HandleTypeDef`、`GPIO_TypeDef`、`HAL_StatusTypeDef` 等厂商类型。
5. 固定硬件资源优先使用逻辑 ID，不使用 `void *` 传递厂商句柄。
6. 公共函数检查空指针、ID 范围、长度和初始化状态。
7. 硬件等待操作必须具有超时，不允许无限轮询。
8. DMA 接口必须说明缓冲区所有权、长度单位、对齐、可访问内存域和 Cache 维护责任。
9. 回调接口必须说明执行上下文是 ISR 还是任务，以及回调中允许执行的操作。

公共接口示例：

```c
typedef enum PLAT_UART_ID_T
{
    PLAT_UART_ID_0 = 0U,
    PLAT_UART_ID_NUM,
} plat_uart_id_t;

platform_err_t plat_uart_send(plat_uart_id_t id,
                              const uint8_t *p_data,
                              uint16_t size,
                              uint32_t timeout_ms);
```

接口中的逻辑 ID 表示 Platform 资源实例，不表示该资源在产品中的用途。

## 5. MCU 实现和资源表规则

具体实现位于 `05_Impl/<target>`，每个 MCU 后端实现相同的 `plat_*` 契约。STM32 实现可以在 `.c` 文件内部使用 HAL 类型：

```c
static UART_HandleTypeDef *const s_uart_handle_table[PLAT_UART_ID_NUM] = {
    [PLAT_UART_ID_0] = &BOARD_UART_0_HANDLE,
};
```

资源表遵守以下规则：

1. 表定义为文件私有的 `static const`；运行期状态使用单独的状态表。
2. 优先使用枚举指定下标初始化，避免依赖枚举声明顺序。
3. 表长度由对应的 `*_ID_NUM` 决定。
4. 访问表前必须验证 ID，不允许未检查的数组索引。
5. 如果允许资源缺省，访问时还必须检查表项是否有效。
6. 表中可以保存 HAL 句柄、端口、引脚和 DMA 信息，但这些类型不得出现在公共头文件。
7. ISR/HAL 回调先将厂商句柄反查为逻辑 ID，再向公共回调上报。
8. 一个固件默认只链接一个 MCU 后端，避免相同 `plat_*` 符号存在多个实现。

建议统一使用不会遗漏负值的 ID 检查方式：

```c
static bool plat_uart_id_is_valid(plat_uart_id_t id)
{
    return ((uint32_t)id < (uint32_t)PLAT_UART_ID_NUM);
}
```

## 6. 与 BSP 的边界

两层的边界定义为：

```text
plat_mcu：MCU 能做什么，以及具体怎么做。
BSP：这块板上有什么，以及它如何连接、怎样才算有效。
```

典型调用关系：

```text
bsp_led_on(BSP_LED_1)
    |
    | LED1 对应哪个 Platform GPIO、有效电平是多少
    v
plat_gpio_write(PLAT_GPIO_ID_0, active_level)
    |
    | Platform GPIO 对应哪个 STM32 端口和引脚
    v
HAL_GPIO_WritePin(...)
```

BSP 负责 LED/按键等板载器件、有效电平、片选/复位/电源控制、器件固定地址及板级连接关系。BSP 可以调用 `plat_*`，但不得直接调用 HAL。

## 7. 资源表与操作表的选择

固定 MCU 资源使用逻辑 ID 和资源表：

```text
plat_uart_id_t -> UART_HandleTypeDef *
plat_gpio_id_t -> GPIO_TypeDef * + pin
```

不要为了形式统一而给每个 `plat_*` 接口增加函数指针。MCU 后端已经由 CMake 和链接器静态选择，直接函数调用更简单。

只有满足下列情况之一时才使用 `ops` 操作表：

- 同一固件需要运行时选择不同实现；
- 器件驱动需要注入 SPI、I2C、延时等传输能力；
- 单元测试需要替换为 mock 实现；
- 同一协议驱动确实存在多个可互换后端。

## 8. 对当前实现的审查结论

### 8.1 可以保留

- `plat_gpio.h`、`plat_uart.h` 声明公共接口，由 `mcu_gpio.c`、`mcu_uart.c` 实现；
- HAL 句柄仅保存在 MCU 实现文件；
- 使用逻辑 ID 查找固定资源；
- `impl_mcu` 私有依赖 `mcu_interface`、`board_config` 和 `stm32cubemx`；
- 上层通过 `platform_err_t` 获取参数或硬件错误。

### 8.2 建议后续修改

这些修改用于强化边界，不影响当前总体设计：

1. 将 `PLAT_GPIO_ID_LED1/KEY1` 这类板级名称逐步改为匿名 Platform 实例，LED/按键含义由 BSP 保存。
2. UART 资源表改用 `[PLAT_UART_...] = ...` 指定下标初始化。
3. 统一 ID 有效性检查，覆盖枚举被强制转换为负值的情况。
4. 明确 `size == 0U`、空回调、重复初始化和并发调用的行为。
5. 在 UART 接口中写明接收回调运行于中断上下文，并定义 DMA 缓冲区和 Cache 维护责任。
6. 检查 `s_buf_size_table` 是否仍参与接收位置计算；若设计已不需要，应在单独改动中移除，而不是长期保留无效状态。

### 8.3 当前不需要修改

- 不需要引入动态注册中心；
- 不需要为 GPIO/UART 增加一套全局 `ops` 表；
- 不需要让 BSP 或上层持有 HAL 句柄；
- 不需要因少量板级命名问题推翻现有 `plat_mcu` 结构。

## 9. 新增接口检查清单

新增或修改 `plat_*` 接口时检查：

- [ ] 接口表达的是 MCU 基础能力，而非板级器件或业务语义；
- [ ] 公共头文件没有厂商 SDK/HAL 类型；
- [ ] 参数使用定宽类型并完成范围、空指针和长度检查；
- [ ] 资源表私有且使用安全索引；
- [ ] 错误统一转换为 `platform_err_t`；
- [ ] 硬件等待具有超时；
- [ ] ISR/任务上下文和并发规则明确；
- [ ] DMA 的内存域、对齐、所有权和 Cache 责任明确；
- [ ] CMake 只选择一个具体 MCU 实现；
- [ ] 构建受影响的 Debug preset 且没有新增警告或错误。
