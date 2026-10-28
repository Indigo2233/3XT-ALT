# 3XT-ALT 安装代理指引

## 目标

在 Windows x64 和 PixInsight 1.9.x 上安装 GitHub Release 提供的
3XT-ALT Local Signing Edition，并验证以下原生 Process：

- `BXTAlt` / `BXT-ALT`
- `SXTAlt` / `SXT-ALT`
- `NXTAlt` / `NXT-ALT`

当前公开版本使用 PixInsight Local Signing Identity。每位用户需要使用自己的
PixInsight 许可证创建本地身份，并在自己的电脑上签名 DLL。
当前支持 Windows x64。macOS arm64/x86_64 模块和运行环境尚未发布；
不得将 ONNX 权重可跨平台使用表述为已支持 Mac 原生 Process。

## 安全要求

- 仅从 `https://github.com/Indigo2233/3XT-ALT/releases` 下载发行资产。
- 下载后必须依据 `SHA256SUMS-local.txt` 验证 ZIP 的 SHA-256。
- `.xssk` 是私钥文件。禁止读取、输出、上传、提交或复制其内容。
- 禁止向用户索取签名密码，禁止将密码写入命令行、脚本、日志或环境变量。
- PixInsight 显示密码窗口时，暂停自动化并让用户亲自输入密码。
- 禁止把 `.onnx`、`.xssk`、`.xsgn`、DLL、运行时或模型分析中间文件提交到 Git。
- 保留用户当前打开的图像、Preview、Process Icon 和参数。

## 安装前检查

1. 确认系统为 Windows x64，并已安装 PixInsight 1.9.x；官方 SigningKeys 若要求更新版本，先引导用户升级。
2. 确认发行包包含：

   ```text
   XTAlt-pxm.dll
   XTAltBackend/runtime/python.exe
   XTAltBackend/engine/run_models.py
   XTAltBackend/engine/models/InDeConv.onnx
   XTAltBackend/engine/models/InDeNoise.onnx
   XTAltBackend/engine/models/InDeStars.onnx
   XTAltBackend/tools/xt_alt_worker.py
   install-local.ps1
   Install-Windows.cmd
   setup-windows.ps1
   installer-common.ps1
   package-manifest.json
   test/native-selftest.js
   ```

3. 检查 PixInsight Local Signing Identity：

   - 首次使用运行 `Script > Development > SigningKeys`。勾选 `Generate Signing Keys`
     和 `Local signing identity`，在 `Output` 指定 `.xssk` 和密码，点击 `Run`。
   - 密码由用户在官方界面设置，至少 8 位，含大写、小写及数字或标点，仅 ASCII，无空格。
   - 将生成的 `.xssk` 保存在私有、仅限当前用户访问的位置。
   - 在 `Edit > Local Signing Identity` 中选择文件、由用户输入密码，勾选
     `Make the local signing identity persistent` 后确认。优先复用已有身份。
   - `.xssk` 的完整路径必须只包含 ASCII 字符，以兼容 PixInsight Windows
     命令行签名接口。

## 安装流程

推荐用户下载 `XTAlt-1.0.1-setup-windows.zip`、全部解压并双击 `Install-Windows.cmd`。
向导自动下载校验完整包、选择路径并调用安装器。完整离线包也包含同一个入口。
逐项界面说明见 `docs/INSTALL.zh-CN.md`。本地身份创建、注册、密码输入和模块扫描
仍需要用户操作；不得声称完全无人值守。

代理可先执行 `setup-windows.ps1 -CheckOnly -PackageDirectory <完整包目录>` 进行只读校验。
向导只持久保存 PixInsight 与密钥的路径，位置为 `%LOCALAPPDATA%\XTAltInstaller\preferences.json`。
使用命令行安装时：

1. 完全退出 PixInsight。
2. 在发行包目录打开 PowerShell。
3. 运行：

   ```powershell
   .\install-local.ps1 -KeysFile "C:\path\PixInsight-local-signing.xssk"
   ```

4. 出现 PixInsight 密码窗口后暂停，让用户输入密码并点击 `OK`。
5. 安装脚本完成后确认以下文件存在：

   ```text
   %LOCALAPPDATA%\Programs\XTAlt\XTAlt-pxm.dll
   %LOCALAPPDATA%\Programs\XTAlt\XTAlt-pxm.xsgn
   %LOCALAPPDATA%\Programs\XTAlt\XTAltBackend\...
   ```

6. 启动 PixInsight，打开 `Process > Modules > Install Modules`，选择：

   ```text
   %LOCALAPPDATA%\Programs\XTAlt
   ```

7. 完成模块扫描后重新启动 PixInsight。

安装脚本支持自定义路径：

```powershell
.\install-local.ps1 `
  -KeysFile "C:\path\PixInsight-local-signing.xssk" `
  -PixInsightRoot "D:\PixInsight" `
  -Destination "D:\PixInsightModules\XTAlt"
```

## 验证

1. 确认 `Process > XTAlt` 中存在三个 Process。
2. 执行 `Script > Execute Script File`，选择安装目录中的：

   ```text
   test\native-selftest.js
   ```

3. 读取 `%TEMP%\xtalt-native-selftest.json`，要求：

   ```json
   {"ok": true}
   ```

4. 使用一张非关键测试图像验证 BXT-ALT Real-Time Preview：

   - 原图可以立即显示；
   - 推理完成后 `Show transformed image` 可用；
   - 修改参数后预览可以更新；
   - 控制台没有 UTF-8 解码错误；
   - 第二次更新能够复用 ONNX Session。

## 故障处理

- `No file(s) found` 且路径乱码：确认 `.xssk` 路径只包含 ASCII 字符。
- 缺少 `.xsgn`：签名未完成，重新关闭 PixInsight并运行安装脚本。
- 模块签名无效：DLL 更新后需要重新运行本地签名。
- 找不到后端：确认 `XTAltBackend` 与 DLL 位于同一安装目录。
- `Show transformed image` 不可用：检查控制台错误，并确认加载的 DLL 来自当前安装目录。

## 发布维护

- Local Signing Edition 使用 `package-local-release.ps1` 打包。
- 公开打包必须显式传入 `-ModelDistributionAuthorized`。
- 使用 `publish-local-release.ps1 -ConfirmPublicRelease` 发布 GitHub Release。
- 同时发布完整 ZIP、小型 setup ZIP 和列出两个 ZIP 的 `SHA256SUMS-local.txt`。
- 完整包包含 `package-manifest.json`；安装前验证逐文件 SHA-256。
- 安装器变更运行 `test/installer-smoke.ps1` 和实际包的 `-CheckOnly`，兼容 Windows PowerShell 5.1。
- Release 打包会使用 `tools/sanitize-release-model.py` 生成去标识化模型副本，保留输入输出接口和权重数值；打包机需要可导入 `onnx` 的 Python，可执行 `python -m pip install onnx` 准备环境。
- 取得 CPD 身份后，使用 `package-release.ps1` 和 `publish-release.ps1`
  发布 Manage Repositories 版本。
