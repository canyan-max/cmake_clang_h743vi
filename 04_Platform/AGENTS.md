# 平台层规则

- `01_common` 放公共平台类型与错误码；`02_bsp` 提供稳定的板级逻辑能力并组装具体器件；`03_mcu_interface` 定义与器件无关的 `plat_*` MCU 抽象接口。
- `00_Board/Inc/board_config.h` 集中存放板级引脚、外设句柄和器件配置；BSP 与 `05_Impl/01_mcu` 通过 `board_config` INTERFACE 目标私有依赖它，且不得因此互相链接。
- BSP 公共接口可以描述显示区域、像素格式、帧事件等上层必须知道的能力，不得暴露芯片型号、SPI/DCMI 等总线细节、引脚或 HAL 类型。
- `bsp_interface` 若作为 CMake 接口目标保留，只负责导出 BSP 公共头文件；它不是 MCU 实现契约，`05_Impl/01_mcu` 不得包含或调用 `bsp_*` API。
- BSP 可以依赖 `board_config.h`、`plat_*` 和 `06_Components/ChipDrivers`，但不得直接调用 HAL；芯片命令、寄存器和初始化序列放在 ChipDrivers，HAL 调用保留在 `05_Impl`。
- 新增或修改外设时，保持“Service → BSP → ChipDrivers/plat_* → MCU 实现”的依赖方向，不再引入通用 Device 或 BSP Binding 转发层。
- HAL 外设回调在 MCU 实现层终止并转换为 `plat_*` 事件；BSP 注册处理函数，Service 不直接实现或引用 `HAL_*Callback`。
- 涉及 DMA 的接口必须明确缓冲区所有权、长度、对齐要求和缓存维护责任。
