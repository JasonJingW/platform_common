# platform_common

- `scripts/build/project_flash.py`：J-Link 烧录入口，从根仓构建配置及 project_config.h 读取参数，支持命令预览。
- `scripts/build/project_build.py`：通用构建入口，读取主仓 build 后端的 build.json 并转发命令，使用说明见 `scripts/build/readme.md`。

- `core/stm32f7xx/`：STM32F7 芯片相关的 HAL、CMSIS、启动文件、系统文件和 HAL 配置，见该目录下的说明。
- `FreeRTOS/`：FreeRTOS 内核、公共头文件、GCC/Cortex-M7 移植选项、heap_4、配置模板及许可证；不保留独立 `.git`，集成步骤见该目录 README。
- `CMakeLists.txt`：仅声明可选源码接口 common::stm32f7xx、freertos_* 和 rtt，最终由主仓构建目标选择。
- `RTT/`：SEGGER RTT 文件，由 platform_common 直接管理，不保留独立的 .git。使用 `RTT/RTT/SEGGER_RTT.c`、`RTT/RTT/SEGGER_RTT_printf.c`，包含路径为 `RTT/RTT` 和 `RTT/Config`；配置位于 `RTT/Config/SEGGER_RTT_Conf.h`。主项目不再保留重复副本。
- `RTT/RTT/SEGGER_RTT.h`：内置 RTT 日志宏。使用原有 RTT 包含路径，启动时调用 `segger_rtt_init()`；`RLOG_INFO`、`RLOG_ERR`、`RLOG_WAR` 分别输出绿色、红色、黄色日志，自动附带级别、恢复默认颜色并换行。日志正文无需手动添加换行，格式参数遵循 `SEGGER_RTT_printf`（不支持浮点等完整标准 printf 格式）。颜色需查看终端支持 ANSI 控制序列。

GPIO、板级时钟初始化等实现由使用方的 BSP 提供。`core/stm32f7xx/config/stm32f7xx_hal_conf.h` 保存当前 HAL 模块开关和晶振参数，多项目复用时需按应用核对或覆盖配置。

当前芯片文件针对 STM32F767；切换其他 STM32F7 型号时，需补齐对应的设备头文件、启动文件，并修改型号宏和链接配置。

应用入口位于独立应用子模块的 `main.c`；`core` 下仅保留芯片相关目录。
