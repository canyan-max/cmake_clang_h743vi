# MCU 实现层规则

- `01_mcu` 是 STM32 HAL/LL 与 CubeMX 生成外设句柄的唯一实现边界；在此实现通用 `plat_*` 接口，不向上暴露 HAL 类型。通用板级事实由 `00_Board/Inc/board_resources.h` 提供，STM32H743 引脚和外设句柄绑定由仅本层可依赖的 `00_Board/Inc/board_stm32h743_binding.h` 提供。
- MCU 实现使用 Board 定义的资源数量建立映射表，访问前检查 `plat_*_id_t` 范围，并为固定映射表添加编译期数量校验。表项的 HAL 引脚或句柄只能来自 `board_stm32h743_binding.h`，不得在 `mcu_interface` 中定义具体外设编号。
- 本层只实现 MCU 传输、中断、DMA、缓存和时基等机制；不得包含或调用 `bsp_*`、ChipDrivers、Service 或 App 接口，也不得实现具体器件命令和业务流程。
- `HAL_*Callback` 在本层终止，转换为已注册的 `plat_*` 回调或事件；回调路径必须可在 ISR 上下文执行，不得阻塞或调用业务代码。
- 对硬件状态轮询使用有限超时，并将 HAL 错误转换为上层可处理的状态或错误码。
- 修改 DMA、Cache、中断优先级、时钟或外设初始化时，说明对实时性和缓存一致性的影响，并进行针对性验证。
- 新增源码或头文件时同步更新本目录与上级相关的 `CMakeLists.txt`。
