# 通用构建入口

project_build.py 只负责发现构建后端、读取 build.json、按原样转发参数和返回退出码。芯片、源码、功能开关、编译器、链接及产物规则均由主仓库 build 中的后端实现。

从主仓库根目录使用：

```powershell
python platform_common/scripts/build/project_build.py build
python platform_common/scripts/build/project_build.py build --backend build/gcc --build-type Release
```

也可直接运行 `python -B platform_common/scripts/build/project_build.py`，默认执行 build。入口打印所选命令、后端路径及最终成功/失败状态。无 --backend 时，从当前目录逐级向上查找 build 后端，再尝试脚本所在工程；先查找 build/build.json，否则选择 build 下唯一带 build.json 的目录；存在多个后端则要求显式选择。使用 Python 3.9+，后端通过当前解释器调用，无需平台包装脚本。

请在终端中通过 python 执行，以便保留完整日志。Windows 双击 .py 文件时，窗口可能在执行结束后立即关闭，导致看不到提示。

后端约定如下，命令名称和工具由后端自行提供：

```json
{
    "schema_version": 1,
    "commands": {
        "build": ["{python}", "{backend}/driver.py", "build"],
        "clean": ["{python}", "{backend}/driver.py", "clean"]
    }
}
```

`{python}` 是当前解释器，`{backend}` 是选中后端的绝对目录。后端在自身目录中执行，调用使用参数数组，不经过 shell；入口不修改后端配置。增加其他芯片或工具链时，在主仓库 build 下提供另一个后端即可，无需修改通用脚本。

## J-Link 烧录入口

`project_flash.py` 读取所选 build 后端的 build.json → flash 配置。默认从 project_config 指向的头文件读取 PROJECT_CHIP_MODEL，从 version_header 读取项目名称和版本，选择对应的 BIN。默认先调用同目录 project_build.py 编译，编译成功后再烧录；传入 --bin 才跳过编译、使用已有固件。编译失败立即退出，不启动 J-Link。工具、芯片和路径均未写死在通用脚本中。

在主仓库根目录执行：

```powershell
# 预览编译及烧录命令，不执行编译、不连接探针。
python platform_common/scripts/build/project_flash.py --dry-run

# 默认先编译，成功后烧录、校验并复位运行。
python platform_common/scripts/build/project_flash.py

# 跳过编译，直接烧录 build/build_file 中当前版本的 BIN。
python platform_common/scripts/build/project_flash.py --bin

# 指定探针、文件或临时覆盖芯片和速度。
python platform_common/scripts/build/project_flash.py --serial 12345678
python platform_common/scripts/build/project_flash.py --bin --file build/build_file/ETHERNET_0.0.1.hex
python platform_common/scripts/build/project_flash.py --device STM32F767IG --speed 1000
python platform_common/scripts/build/project_flash.py --no-run
```

默认参数由根仓维护：PROJECT_CHIP_MODEL 位于 board/config/project_config.h，当前为 STM32F767IG；接口 SWD、速度 4000 kHz、BIN 地址 0x08000000 位于 build/gcc/build.json。HEX/ELF 使用文件中自带的地址；更换芯片时还需同步适配编译宏、启动文件和链接布局，修改烧录型号不会改变编译目标。

工具优先级：--jlink → JLINK 环境变量 → build.json 的命令名/PATH → Windows SEGGER 安装注册表。本机识别到 J-Link Commander V9.94，探针参数 --serial 对应 USB 探针序列号或昵称，与 UART 无关。烧录使用临时命令文件和 LoadFile 默认校验，完成后复位运行；--no-run 保持暂停。--bin 使用已有产物，不检查源码是否更新；--dry-run 仅预览，不运行编译或 J-Link。失败返回非零退出码，默认超时 120 秒，可用 --timeout 调整。

命令依据：[SEGGER J-Link Commander](https://kb.segger.com/J-Link_Commander)。已验证命令预览、模拟执行和失败处理；尚未进行实物烧录测试。
