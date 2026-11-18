# FreeRTOS 内核依赖

从工作区的 `FreeRTOS-Kernel` 仓库复制，由 platform_common 直接管理，不包含 `.git`，未修改上游源码。

- 上游：FreeRTOS/FreeRTOS-Kernel。
- 来源提交：`ca759444e9ca59cfa3a9faa6cb8b49e0772ca520`。
- 源码版本标记：`V11.1.0+`（开发版本，不等同于 V11.1.0 正式标签）。
- 许可证：MIT，保留 `LICENSE.md` 和源码版权声明。
- `SOURCE_MANIFEST.csv`：每个复制文件的来源路径和 SHA256。
- `UPSTREAM_README.md`、`History.txt`：上游使用说明及版本历史。

## 文件范围

```text
FreeRTOS/
├── include/                       公共内核头文件
├── tasks.c、queue.c、list.c        任务、队列和链表
├── timers.c                       软件定时器
├── event_groups.c                 事件组
├── stream_buffer.c                流缓冲和消息缓冲
├── croutine.c                     可选协程（通常不启用）
├── portable/
│   ├── GCC/ARM_CM4F/              非 r0p1 Cortex-M7 可选移植层
│   ├── GCC/ARM_CM7/r0p1/          Cortex-M7 移植层，包含勘误处理
│   └── MemMang/heap_4.c           支持释放和相邻空闲块合并的内存管理
└── config_template/FreeRTOSConfig.h  上游完整配置模板
```

仅复制 GCC/Cortex-M7 所需的移植选项和 heap_4，不包含其他处理器移植、演示工程及仓库管理文件。

## 集成方法

1. 在使用方（例如 `board/config`）创建项目实际使用的 `FreeRTOSConfig.h`，可参考 `config_template/FreeRTOSConfig.h`。模板不是已适配本板卡的配置，不应直接加入默认包含路径。
2. 加入包含路径：本目录的 `include`、选中的移植层目录，以及使用方配置头所在目录。
3. 编译 `tasks.c`、`queue.c`、`list.c`，按所用功能编译 `timers.c`、`event_groups.c`、`stream_buffer.c`；通常不需要 `croutine.c`。
4. 只能编译一个移植层的 `port.c`。根据本地上游 `portable/GCC/ARM_CM7/ReadMe.txt`：r0p1 使用 `GCC/ARM_CM7/r0p1`，其他 Cortex-M7 修订版推荐 `GCC/ARM_CM4F`；尚未确认修订版时，r0p1 移植层可用于所有 Cortex-M7 修订版。不要同时加入两个目录。
5. 使用动态内存时编译 `portable/MemMang/heap_4.c`，配置堆大小；只选择一个堆实现。heap_4 自行管理 FreeRTOS 堆，不依赖标准库 malloc 的 `_sbrk`。
6. 当前 STM32F767/GCC 目标使用 Cortex-M7、Thumb、FPv5-D16；浮点 ABI 应与其他源码和运行库一致。其他芯片或工具链需要补充对应移植层。

## 当前工程适配

- 实际配置位于主项目 `board/config/FreeRTOSConfig.h`，包含路径中应使用此目录，不能使用 `config_template` 替代。
- 该项目默认启用 `FREERTOS_USE_CHIP_CONFIG=1`，引入芯片配置 `core/stm32f7xx/config/stm32f767xgx_config.h`（相对于 platform_common）。每个 `CHIP_RTOS_*` 参数单独判断：已定义则使用芯片值，未定义则使用原默认值。设置 `FREERTOS_USE_CHIP_CONFIG=0` 可跳过芯片头；未从其他地方定义覆盖宏时，完全采用原默认配置。正常情况下直接修改芯片配置头文件中的参数，避免与编译选项重复定义。
- 选用 `portable/GCC/ARM_CM7/r0p1/port.c`，它可用于所有 Cortex-M7 修订版；不要再编译 CM4F 的 port.c。使用 `portable/MemMang/heap_4.c`，堆大小 32 KiB。
- CPU 频率取板级时钟初始化更新后的 `SystemCoreClock`；HAL 与 RTOS 共用 1 kHz SysTick，关闭 tickless idle。
- 配置将 `vPortSVCHandler`、`xPortPendSVHandler` 映射到启动文件的 `SVC_Handler`、`PendSV_Handler`。`system_init.c` 的 `SysTick_Handler()` 更新 HAL 时基，并在调度器启动后调用移植层的 `xPortSysTickHandler()`。
- `system_init.c` 通过编译检查要求 tick 频率为 1 kHz；如需其他频率或 tickless，需先调整 HAL 时基方案。
- 中断优先级最低为 15，调用 FromISR API 的中断优先级数值须不小于 5，且使用与板级设置一致的优先级分组。
- `project_config.h` 已按正确顺序引入 FreeRTOS 及任务、队列、信号量、定时器等接口；板级接口由使用方单独包含 `bsp_gpio.h`。
- `main.c` 初始化后调用 `task_start()`，由应用仓库的 `task/task_manage.c` 创建任务并启动调度器；任务栈默认 512 个 32 位字（2 KiB），当前每 500 ms 翻转 LED0、每 1000 ms 翻转 LED1。内存分配失败和栈溢出钩子也位于任务管理模块，发生错误后停留等待调试。

心跳任务调度位于 `platform_application/task/task_heartbeat.c`，状态灯逻辑位于 `platform_application/heartbeat.c`，主项目 `board/config/task_config.h` 仅保存优先级；创建函数、任务入口、名称、栈和循环延时间隔均在任务自身的 .c/.h 中管理。其中 `HEARTBEAT_TASK_STACK_DEPTH` 直接在任务头文件中定义为 512U，不进行芯片参数选择或条件回退。其余默认值保持为：heap 32 KiB、idle 栈 128 字、timer 栈 256 字、timer 队列 10 项、8 个优先级、名称长度 16、timer 优先级 2。物理 RAM 容量与 RTOS 分配预算分开定义；所有分配仍须满足链接区域和实际任务负载要求。

本仓声明 common::freertos_* 源码接口。主仓 build/gcc 固定组合内核、heap_4 和选定的移植层，并按配置编入定时器。configUSE_TIMERS 使用后端给出的 PROJECT_USE_SOFTWARE_TIMERS，GCC_TIMERS=AUTO 默认开启，可用 ON/OFF 显式覆盖。

当前已通过主项目 build/gcc 的完整固件编译、ELF 链接及 HEX/BIN 导出，验证了默认 RTOS、关闭组件、裸机和替代移植层组合；尚未上板运行。
