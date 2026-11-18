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

可调整参数采用 `CHIP_RTOS_*` 命名：`TOTAL_HEAP_SIZE`（字节）、`MINIMAL_STACK_SIZE`、`TIMER_TASK_STACK_DEPTH`（32 位字数），以及 `TIMER_QUEUE_LENGTH`、`MAX_PRIORITIES`、`MAX_TASK_NAME_LEN`、`TIMER_TASK_PRIORITY`。这些值直接在该芯片配置头文件中修改；不是由物理容量自动推导的最佳值；改变 RAM 区域或任务负载后需重新评估。

CMSIS 保留供应商内部目录层级及许可证。`hal_driver` 当前仅包含基础、GPIO、RCC、Cortex、PWR、FLASH 模块及其必要扩展文件。

## 集成路径

以下路径均相对于本目录：

- 头文件路径：本目录、`hal_driver/Inc`、`cmsis/Core/Include`、`cmsis/Device/ST/STM32F7xx/Include`、`config`。
- 系统初始化源文件：`system_init.c`，依赖 HAL/CMSIS，不依赖 RTT；根构建定义 PROJECT_USE_FREERTOS=1 时再接入 RTOS 时基及相应头文件，定义为 0 时保持裸机 HAL 时基。
- HAL 源文件：`hal_driver/Src/*.c`。
- 系统文件：`cmsis/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c`。
- GCC 启动文件：`cmsis/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f767xx.s`。
- 当前型号宏：`STM32F767xx`；使用 HAL 时定义 `USE_HAL_DRIVER`。

本仓仅提供 common::stm32f7xx 源码接口；当前主项目 build/gcc 提供工具链和链接脚本，已完成完整固件链接，尚未上板运行。最终构建方需要提供匹配芯片容量的内存布局、板级初始化和应用源文件。

`config/stm32f7xx_hal_conf.h` 是当前项目配置，包含 HAL 模块开关和 HSE 等参数；切换板卡时需核对。若使用应用自有配置，应将其所在目录加入包含路径，并避免同时引入多个同名配置。

应用在 `main()` 中先调用 `hardware_init()`，再初始化所需中间件和板级资源。`hardware_init()` 仅初始化 HAL 和芯片支持的缓存：根据 `__ICACHE_PRESENT`、`__DCACHE_PRESENT` 宏，开启存在的 I-cache；仅当 D-cache 已启用时调用 `SCB_DisableDCache()`，避免在复位后的未初始化缓存上执行 clean 操作。D-cache 保持关闭，启动时调用一次。RTT 初始化及缓冲区策略由应用自行设置。

`system_init.c` 同时提供 STM32F7 启动向量对应的 `SysTick_Handler()` 和 `HardFault_Handler()`：前者维护 HAL 毫秒计数，并在 FreeRTOS 调度开始后驱动内核 tick，要求配置为 1 kHz；后者关闭中断并停留在死循环，便于连接调试器检查，不依赖 RTT。SVC/PendSV 由 FreeRTOS 移植层提供，通过 FreeRTOSConfig.h 映射到启动入口。其他芯片应在各自硬件实现中提供对应处理函数；集成时避免重复定义同名中断处理函数。

GPIO 引脚选择和板级 PLL/时钟设置由主项目的 `board/bsp/board.c` 提供，使用方将 `board/bsp` 加入包含路径。应用入口和业务代码不放入本芯片目录。
