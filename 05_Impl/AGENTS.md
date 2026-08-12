# MCU 实现层规则

- `01_mcu` 是 STM32 HAL/LL 与 CubeMX 生成外设句柄的唯一实现边界；在此实现 `plat_*` 接口，不向上暴露 HAL 类型。板级配置由 `00_Board/Inc/board_config.h` 提供；芯片驱动实现可私有依赖 `bsp_interface`，但不得链接 BSP 实现库。
- 对硬件状态轮询使用有限超时，并将 HAL 错误转换为上层可处理的状态或错误码。
- 修改 DMA、Cache、中断优先级、时钟或外设初始化时，说明对实时性和缓存一致性的影响，并进行针对性验证。
- 新增源码或头文件时同步更新本目录与上级相关的 `CMakeLists.txt`。
