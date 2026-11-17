# platform_common

- `core/stm32f7xx/`：STM32F7 芯片相关的 HAL、CMSIS、启动文件、系统文件和 HAL 配置，见该目录下的说明。
- `RTT/`：SEGGER RTT 源码及其原有 Git 元数据。

GPIO、板级时钟初始化和 `HAL_ETH_MspInit()` 等实现由使用方的 BSP 提供。`core/stm32f7xx/config/stm32f7xx_hal_conf.h` 保存当前 HAL 模块开关和晶振参数，多项目复用时需按应用核对或覆盖配置。

当前芯片文件针对 STM32F767；切换其他 STM32F7 型号时，需补齐对应的设备头文件、启动文件，并修改型号宏和链接配置。

应用入口、应用配置和系统调用适配已合并到独立应用子模块的 `main.c`；`core` 下仅保留芯片相关目录。
