# 平台层规则

- `01_common` 放公共平台类型与错误码；`02_bsp` 放板级外设能力；`02_bsp_binding` 负责将 BSP 绑定到设备接口；`03_mcu_interface` 定义 `plat_*` MCU 抽象接口。
- `00_Board/Inc/board_config.h` 集中存放板级引脚、外设句柄和芯片配置；BSP 与 `05_Impl/01_mcu` 通过 `board_config` INTERFACE 目标私有依赖它，且不得因此互相链接。
- `bsp_interface` 仅导出 BSP 公共驱动契约；`05_Impl/01_mcu` 可私有依赖它以实现芯片驱动 ops，但不得链接 `bsp` 静态库。
- BSP 可以引用 `board_config.h` 和 `plat_*` 接口，但不得直接调用 HAL；HAL 调用必须保留在 `05_Impl`。
- 新增或修改外设时，保持“Device → BSP/Binding → plat_* → MCU 实现”的依赖方向，并处理参数范围与硬件错误。
- 涉及 DMA 的接口必须明确缓冲区所有权、长度、对齐要求和缓存维护责任。
