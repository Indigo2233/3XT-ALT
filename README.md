# 3XT-ALT

3XT-ALT 为 PixInsight 提供三个独立的原生 Process：

- `BXTAlt`，界面名称为 `BXT-ALT`
- `SXTAlt`，界面名称为 `SXT-ALT`
- `NXTAlt`，界面名称为 `NXT-ALT`

三个 Process 均支持 Process Icon、View/Preview 直接执行和 PixInsight 原生
Real-Time Preview。图像数据通过内存管道与本地 Python/ONNX 后端交换，处理过程
不创建临时图像文件。

## 当前发布状态

本仓库公开 PCL 源码、构建脚本、测试脚本和安装文档。项目自研的三个 ONNX 模型与
精简运行时通过 GitHub Release 资产分发，避免将大型二进制写入 Git 历史。

当前公开版本为 Local Signing Edition。每位用户使用自己的 PixInsight Local Signing
Identity 在本机签名 DLL，即可安装完整的三个 Process 和 Real-Time Preview。
标准 Manage Repositories 一键安装仍需要 Certified PixInsight Developer（CPD）签名。
详细范围参见 [NOTICE.md](NOTICE.md)。

**当前安装包支持 Windows x64。macOS Apple Silicon / Intel 安装包尚未构建和验证。**
ONNX 模型文件可以共用，Mac 所需的原生模块与运行环境需要分别适配。
架构对应关系见[安装说明](docs/INSTALL.zh-CN.md#macosapple-silicon-与-intel)。

桌面版附带的 Siril 脚本使用模型名称命名：`InDeConv.py`、`InDeStars.py` 和
`InDeNoise.py`，分别对应反卷积、去星和降噪功能。

## 用户安装

### Local Signing Edition

1. 下载[小型 Windows 安装入口](https://github.com/Indigo2233/3XT-ALT/releases/download/v1.0.1-local/XTAlt-1.0.1-setup-windows.zip)。
2. 全部解压，双击 `Install-Windows.cmd`。
3. 按向导选择 PixInsight 和自己的 `.xssk` 文件。首次使用会引导生成并注册本地身份。
4. 保存图像并退出 PixInsight；向导自动下载、校验并安装完整包，在官方窗口输入签名密码。
5. 在 `Process > Modules > Install Modules` 中选择向导显示的安装目录，点击 `Search` 和 `Install`，然后重启。

无需另装 Python、配置模型路径或编译源码。三个 Process 共用一个模块，安装一次即可。
下载、校验、路径选择和文件安装已自动化；首次密钥生成、身份注册、密码输入和模块扫描由用户完成。

**还没有 `.xssk`？** 打开 `Script > Development > SigningKeys`，勾选
`Generate Signing Keys` 和 `Local signing identity`，在 `Output` 选择 `local.xssk`
保存位置并设置密码，点击 `Run`。然后通过 `Edit > Local Signing Identity` 注册该文件，
勾选 `Make the local signing identity persistent`。
完整的逐项填写说明、密码要求、路径示例见[安装说明](docs/INSTALL.zh-CN.md#首次生成-xssk按界面逐项填写)。

离线使用：[下载完整包](https://github.com/Indigo2233/3XT-ALT/releases/download/v1.0.1-local/XTAlt-1.0.1-local-windows-x64.zip)，
依据 [SHA256SUMS-local.txt](https://github.com/Indigo2233/3XT-ALT/releases/download/v1.0.1-local/SHA256SUMS-local.txt)
校验后全部解压，双击相同入口。完整包内的向导使用本地文件。

适合交给 Codex 或其他本地编程代理执行的完整规范位于 [AGENTS.md](AGENTS.md)，
人工安装与验证步骤位于 [docs/INSTALL.zh-CN.md](docs/INSTALL.zh-CN.md)。

### Manage Repositories

当前尚未启用。取得有效 CPD 签名并发布签名仓库描述文件后，可以通过 PixInsight 更新系统
分发各平台模块与后端。当前用户请使用上方 Local Signing Edition 安装向导。

开发或兼容旧版安装时，也可以在启动 PixInsight 前设置环境变量
`LFT_INSTALL_DIR`。旧版默认后端目录为：

```text
%LOCALAPPDATA%\Programs\老佛爷的三件套
```

## 源码构建

Windows x64 构建要求：

- PixInsight 1.9.x，安装目录包含 `include` 和 `src`
- Visual Studio 2022 C++ 工具链
- PowerShell 5.1 或更高版本

执行：

```powershell
.\build.ps1
```

脚本会自动查找标准 PixInsight 与 Visual Studio 安装目录。自定义安装可使用：

```powershell
.\build.ps1 -PixInsightRoot "D:\PixInsight" -MSBuildPath "D:\Visual Studio\MSBuild\Current\Bin\MSBuild.exe"
```

输出文件为 `build\bin\XTAlt-pxm.dll`。

## 本地开发签名

1. 在 PixInsight 中运行标准 `SigningKeys` 脚本，创建 Local Signing Identity。
2. 在 `Edit > Local Signing Identity` 中注册该身份。
3. 运行标准 `CodeSign` 脚本，对 `build\bin\XTAlt-pxm.dll` 签名。
4. 执行 `.\install.ps1`。
5. 在 PixInsight 的 `Process > Modules > Install Modules` 中选择脚本输出目录。

`.xssk` 文件包含私钥，应始终保存在仓库之外。

## 生成公共发行包

### Local Signing Edition

确认模型公开分发授权后执行：

```powershell
.\package-local-release.ps1 -Version 1.0.1 -ModelDistributionAuthorized
.\publish-local-release.ps1 -Version 1.0.1 -ConfirmPublicRelease
```

发布包不包含 `.xsgn`，每位用户在本机运行 `install-local.ps1` 生成自己的签名。
打包机需要可导入 `onnx` 的 Python，可用 `-OnnxPythonPath` 指定；发布模型由去标识化工具生成副本。
打包同时生成小型安装入口、完整离线包、完整包内的逐文件哈希清单，以及两个 ZIP 的 SHA-256。
发布脚本会更新这三个 Release 资产。

安装器维护时运行 `test/installer-smoke.ps1`；用 Windows PowerShell 5.1 执行
`setup-windows.ps1 -CheckOnly -PackageDirectory <完整包目录>` 验证实际发行包。

### CPD / Manage Repositories

使用 CPD 身份签名 DLL，并确认模型具备公开分发授权后执行：

```powershell
.\package-release.ps1 `
  -Version 1.0.1 `
  -SignaturePath "C:\path\XTAlt-pxm.xsgn" `
  -CpdSigned `
  -ModelDistributionAuthorized
```

脚本会生成完整 Windows x64 更新包、SHA-256 校验文件和未签名的 `updates.xri`，
并在打包前使用精简运行时加载三个 ONNX 模型进行验证。随后使用 PixInsight
`CodeSign` 对 `updates.xri` 进行 XML 签名，提交并推送该文件，再发布 Release：

```powershell
.\publish-release.ps1 -Version 1.0.1 -ConfirmPublicRelease
```

发行资产的下载地址写入 `updates.xri` 的 `serverURL`，模型文件不会进入 Git 历史。

## 后端依赖

模块会按以下顺序定位兼容后端：

1. `LFT_INSTALL_DIR`
2. 模块同目录下的 `XTAltBackend`
3. 模块文件的上级目录
4. `%LOCALAPPDATA%\Programs\老佛爷的三件套`

后端目录需要包含：

```text
runtime/python.exe
engine/run_models.py
engine/models/InDeConv.onnx
engine/models/InDeNoise.onnx
engine/models/InDeStars.onnx
tools/xt_alt_worker.py
```

模型和运行时由用户通过具有相应授权的渠道自行取得。
