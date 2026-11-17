# STM32F7 芯片依赖

本目录集中存放与 STM32F7 芯片相关的文件，当前提供 STM32F767 所需子集。

```text
stm32f7xx/
├── hal_driver/
│   ├── Inc/
│   ├── Src/
│   └── LICENSE.md
├── cmsis/
│   ├── Core/Include/
│   ├── Device/ST/STM32F7xx/
│   │   ├── Include/
│   │   └── Source/Templates/
│   │       ├── system_stm32f7xx.c
│   │       └── gcc/startup_stm32f767xx.s
│   └── LICENSE.txt
└── config/stm32f7xx_hal_conf.h
```

CMSIS 保留供应商内部目录层级及许可证。`hal_driver` 当前仅包含基础、GPIO、RCC、Cortex、PWR、FLASH、ETH 模块及其必要扩展文件。

## 集成路径

以下路径均相对于本目录：

- 头文件路径：`hal_driver/Inc`、`cmsis/Core/Include`、`cmsis/Device/ST/STM32F7xx/Include`、`config`。
- HAL 源文件：`hal_driver/Src/*.c`。
- 系统文件：`cmsis/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c`。
- GCC 启动文件：`cmsis/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f767xx.s`。
- 当前型号宏：`STM32F767xx`；使用 HAL 时定义 `USE_HAL_DRIVER`。

构建方还需提供适配具体芯片容量及内存布局的链接脚本、Cortex-M7 工具链参数、板级初始化和应用源文件。当前尚无构建工程，未验证编译或烧录。

`config/stm32f7xx_hal_conf.h` 是当前项目配置，包含 HAL 模块开关和 HSE 等参数；切换板卡时需核对。若使用应用自有配置，应将其所在目录加入包含路径，并避免同时引入多个同名配置。

GPIO 引脚选择、板级 PLL/时钟设置、PHY 复位及 `HAL_ETH_MspInit()` 由主项目的 `BSP/board.c` 提供。应用入口、网络地址和业务代码不放入本芯片目录。
