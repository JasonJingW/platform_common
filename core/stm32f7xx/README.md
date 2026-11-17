# STM32F7 芯片依赖

本目录集中存放与 STM32F7 芯片相关的文件，当前提供 STM32F767 所需子集。

```text
stm32f7xx/
├── system_init.c
├── system_init.h
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

`config/stm32f767xgx_config.h` 保存当前 STM32F767xGx 的物理内存容量及可调整 RTOS 参数。该型号 Flash 为 1 MiB；系统 RAM 为 SRAM1 368 KiB、SRAM2 16 KiB、DTCM 128 KiB，共 512 KiB。另有 ITCM 16 KiB 和备份 SRAM 4 KiB，不将其并入普通连续 RAM 预算。容量宏用于描述硬件，不能替代链接脚本，也不会自动改变堆的存放区域。

可调整参数采用 `CHIP_RTOS_*` 命名：`TOTAL_HEAP_SIZE`（字节）、`MINIMAL_STACK_SIZE`、`TIMER_TASK_STACK_DEPTH`、`APP_TASK_STACK_DEPTH`（32 位字数），以及 `TIMER_QUEUE_LENGTH`、`MAX_PRIORITIES`、`MAX_TASK_NAME_LEN`、`TIMER_TASK_PRIORITY`。这些是当前配置值，不是由物理容量自动推导的最佳值；改变 RAM 区域或任务负载后需重新评估。

CMSIS 保留供应商内部目录层级及许可证。`hal_driver` 当前仅包含基础、GPIO、RCC、Cortex、PWR、FLASH 模块及其必要扩展文件。

## 集成路径

以下路径均相对于本目录：

- 头文件路径：本目录、`hal_driver/Inc`、`cmsis/Core/Include`、`cmsis/Device/ST/STM32F7xx/Include`、`config`。
- 系统初始化源文件：`system_init.c`，仅依赖芯片 HAL/CMSIS，不依赖 RTT。
- HAL 源文件：`hal_driver/Src/*.c`。
- 系统文件：`cmsis/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c`。
- GCC 启动文件：`cmsis/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f767xx.s`。
- 当前型号宏：`STM32F767xx`；使用 HAL 时定义 `USE_HAL_DRIVER`。

构建方还需提供适配具体芯片容量及内存布局的链接脚本、Cortex-M7 工具链参数、板级初始化和应用源文件。当前尚无构建工程，未验证编译或烧录。

`config/stm32f7xx_hal_conf.h` 是当前项目配置，包含 HAL 模块开关和 HSE 等参数；切换板卡时需核对。若使用应用自有配置，应将其所在目录加入包含路径，并避免同时引入多个同名配置。

应用在 `main()` 中先调用 `hardware_init()`，再初始化所需中间件和板级资源。`hardware_init()` 仅初始化 HAL 和芯片支持的缓存：根据 `__ICACHE_PRESENT`、`__DCACHE_PRESENT` 宏，开启存在的 I-cache、关闭存在的 D-cache；启动时调用一次。应用仍需提供调用 `HAL_IncTick()` 的 SysTick 处理函数。RTT 初始化及缓冲区策略由应用自行设置。

GPIO 引脚选择和板级 PLL/时钟设置由主项目的 `board/bsp/board.c` 提供，使用方将 `board/bsp` 加入包含路径。应用入口和业务代码不放入本芯片目录。
