# 通用构建入口

build.py 只负责发现构建后端、读取 build.json、按原样转发参数和返回退出码。芯片、源码、功能开关、编译器、链接及产物规则均由主仓库 build 中的后端实现。

从主仓库根目录使用：

```powershell
python platform_common/scripts/build/build.py build
python platform_common/scripts/build/build.py build --backend build/gcc --build-type Release
```

也可直接运行 `python -B platform_common/scripts/build/build.py`，默认执行 build。入口打印所选命令、后端路径及最终成功/失败状态。无 --backend 时，从当前目录逐级向上查找 build 后端，再尝试脚本所在工程；先查找 build/build.json，否则选择 build 下唯一带 build.json 的目录；存在多个后端则要求显式选择。使用 Python 3.9+，后端通过当前解释器调用，无需平台包装脚本。

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
