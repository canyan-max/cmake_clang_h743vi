# 平台层规则

- `01_common` 放公共平台类型与错误码；`02_bsp` 提供稳定的板级逻辑能力并组装具体器件；`03_mcu_interface` 定义与器件无关的 `plat_*` MCU 抽象接口。
- `00_Board/Inc/board_resources.h` 只存放不依赖 HAL/CubeMX 的板级资源编号、电气属性和器件配置，供 BSP 与 `05_Impl/01_mcu` 通过 `board_resources` INTERFACE 目标私有依赖；`00_Board/Inc/board_stm32h743_binding.h` 保存资源到 STM32H743 引脚、外设句柄的绑定，只允许 `05_Impl/01_mcu` 通过 `board_stm32h743_binding` 目标使用。
- `plat_resource_id_t` 只统一资源 ID 的存储宽度；各公共接口继续使用 `plat_gpio_id_t`、`plat_i2c_id_t`、`plat_spi_id_t`、`plat_uart_id_t` 等类别类型。`mcu_interface` 不定义具体板级实例、用途名称或资源数量，这些由 `board_resources.h` 管理。
- BSP 公共接口可以描述显示区域、像素格式、帧事件等上层必须知道的能力，不得暴露芯片型号、SPI/DCMI 等总线细节、引脚或 HAL 类型。
- `bsp_interface` 若作为 CMake 接口目标保留，只负责导出 BSP 公共头文件；它不是 MCU 实现契约，`05_Impl/01_mcu` 不得包含或调用 `bsp_*` API。
- BSP 可以依赖 `board_resources.h`、`plat_*` 和 `06_Components/ChipDrivers`，但不得包含 `board_stm32h743_binding.h` 或直接调用 HAL；芯片命令、寄存器和初始化序列放在 ChipDrivers，HAL 调用保留在 `05_Impl`。
- 新增或修改外设时，保持“Service → BSP → ChipDrivers/plat_* → MCU 实现”的依赖方向，不再引入通用 Device 或 BSP Binding 转发层。
- HAL 外设回调在 MCU 实现层终止并转换为 `plat_*` 事件；BSP 注册处理函数，Service 不直接实现或引用 `HAL_*Callback`。
- 涉及 DMA 的接口必须明确缓冲区所有权、长度、对齐要求和缓存维护责任。
- 平台公共头文件直接使用 `<stdint.h>` 等标准类型并保持自包含；不得建立 `plat_u8_t`、`plat_u16_t` 等无领域语义的基础类型包装。
