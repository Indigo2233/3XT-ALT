# 3XT-ALT

3XT-ALT 为 PixInsight 提供三个独立的原生 Process：

- `BXTAlt`，界面名称为 `BXT-ALT`
- `SXTAlt`，界面名称为 `SXT-ALT`
- `NXTAlt`，界面名称为 `NXT-ALT`

三个 Process 均支持 Process Icon、View/Preview 直接执行和 PixInsight 原生
Real-Time Preview。图像数据通过内存管道与本地 Python/ONNX 后端交换，处理过程
不创建临时图像文件。

## 当前发布状态

本仓库公开 PCL 源码、构建脚本、测试脚本和 PixInsight 更新源描述文件。公开二进制
发行版需要有效的 Certified PixInsight Developer（CPD）代码签名。开发者本机生成的
Local Signing Identity 仅适合本地开发和测试。

Git 历史不保存 ONNX 模型、应用运行时、`.xssk` 私钥、`.xsgn` 签名或生成的二进制。
获得相应分发授权后，完整后端由发布脚本打包为 GitHub Release 资产。详细范围参见
[NOTICE.md](NOTICE.md)。

## 用户安装

取得 CPD 签名并发布 GitHub Release 后，用户可直接通过 PixInsight 更新系统安装：

1. 打开 `Resources > Updates > Manage Repositories`。
2. 添加以下仓库 URL，保留末尾的 `/`：

   ```text
   https://raw.githubusercontent.com/Indigo2233/3XT-ALT/main/
   ```

3. 执行 `Resources > Updates > Check for Updates`，选择 `3XT-ALT` 更新。
4. 下载完成后退出 PixInsight，由更新程序安装并自动重新启动。
5. 在 `Process > XTAlt` 中打开 `BXTAlt`、`SXTAlt` 或 `NXTAlt`。

更新包会将模块安装到 PixInsight 的 `bin` 目录，并将自包含后端安装到
`bin/XTAltBackend`。无需额外安装 Python 或手工复制模型。

开发或兼容旧版安装时，也可以在启动 PixInsight 前设置环境变量
`LFT_INSTALL_DIR`。旧版默认后端目录为：

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
