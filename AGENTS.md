# AGENTS.md

本文件适用于整个仓库；进入子目录时，还必须遵守该目录下的 `AGENTS.md`。

## 工作原则

- 先明确成功标准、影响范围和验证方式；多步骤修改先给出简要计划。
- 需求存在会影响接口、实时性或硬件行为的歧义时，说明假设并向用户确认。
- 只做与需求直接相关的手术式修改；不顺带重构、格式化或删除既有死代码。
- 嵌入式代码必须保留参数检查、数组边界检查、硬件等待超时和必要的缓存一致性处理。
- 完成改动后必须构建受影响的 preset，并确保没有新增警告或错误。

## 项目环境与构建

| 项目 | 值 |
| --- | --- |
| MCU | STM32H743VIT6（Cortex-M7，400 MHz，FPU，双 Bank Flash） |
| 工具链 | starm-clang（LLVM/clang）+ CMake + Ninja |
| RTOS | FreeRTOS Kernel V10.6.2（CMSIS-OS2） |
| 调试器 | J-Link |

```bash
./runbuid_and_flash.sh -b               # Debug：仅构建
./runbuid_and_flash.sh                   # Debug：构建并烧录
./runbuid_and_flash.sh -c                # 清理后构建，不烧录
./runbuid_and_flash.sh -f                # 仅烧录已有镜像
./runbuid_and_flash.sh -p Release -b     # Release：仅构建
```

构建产物位于 `build/<preset>/cmake-clang-h743vi.{elf,bin,hex}`。常规改动优先使用 `-b`，只有用户明确要求时才烧录。

## 目录与依赖方向

仓库采用自上而下的分层；上层可以依赖下层公开接口，下层不得反向依赖上层。

```text
01_App（应用入口；当前可为空）
  ↓
02_Service（业务编排；保持 RTOS 与硬件实现无关）
  ↓ 公共 bsp_* 能力
04_Platform/02_bsp（板级能力与器件组装）
  ├── 06_Components/ChipDrivers（可复用芯片协议驱动）
  └── 04_Platform/03_mcu_interface（通用 plat_* MCU 抽象）
        ↓
      05_Impl/01_mcu（STM32 HAL/LL 具体实现）

06_Components（工程主动维护的可复用驱动、协议、算法和工具组件）

00_Board（板级资源、电气属性与 STM32H743 连接绑定）
  ├── BSP
  └── 05_Impl/01_mcu
```

`00_Board` 是 BSP 和 MCU 实现共同的基础配置，不包含业务逻辑。`board_resources.h` 保存不依赖 HAL/CubeMX 的板级资源和固定属性；`board_stm32h743_binding.h` 保存资源到 STM32H743 引脚与外设句柄的绑定，只允许 MCU 实现使用。工程不再设置通用 `03_Device`、`03_Device_interface` 或 BSP Binding 层；只有出现跨设备聚合、复杂生命周期、并发仲裁或真正可替换的设备语义时，才按需增加独立抽象。

`plat_gpio_id_t`、`plat_i2c_id_t`、`plat_spi_id_t`、`plat_uart_id_t` 等是按外设类别区分的不透明资源 ID，可以统一以 `plat_resource_id_t` 存储，但公共 API 不得退化为无类别的通用 ID。具体资源名称、用途别名和数量由 `board_resources.h` 定义；MCU 实现检查范围后，通过 `board_stm32h743_binding.h` 映射到 HAL 引脚或句柄。`mcu_interface` 不定义 LED1、I2C0、协议串口等板级实例。

### 分层依赖边界

下表定义新增依赖时的默认边界。受控例外只能使用目标层公开的头文件和 CMake 接口目标，不得访问私有状态或实现文件。

| 层 | 常规依赖 | 受控例外 | 禁止 |
| --- | --- | --- | --- |
| `01_App` | `02_Service` | FreeRTOS/CMSIS-OS2 任务控制接口、通用日志中间件 | BSP、`plat_*`、HAL、CubeMX |
| `02_Service` | 公共 `bsp_*`、中间件 | `plat_log`、`plat_time`、必要诊断接口 | `board_resources.h`、BSP 私有状态、HAL、CubeMX、RTOS 实现细节 |
| `04_Platform/02_bsp` | `board_resources`、`mcu_interface`、`ChipDrivers`、必要中间件 | 无 | Service、App、`board_stm32h743_binding`、HAL、CubeMX |
| `04_Platform/03_mcu_interface` | `01_common` | 通用日志中间件 | BSP、Board、Service、App、HAL、CubeMX |
| `05_Impl/01_mcu` | `mcu_interface`、`board_resources`、`board_stm32h743_binding`、CubeMX、`01_common` | 无 | Service、BSP 实现与公共 API、ChipDrivers |
| `06_Components` | 标准 C 与显式声明的通用组件 | 芯片驱动可依赖字库等纯软件资源 | `board_resources.h`、BSP、`plat_*`、HAL、CubeMX、RTOS、业务层 |

`Core/` 是 CubeMX 生成层：保留启动、外设初始化、中断入口与任务入口壳；业务流程应委托给 `01_App` 或 `02_Service`。BSP 对 Service 只公开板级逻辑能力，例如显示区域、像素格式和帧事件，不公开芯片型号、总线、引脚或 HAL 句柄。`06_Components/` 保存工程主动维护的可复用组件；`Middlewares/Third_Party/` 由 CubeMX 管理。`Drivers/` 与 CubeMX 第三方中间件代码默认不修改，除非需求明确涉及它们。

## 内存、实时性与 DMA

以 `STM32H743XX_FLASH.ld` 为准：Flash 2 MB；DTCM 128 KB；AXI RAM 512 KB；RAM_D2 288 KB；RAM_D3 64 KB；ITCM 64 KB。链接脚本当前定义 `_Min_Stack_Size = 0x1000`、`_Min_Heap_Size = 0x200`。

- FreeRTOS 堆由 `heap_4.c` 与 `configTOTAL_HEAP_SIZE` 管理；项目的 `ucHeap` 在 `Core/Src/freertos.c` 中定义。
- 禁止直接使用 `malloc`/`free`。只有确有必要时才使用 `pvPortMalloc`，并说明生命周期、失败处理及内存影响。
- DMA 缓冲区放入合适的非缓存/专用段，或在 DMA 前后完成 D-Cache 清理/失效；不得假定缓存自动一致。
- UART 中断响应小于 10 µs、1 kHz 控制环仅适用于明确承担这些职责的模块；变更相关路径时必须评估并验证时序影响。

## C 编码规则

- 业务数据和硬件寄存器接口使用 `<stdint.h>` 的定宽类型；常量使用 `U` 后缀，避免魔术数字。
- 直接使用标准 `<stdint.h>` 类型，不新增 `plat_u8_t`、`u16` 等无业务语义的整数别名；项目自定义类型应表达资源、状态、型号或单位等领域含义。公共头文件必须自行包含其声明所需的标准头文件，不依赖传递包含。
- `int`、`long` 仅用于第三方/HAL/标准库接口或必要索引场景；跨模块接口优先使用定宽类型，并在边界显式转换。
- 全局变量使用 `g_` 前缀；文件静态变量不加此前缀；宏全大写；函数使用小写下划线风格。新类型遵循相邻模块的现有命名风格。
- 对外 API、驱动/BSP 接口、ISR 回调及复杂或有副作用的私有函数须使用项目 Doxygen 风格注释；简单私有辅助函数可省略冗余注释。
- 公共接口和缓冲区操作必须检查 `NULL`、范围及初始化状态；硬件轮询必须有超时；每个 `switch` 必须包含 `default`。

## DWT 计时

`plat_time_init()` 目前由 `MX_FREERTOS_Init()` 调用，STM32 实现使用 DWT 提供微秒时间。使用 `plat_time_get_us()` 前应确认该初始化路径已经执行；不要因测量单个函数而重复初始化。若测量发生在 FreeRTOS 初始化之前，需在调用点之前显式初始化并说明原因。
