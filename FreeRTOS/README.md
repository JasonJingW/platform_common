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

## 接入现有工程时需处理

- 中断入口：启动文件使用 `SVC_Handler`、`PendSV_Handler` 和 `SysTick_Handler`。将移植层的 `vPortSVCHandler`、`xPortPendSVHandler` 对应到启动入口，并将 RTOS tick 正确接入 SysTick。
- 当前 `core/stm32f7xx/system_init.c` 已实现 SysTick，只调用 `HAL_IncTick()`。接入调度器时需调整此处，保留 HAL 的 1ms 时基并正确驱动 RTOS tick；不能再额外定义同名 SysTick。HAL 与 RTOS tick 频率不同时应另设时基或做正确的分频处理。
- 配置 CPU 实际频率、tick 频率、中断优先级、任务栈、堆及可选钩子；调用 FromISR API 的中断必须满足移植层的优先级约束。
- 应用创建任务后调用 `vTaskStartScheduler()`；现有 `main.c` 的等待中断循环尚未改为任务调度。
- FreeRTOS API 头应在项目实际配置完成后再纳入 `project_config.h`，顺序为 `FreeRTOS.h` 在前，然后 `task.h`、`queue.h` 等所用接口。

此次仅整理依赖及说明，未启用调度器。内核、两种备选移植层和 heap_4 使用临时验证配置进行 ARM GCC 语法检查；不代表板级配置、完整链接或上板运行已验证。
