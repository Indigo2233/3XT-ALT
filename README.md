# 3XT-ALT

3XT-ALT 为 PixInsight 提供三个独立的原生 Process：

- `BXTAlt`，界面名称为 `BXT-ALT`
- `SXTAlt`，界面名称为 `SXT-ALT`
- `NXTAlt`，界面名称为 `NXT-ALT`

三个 Process 均支持 Process Icon、View/Preview 直接执行和 PixInsight 原生
Real-Time Preview。图像数据通过内存管道与本地 Python/ONNX 后端交换，处理过程
不创建临时图像文件。

## 当前发布状态

本仓库公开 PCL 源码、构建脚本和测试脚本。公开二进制发行版需要有效的
Certified PixInsight Developer（CPD）代码签名。开发者本机生成的 Local Signing
Identity 仅适合本地开发和测试。

仓库和 Release 均不包含 ONNX 模型、解密计算图、应用运行时、`.xssk` 私钥或
本地 `.xsgn` 签名。详细范围参见 [NOTICE.md](NOTICE.md)。

## 用户安装

取得 CPD 签名并发布 GitHub Release 后，用户安装流程如下：

1. 从 [Releases](https://github.com/Indigo2233/3XT-ALT/releases) 下载
   `XTAlt-<version>-windows-x64.zip`。
2. 将压缩包解压到长期保留的目录。
3. 在 PixInsight 中打开 `Process > Modules > Install Modules`。
4. 选择解压目录，确认 `XTAlt-pxm.dll`，然后执行安装。
5. 在 `Process > XTAlt` 中打开三个 Process。

后端安装在自定义目录时，请在启动 PixInsight 前设置环境变量
`LFT_INSTALL_DIR`。默认后端目录为：

```text
%LOCALAPPDATA%\Programs\老佛爷的三件套
```

完整说明参见 [docs/INSTALL.zh-CN.md](docs/INSTALL.zh-CN.md)。

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

取得 CPD 签名后执行：

```powershell
.\package-release.ps1 -Version 1.0.0 -SignaturePath "C:\path\XTAlt-pxm.xsgn" -CpdSigned
```

发行包和 SHA-256 校验文件将生成到 `release` 目录。

## 后端依赖

模块会按以下顺序定位兼容后端：

1. `LFT_INSTALL_DIR`
2. 模块文件的上级目录
3. `%LOCALAPPDATA%\Programs\老佛爷的三件套`

后端目录需要包含：

```text
runtime/python.exe
engine/run_models.py
tools/xt_alt_worker.py
```

模型和运行时由用户通过具有相应授权的渠道自行取得。
