# 🔌 SerialPortDebugger — 跨平台串口调试助手

## 📖 概述

**SerialPortDebugger** 是一款基于 **Qt 5** 框架开发的**跨平台串口通信调试工具**，使用 **C++11** 编写，支持 **Windows** 和
**Linux** 双平台（x86_64 / ARM64）。软件提供了直观的图形界面，集成了串口参数配置、数据收发、指令管理、定时发送、校验码计算、脚本执行、日志保存等实用功能，适用于嵌入式开发、硬件调试、物联网设备测试等场景。

## 🖥️ 支持的平台

| 平台         | 架构    | 截图                                            |
|:-----------|:------|:----------------------------------------------|
| Windows 7  | 64 位  | ![windows_7.png](screenshots/windows_7.png)   |
| Windows 10 | 64 位  | ![windows_10.png](screenshots/windows_10.png) |
| Windows 11 | 64 位  | ![windows_11.png](screenshots/windows_11.png) |
| Linux      | AMD64 | ![linux_amd.png](screenshots/linux_amd.png)   |
| Linux      | ARM64 | ![linux_arm.png](screenshots/linux_arm.png)   |

## ✨ 功能特性

### 🔌 串口通信

- **自动检测**系统中所有可用串口，支持手动刷新
- 可配置的串口参数：
    - **端口号**：自动枚举可用串口
    - **波特率**：`9600` / `14400` / `19200` / `38400` / `56000` / `57600` / `115200` / `128000` / `230400`
    - **数据位**：`5` / `6` / `7` / `8`
    - **校验位**：`None` / `Even` / `Odd` / `Mark` / `Space`
    - **停止位**：`1` / `1.5` / `2`
    - **流控**：默认 `None`
- 串口开启后**自动锁定参数**，防止误操作；关闭后解锁
- 串口 I/O 运行在**独立线程**，UI 不阻塞

### 📨 数据收发

- 支持**文本模式**和 **HEX 十六进制模式**发送
- 接收数据支持**文本 / HEX 双模式**实时切换显示，无需重新收发
- 发送数据以**绿色**显示，接收数据以**默认颜色**显示，便于区分
- 每条消息附带**毫秒级时间戳**（格式：`[HH:mm:ss.zzz]`）
- 可选**追加回车换行**（`\r\n`）
- 实时统计**发送 / 接收字节数**（自动换算 B / KB / MB）
- 发送区与指令库联动，点击指令即可自动填入

### 🔐 校验码支持（HEX 发送模式）

| 校验类型      | 说明               |
|:----------|:-----------------|
| 无校验       | 不追加校验码           |
| CRC-8     | 多项式 0x07         |
| CRC-16(L) | MODBUS CRC，低字节在前 |
| CRC-16(H) | MODBUS CRC，高字节在前 |
| XOR       | 异或校验             |
| Checksum  | 求和取反             |

### 📋 指令管理

- 基于 **SQLite 数据库**持久化存储常用指令
- **预设 5 条默认指令**（上电、下电、电位查询、复位、状态检测）
- 支持指令的**添加、编辑、删除、复制**操作
- 每条指令包含**指令值（HEX 格式）**和**备注**，方便识别
- 指令去重检测，防止重复添加
- **右键上下文菜单**快捷操作（发送 / 复制 / 编辑 / 删除）
- 点击指令自动填入发送区

### 🕒 发送历史

- 自动记录每次发送的指令，去重并更新到列表顶部
- 点击历史项快速填入发送区

### ⏱️ 定时发送

- 支持按设定时间间隔**循环自动发送**当前指令
- 间隔时间以毫秒为单位，可灵活配置
- 一键启停，串口关闭时自动停止

### 📜 指令脚本执行

- 支持选择多条指令组成**执行脚本**
- 可**拖拽排序**调整指令执行顺序
- 统一配置指令间的**发送间隔**
- 按顺序依次执行，执行完毕后弹窗提示
- 执行期间保护，防止重复启动

### 📝 日志与数据管理

- 一键将通信历史记录**导出为 `.txt` 文件**
- 日志格式：`[时间戳]【方向】数据内容`
- 一键清空消息显示区和历史记录，重置收发字节计数

## 🏗️ 技术架构

| 技术栈   | 说明                           |
|:------|:-----------------------------|
| 开发语言  | C++11                        |
| UI 框架 | Qt 5（Widgets）                |
| 数据库   | SQLite（通过 Qt SQL 模块）         |
| 串口通信  | Qt SerialPort 模块             |
| 线程模型  | 串口 I/O 运行在独立 QThread，UI 线程安全 |
| 构建系统  | qmake                        |

### 📁 项目结构

```
SerialPortDebugger/
├── main.cpp                     # 程序入口，高 DPI 适配
├── DebuggerApplication.*        # 自定义 QApplication，加载字体、样式表
├── MainWindow.* / .ui           # 主窗口，核心 UI 交互逻辑
├── SerialPortManager.*          # 串口管理器（单例，独立线程）
├── SerialPortObserver.*         # 串口观察者（遗留/备用组件）
├── DatabaseWrapper.*            # 数据库封装（单例，SQLite CRUD）
├── TaskExecutor.*               # 脚本任务执行器（QTimer 异步顺序执行）
├── CommandItemWidget.*          # 自定义指令列表项组件
├── SaveCommandDialog.* / .ui    # 添加/编辑指令对话框
├── CommandScriptDialog.* / .ui  # 指令脚本配置对话框
├── Utils.*                      # 工具类（HEX 转换、校验码计算）
├── Logger.*                     # 日志系统（带 ANSI 颜色码）
├── GlobalDefinition.hpp         # 全局数据结构定义
├── style.qss                    # 全局 QSS 样式表
├── image.qrc                    # 图片资源
├── font.qrc                     # 字体资源（微软雅黑）
├── style.qrc                    # 样式资源
├── SerialPortDebugger.pro       # qmake 工程文件
├── kit/                         # 各平台发布打包工具集
│   ├── windows_x86_64/          # Windows Inno Setup 打包
│   ├── amd_x86_64/              # Linux AMD64 AppImage 打包
│   └── arm64/                   # Linux ARM64 AppImage 打包
└── screenshots/                 # 截图
```

### 🧩 架构设计

```
main.cpp
  └── DebuggerApplication (QApplication)
        ├── 加载字体 (msyh.ttc)
        ├── 加载样式 (style.qss)
        └── MainWindow (QMainWindow)
              ├── SerialPortManager (单例, 独立 QThread)
              │     └── SerialPortWorker (QObject, 实际串口操作)
              │           └── QSerialPort
              ├── DatabaseWrapper (单例)
              │     └── SQLite (commands.db)
              ├── TaskExecutor (基于 QTimer 的顺序任务执行)
              ├── CommandItemWidget (自定义列表项)
              ├── SaveCommandDialog (添加/编辑指令)
              └── CommandScriptDialog (脚本配置)
```

**设计模式**：

- **SerialPortManager**：单例 + Facade 模式，Worker 运行在独立 QThread，UI 通过 `QMetaObject::invokeMethod` 跨线程安全调用
- **DatabaseWrapper**：单例模式，封装 SQLite CRUD
- **TaskExecutor**：基于 QTimer 的异步顺序任务执行器
- **Utils / Logger**：纯静态工具类

## 🔧 构建说明

### 📦 依赖

- Qt 5.x（需包含 `widgets`、`serialport`、`sql` 模块）
- 支持 C++11 的编译器

### ⚙️ 编译

```bash
qmake SerialPortDebugger.pro
make          # Linux
nmake         # Windows (MSVC)
mingw32-make  # Windows (MinGW)
```

### 📦 打包发布

各平台的打包脚本位于 `kit/` 目录下：

| 平台             | 打包方式           | 脚本                                     |
|:---------------|:---------------|:---------------------------------------|
| Windows x86_64 | Inno Setup 安装包 | `kit/windows_x86_64/build_release.bat` |
| Linux AMD64    | AppImage       | `kit/amd_x86_64/build_appimage.sh`     |
| Linux ARM64    | AppImage       | `kit/arm64/build_appimage.sh`          |

## 📄 许可证

本项目基于 MIT 许可证开源，详见 [LICENSE](LICENSE)。
