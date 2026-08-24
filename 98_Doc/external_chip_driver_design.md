# MCU 外挂芯片驱动设计规范

## 1. 目的

本文定义 MCU 外挂芯片的通用设计方式，适用于 AT24、AHTxx、W25Qxx、RTC、显示和其他通过 I2C、SPI、UART 等总线连接的芯片。

目标是让芯片协议代码能够跨板卡、跨 MCU、跨工程复用。本文只描述稳定的设计方式，不包含当前工程的迁移计划。MCU 基础能力规则见 [`plat_mcu` 设计规范](./plat_mcu_design.md)。

## 2. 核心原则

最重要的区分是：

> `06_Components` 描述“这一类芯片怎样工作”，BSP 描述“当前板上具体装了哪个对象，以及它怎样连接”。

例如，`06_Components/ChipDrivers/at24cxx_driver.*` 是可复制到其他工程的 AT24Cxx 驱动；`bsp_eeprom` 则是当前板上的那颗 EEPROM，它明确选择 AT24C02、I2C0、地址 `0x50`、容量和页大小，并把这些信息装配成一个可使用的板载对象。

可以把这种关系理解为：

```text
可复用的芯片类型（Component）
             +
板级资源与连接方式（Board/BSP）
             =
当前板上的具体对象（bsp_xxx）
```

外挂芯片同时涉及三类知识，必须分开：

| 知识 | 回答的问题 | 归属 |
| --- | --- | --- |
| 芯片协议 | 为什么发送这些字节 | `06_Components/<chip>` |
| 板级装配 | 当前板装了什么、怎样连接并形成具体对象 | Board/BSP |
| MCU 实现 | 怎样把字节送上总线 | `plat_<bus>` + MCU Impl |

完整关系：

```text
Service
   ↓ 使用板级能力
BSP
   ↓ 创建芯片实例、提供 Adapter
06_Components/<chip>
   ↓ 通过 transport 请求宿主能力
plat_i2c / plat_spi / ...
   ↓
MCU Impl
   ↓
Vendor SDK / Hardware
```

换芯片、换板和换 MCU 时，应分别修改不同模块：

| 变化 | 主要修改位置 |
| --- | --- |
| AT24C02 换成 Component 已支持的 AT24 型号 | BSP |
| AT24 换成另一系列 EEPROM | 新 Component + BSP |
| AT24 由 I2C0 改接 I2C1 | Board/BSP 实例配置 |
| STM32 换成 NXP | MCU Impl |
| 把 AT24 拿到另一个软件框架 | 重新实现薄 Adapter |

## 3. 四个设计角色

### 3.1 Component：芯片协议

Component 负责芯片自身的规则，例如：

- 命令、寄存器和数据格式；
- 初始化序列和状态检查；
- CRC、数据换算和协议校验；
- 芯片规定的延时和超时；
- 型号参数和芯片内部状态。

Component 不得依赖：

- `plat_*`、`board_config.h`；
- HAL/LL、CubeMX；
- FreeRTOS/CMSIS-OS；
- 当前工程日志和业务模块；
- 当前板使用的总线、地址和引脚。

判断标准：把整个 Component 复制到另一个工程，只重新实现 transport，芯片协议源码不需要修改。

### 3.2 Transport：Component 的最小宿主契约

Transport 表达芯片驱动需要宿主提供的能力，例如：

- write、read、write-read；
- 检查设备是否应答；
- 获取时间或延时；
- 控制芯片专用片选、复位或电源。

规则是：

> 谁需要能力，谁定义接口。

因此 AT24Cxx 定义 `at24cxx_transport_t`，AHTxx 定义 `ahtxx_transport_t`。不要复制 HAL API，也不要设计一张包含所有总线能力的万能 `ops` 表。

### 3.3 Adapter：连接 Component 与 Platform

Adapter 实现 transport，并调用当前工程的 `plat_*`：

```text
at24cxx_transport.read/write
            ↓
at24cxx_plat_adapter
            ↓
plat_i2c_memory_read/write
```

Adapter 只负责：

- 绑定当前板使用的逻辑总线 ID，或在需要多实例时从 context 取得；
- 转发到 `plat_*`；
- 转换错误码；
- 保证 transport 约定的事务语义。

Adapter 不实现芯片协议，也不直接调用 HAL。

### 3.4 BSP：当前板的芯片实例

BSP 不是通用芯片驱动的存放处。它的主要职责是把 Component 实例化为当前板上真实存在、连接关系固定的对象。

BSP 负责：

- 选择芯片型号；
- 指定逻辑总线、地址、片选和板级引脚；
- 创建静态芯片实例；只有存在多实例或运行时选路需求时才创建 context；
- 处理 WP、复位和供电等板级连接；
- 对上提供稳定的 `bsp_*` 能力。

BSP 不重复实现芯片协议，也不直接调用 HAL。

以当前板载 EEPROM 为例：

```text
06_Components/ChipDrivers/at24cxx_driver
    提供 AT24 类型及其协议能力
              ↓ 由 BSP 选择和装配
bsp_eeprom
    = AT24CXX_MODEL_C02
    + BOARD_I2C_EEPROM_BUS
    + BOARD_EEPROM_I2C_ADDRESS_7B
    + 256-byte capacity
    + 8-byte page
    + 当前板的 WP/供电连接（如果存在）
```

因此，`bsp_eeprom_read()` 表达的是“读取当前板载 EEPROM”，而不是“操作任意一颗 AT24”。上层通常只关心板载能力，不需要知道具体芯片型号、总线编号和器件地址。

### 3.5 Board 与 BSP 的区别

Board 和 BSP 都包含板级知识，但职责不同：

| 模块 | 主要内容 | 是否执行驱动逻辑 |
| --- | --- | --- |
| `00_Board` | 引脚、逻辑总线映射、器件地址、功能是否装配等静态资源配置 | 否 |
| BSP | 创建具体实例、连接 Adapter、处理板级控制并提供 `bsp_*` API | 是 |

`00_Board` 回答“资源是什么”，BSP 回答“怎样用这些资源组成当前板的设备能力”。不要在 `board_resources.h` 中放芯片协议流程，也不要让 Component 直接读取 Board 配置。

## 4. Component 的标准模型

可复用芯片组件通常由以下部分组成：

```text
chip_model_t        标准芯片型号；用于选择确实不同的协议和寻址规则
chip_transport_t    芯片需要的宿主能力
chip_t              一个独立芯片实例
void *context       可选：多实例或运行时宿主信息
```

### 4.1 Model 与可选 Config

不要为了集中参数而机械地建立 `chip_model_config_t`。芯片运行必需且由板上具体型号决定的简单属性，可以直接保存在驱动实例中，由 BSP 初始化。例如当前 AT24Cxx 实例直接保存容量和页大小，用于越界检查和跨页写入。

`chip_model_t` 只选择容量和页大小无法表达的协议差异。例如 AT24C02 使用 1-byte 字地址，AT24C16 还会把高位存储地址编码进器件地址；驱动根据 `model` 解析这些差异。不要仅根据容量猜测寻址规则。

只有参数数量确实增多、需要整体校验或被多个 API 反复传递时，才引入 Config 结构体。Config 中也不能保存 I2C1、`hi2c1`、板级引脚、RTOS mutex 或产品数据地址。

### 4.2 Transport 与可选 Context

简化示例：

```c
typedef struct AT24CXX_TRANSPORT_T
{
    at24cxx_status_t (*write)(uint8_t address_7b,
                              uint16_t word_address,
                              uint8_t word_address_size_bytes,
                              const uint8_t *p_data,
                              uint16_t size,
                              uint32_t timeout_ms);

    at24cxx_status_t (*read)(uint8_t address_7b,
                             uint16_t word_address,
                             uint8_t word_address_size_bytes,
                             uint8_t *p_data,
                             uint16_t size,
                             uint32_t timeout_ms);

    at24cxx_status_t (*wait_ready)(uint8_t address_7b,
                                   uint32_t timeout_ms);
} at24cxx_transport_t;
```

AT24Cxx Component 负责把逻辑存储地址解析为最终的器件地址、字地址和地址宽度；Transport 只执行一次已经解析完整的 memory transaction。这样调用者的数据缓冲区可以直接传到底层，不需要在驱动实例中设置“地址前缀 + 页数据”的工作缓冲区。

`wait_ready` 表示一次完整且有界的就绪等待。Component 决定何时等待，调用者通过 API 传入允许等待多久；Adapter 使用宿主的单次应答探测、时基和延时完成轮询。这样既保留芯片操作超时，又不要求 Component 分别注入 tick、delay 和单次 probe。

Context 不是组件式驱动的必选项。当前板只有一颗固定接在共享设备 I2C 总线上的 EEPROM，因此 Adapter 直接绑定 `BOARD_I2C_EEPROM_BUS`，实例无需保存 context：

```c
static at24cxx_status_t at24cxx_plat_write(
    uint8_t address_7b,
    uint16_t word_address,
    uint8_t word_address_size_bytes,
    const uint8_t *p_data,
    uint16_t size,
    uint32_t timeout_ms)
{
    return convert_platform_error(
        plat_i2c_memory_write(BOARD_I2C_EEPROM_BUS, address_7b,
                              word_address, convert_address_size(
                                  word_address_size_bytes),
                              p_data, size, timeout_ms));
}
```

只有满足下列任一条件时才增加 `void *p_context`：

- 同一 transport 需要服务多个独立实例；
- 总线、片选或宿主资源需要在运行时选择；
- 组件测试或移植环境不能通过固定 Adapter 绑定状态。

无论是否使用 context，Component 都只能调用 transport，不得访问 `plat_*`、Board 或 HAL。context 是解决实例选路的工具，不是组件化必须遵守的形式。

### 4.3 Instance

当前 AT24Cxx 实例只保存驱动工作真正需要的数据：

```c
typedef struct AT24CXX_DRIVER_T
{
    const at24cxx_transport_t *p_transport;
    uint32_t                   capacity_bytes;
    uint16_t                   page_size_bytes;
    at24cxx_model_t            model;
    uint8_t                    address_7b;
    uint8_t                    initialized;
} at24cxx_driver_t;
```

其中 `capacity_bytes` 用于范围检查，`page_size_bytes` 用于拆分跨页写入，`model` 只用于寻址规则等协议差异。实例不保存 context，也不保存工作缓冲区。

实例规则：

- 默认支持当前板所需的静态实例；确有多实例需求时再引入 context；
- 初始化时完整注入依赖；
- 不在 Component 内写死 `g_xxx_ops`；
- 不在 Component 内选择具体 MCU 总线；
- 初始化失败时保持明确的未初始化状态。

## 5. 芯片 API 应表达完整能力

Component 的公共 API 不应泄漏内部协议步骤。

AT24 对外适合提供：

```text
at24cxx_init
at24cxx_read
at24cxx_write
at24cxx_is_ready
```

以下内容应作为 AT24 私有实现：

```text
页边界计算
单页写入
存储地址编码
发起有界的写周期 ACK 等待
```

AHTxx 对外应提供初始化和测量结果，而不是要求调用者手工发送命令、轮询 Busy 位和换算原始数据。

## 6. Platform 与 Component 的边界

`plat_i2c`、`plat_spi` 等只提供 MCU 基础事务：

- 通过逻辑 ID 选择总线实例；
- 校验参数并转换厂商错误；
- 执行有超时的总线事务；
- 保证复合事务的原子性。

与外挂芯片有关的关键约定：

- I2C 公共接口统一使用未左移的 7 位地址；
- `write_read` 必须明确 repeated START 语义；
- SPI 必须明确片选由谁控制以及保持时间；
- 一次复合事务期间不得插入其他设备事务；
- Platform 不提供 `plat_at24_*()`、`plat_ahtxx_*()` 等具体芯片接口。

## 7. Device 是可选层

Device 不是每个外挂芯片都必须经过的层。只有它增加了以下价值时才建立：

- 独立状态机；
- DMA 缓冲区等资源所有权；
- 去抖、滤波、换算或聚合行为；
- 生命周期、并发或故障恢复；
- 多个真正可替换实现的统一语义。

如果 `device_xxx()` 只是检查参数后原样调用 `bsp_xxx()`，通常不需要 Device。

BSP 已经给出了当前板上的具体设备对象，因此不能仅因为存在一个外挂芯片就机械地再增加 Device。Device 必须提供独立于板级装配的新语义，才有存在价值。

| 场景 | 推荐归属 |
| --- | --- |
| EEPROM 原始字节读写 | BSP + EEPROM Component |
| 参数分区、CRC、双备份、版本管理 | 更高层存储模块 |
| 按键原始按下状态 | BSP |
| 去抖、短按和长按事件 | Device 或输入组件 |
| 摄像头寄存器协议 | 摄像头 Component |
| 帧缓冲区和采集生命周期 | Device |

## 8. 内存、并发和超时

### 内存

- 不使用 `malloc/free`；
- 默认不使用 `pvPortMalloc`；
- 确实需要的工作缓冲区必须静态、有界或由调用者提供；能够通过完整 transport 事务直接使用调用者缓冲区时，不增加中间缓冲区；
- 不在栈上创建大小不受控的页缓冲区；
- DMA 缓冲区必须明确所有者、内存域、对齐和 Cache 责任。

### 并发

- 同一芯片实例默认不保证并发安全；
- 调用者负责同一实例的串行访问；
- 复合总线事务必须保持原子性；
- 芯片内部等待期间不应长期占有共享总线；
- 通用 Component 不直接引入 RTOS mutex。

### 超时

必须区分：

```text
总线传输超时：一次总线事务没有完成
芯片操作超时：芯片内部状态长期没有就绪
```

两类等待都必须有界，不能用无限循环代替。

## 9. 错误边界

```text
Vendor/HAL error
      ↓ MCU Impl
platform_err_t
      ↓ Adapter
chip_status_t 的传输错误子集
      ↓ Component/BSP
项目公共错误
```

简单驱动允许 Transport 直接返回芯片状态码，避免建立内容几乎相同的两套枚举。Adapter 只能返回 `OK`、`BUSY`、`TIMEOUT` 和底层传输错误等与宿主操作有关的状态；参数、未初始化和范围错误仍由 Component 产生。

芯片错误至少应能区分参数错误、未初始化、范围错误、Busy、超时和底层传输错误，不能把所有失败都压缩成一个 `ERROR`。

## 10. 目录和 CMake 规则

推荐目录：

```text
06_Components/ChipDrivers/  通用芯片协议源码与头文件
04_Platform/02_bsp/      板级实例和 Adapter
04_Platform/03_mcu_interface/  总线公共契约
05_Impl/<target>/        MCU 总线实现
```

目标依赖：

```text
独立的 chip_driver target
    无项目内部依赖

bsp
    PRIVATE -> chip_driver
    PRIVATE -> mcu_interface
    PRIVATE -> board_config

impl_mcu
    PRIVATE -> mcu_interface
    PRIVATE -> board_config
    PRIVATE -> vendor SDK
```

禁止反向依赖：

```text
chip_driver -X-> bsp / mcu_interface / board_config / vendor SDK
impl_mcu    -X-> chip_driver / bsp
```

`06_Components/ChipDrivers` 使用目录级 `chip_drivers` 静态库统一管理依赖边界一致的外挂芯片驱动。各驱动仍须保持源码、头文件和 API 独立，不能依靠 BSP 顺带编译驱动源码。新增驱动通常只需加入 `CHIP_DRIVERS_SRC`；若某个驱动需要特殊第三方依赖、编译选项或独立测试，再为它拆分单独的 CMake target。

## 11. 两个示例

### AT24

```text
Component：范围检查、地址规则、分页、决定 ACK 等待时机
Transport：memory write、memory read、wait_ready
BSP：AT24C02、I2C0、地址 0x50、容量、页大小、WP 引脚
Platform：完成通用 I2C memory transaction
```

### AHTxx

```text
Component：初始化命令、测量命令、Busy、CRC、温湿度换算
Transport：write、read、时间能力
BSP：AHT20、I2C0、地址 0x38、供电方式
Platform：完成通用 I2C 事务
```

两个芯片共享 `plat_i2c`，但各自定义最小 transport 和独立协议实现。

## 12. 常见错误

- 为每个 I2C 芯片创建一个 `mcu_xxx.c`；
- 在 Component 中包含 HAL、Board、Platform 或 RTOS 头文件；
- 直接复制某个 HAL 的专用接口作为 transport；
- 在 Component 内写死全局 ops 或具体总线；
- 使用左移后的 I2C 地址穿过公共层；
- 把芯片分页、Busy polling 放进 Service；
- 让单个芯片初始化或反初始化共享总线；
- 为所有芯片设计一张万能 transport；
- 为了目录对称而建立纯转发 Device；
- 在 Component 中动态申请内存或直接使用 RTOS 锁。

## 13. 新增外挂芯片的设计流程

1. 列出芯片协议规则，不考虑 MCU API。
2. 定义芯片对外提供的完整能力。
3. 列出实现这些能力所需的最小宿主操作。
4. 由芯片定义 model、transport、instance 和错误码；简单且必要的芯片属性可以直接进入 instance，Config 仅在参数集合确实复杂时引入。
5. 明确内存、超时、并发和事务语义。
6. 使用 Adapter 将 transport 连接到 `plat_*`。
7. 在 Board 中声明静态资源，在 BSP 中选择型号、总线和地址并创建当前板实例。
8. 只有出现独立设备语义时才增加 Device。

## 14. 设计检查清单

- [ ] Component 能脱离当前工程编译；
- [ ] Component 不依赖 HAL、Board、Platform、RTOS 和业务；
- [ ] 芯片 API 没有泄漏内部协议步骤；
- [ ] transport 由芯片定义且保持最小化；
- [ ] context 仅在多实例或运行时选路确有需求时引入；
- [ ] Board 只声明静态资源，不包含芯片协议流程；
- [ ] BSP 负责把型号、总线、地址和板级连接装配成具体对象；
- [ ] BSP 对上表达板载能力，不泄漏不必要的芯片和总线细节；
- [ ] MCU Impl 只实现通用总线；
- [ ] 内存静态且有界；
- [ ] 总线传输和芯片操作均有超时；
- [ ] 并发所有权和复合事务原子性明确；
- [ ] Device 确实增加了转发之外的价值；
- [ ] CMake 不存在反向依赖或实现库循环。
