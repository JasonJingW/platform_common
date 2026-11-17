# Kconfig 配置工具

从工作区 `E:/Project/EtherNet/Kconfiglib` 复制所需文件，由 platform_common 直接管理，不包含 `.git`，未修改上游源码。

- 上游：<https://github.com/ulfalizer/Kconfiglib>
- 来源提交：`061e71f7d78cb057762d88de088055361863deff`。
- 版本：14.1.0（来源仓库 setup.py）。
- 许可证：ISC，保留 `LICENSE.txt` 和源码版权声明。
- `source_manifest.csv`：复制文件的原始路径、目标路径和 SHA256。
- `upstream_readme.rst`：上游完整使用说明。

## 保留的工具

| 文件 | 用途 |
|---|---|
| `kconfiglib.py` | 配置解析、依赖计算、读取及保存配置的核心库 |
| `guiconfig.py` | Tkinter 图形配置界面 |
| `menuconfig.py` | 终端配置界面 |
| `genconfig.py` | 从配置生成 C 宏头文件 |
| `defconfig.py`、`olddefconfig.py`、`oldconfig.py` | 加载默认配置、更新配置及处理新增选项 |
| `savedefconfig.py` | 导出精简的用户配置 |
| `setconfig.py`、`listnewconfig.py` | 命令行修改参数、列出新选项 |
| `all*config.py` | 生成各类默认/全开/全关配置 |
| `examples/Kmenuconfig` | 上游菜单、类型、依赖和选择项示例 |
| `examples/merge_config.py` 等 | 合并配置、查询配置树及符号的示例脚本 |

未复制测试集、安装打包文件、Linux 内核 Makefile 补丁、依赖 readline 的教学界面，以及 Windows 下只包含符号链接目标文本的 examples/kconfiglib.py。独立运行 examples 下的 Python 查询脚本时，应把工具根目录加入 PYTHONPATH，以便导入 kconfiglib。

## Windows 运行方式

运行时使用 Python 3.9+。项目入口 python config.py 默认使用 menuconfig，在当前终端显示菜单，不弹出窗口。

从主仓库根目录执行：

```powershell
# 新机器首次安装终端依赖（本机已安装）。
python config.py setup

# 打开当前项目的终端菜单。
python config.py

# 检查已保存配置和生成文件。
python config.py check
```

Windows 的 windows-curses 按 Python 主/次版本安装到 DEV_TOOLS_HOME 指定的工程外工具目录，setup 在当前 Python 环境注册 .pth 搜索路径。终端需支持交互；方向键/Enter 操作，Space 切换，S 保存，Q 退出。guiconfig 仍是上游提供的可选界面，但项目默认入口使用终端。

直接运行上游 menuconfig.py 时，需自行提供相应 curses 模块的 Python 搜索路径；推荐使用项目入口。上游示例菜单仍保留在 examples/Kmenuconfig，其他用法参见 upstream_readme.rst。

## 当前验证与后续集成

已使用复制后的文件完成：解析官方示例、保存 .config、生成 C 头文件，并验证 guiconfig 命令行入口、Tkinter 导入及项目菜单窗口创建/事件循环（隐藏窗口自动关闭检查）。

当前主项目已通过根目录 config.py 接入芯片、FreeRTOS、任务和 RTT 配置；配置与生成流程见主项目 README.md 的配置工具章节。工具源码保持上游版本，项目菜单与生成适配放在各自模块和主项目中。配置生成在电脑端执行，这些 Python 文件不参与芯片固件编译。

终端界面已在本机交互终端验证：进入 Tasks/Heartbeat 菜单、显示各任务参数、保存并退出，随后正常生成配置。
