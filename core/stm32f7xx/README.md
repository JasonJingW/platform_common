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
- 系统初始化源文件：`system_init.c`，使用 HAL/CMSIS 和 FreeRTOS，并提供共享的 SysTick 入口。
- HAL 源文件：`hal_driver/Src/*.c`。
- 系统文件：`cmsis/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c`。
- GCC 启动文件：`cmsis/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f767xx.s`。
- 当前型号宏：`STM32F767xx`；使用 HAL 时定义 `USE_HAL_DRIVER`。

本仓仅提供 common::stm32f7xx 源码接口；当前主项目 build/gcc 提供工具链和链接脚本，已完成完整固件链接，尚未上板运行。最终构建方需要提供匹配芯片容量的内存布局、板级初始化和应用源文件。

`config/stm32f7xx_hal_conf.h` 是当前项目配置，包含 HAL 模块开关和 HSE 等参数；切换板卡时需核对。若使用应用自有配置，应将其所在目录加入包含路径，并避免同时引入多个同名配置。

应用在 `main()` 中先调用 `hardware_init()`，再初始化所需中间件和板级资源。`hardware_init()` 先在 D-cache 已启用时写回并关闭它，以便变更内存属性。随后用 MPU 将 `CHIP_NONCACHE_RAM_BASE` 起始的 16 KiB 共享 RAM 配置为不可缓存、不可执行，再开启 D-cache、I-cache 并初始化 HAL。链接脚本须提供 `__noncache_start__` 和 `__noncache_end__` 两个边界符号，并预留 16 KiB 对齐区域。RTT 初始化及缓冲区布局由应用和链接脚本负责。

`system_init.c` 的 `SysTick_Handler()` 更新 HAL 时基，并在调度器启动后调用 FreeRTOS 移植层的 `xPortSysTickHandler()`。SVC 和 PendSV 直接映射到 FreeRTOS 移植层。HardFault 等未覆盖的异常使用 STM32 启动文件的弱别名，进入 `Default_Handler` 停留以供调试。

`hardware_init()` 在 HAL 初始化后用 25 MHz HSE 和 PLL 配置 216 MHz 系统时钟及总线分频。时钟配置失败时停在初始化阶段，便于调试。GPIO 引脚由主项目的 `board/bsp/bsp_gpio.c` 管理。
