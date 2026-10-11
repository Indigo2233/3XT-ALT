# 3XT-ALT 安装说明

## 通过 Manage Repositories 安装

此流程适用于带有有效 CPD 签名的正式发行版。

1. 在 PixInsight 中打开 `Resources > Updates > Manage Repositories`。
2. 添加以下 URL，保留末尾的 `/`：

   ```text
   https://raw.githubusercontent.com/Indigo2233/3XT-ALT/main/
   ```

3. 执行 `Resources > Updates > Check for Updates`。
4. 选择 `3XT-ALT`，完成下载后退出 PixInsight。
5. 更新程序会安装模块和完整后端，并自动重新启动 PixInsight。
6. 在 Process Explorer 的 `XTAlt` 分类中确认以下项目：

   - `BXTAlt`
   - `SXTAlt`
   - `NXTAlt`

## 后端目录

Manage Repositories 更新包会将完整后端安装到：

```text
<PixInsight 安装目录>\bin\XTAltBackend
```

模块也兼容现有桌面版后端，默认查找：

```text
%LOCALAPPDATA%\Programs\老佛爷的三件套
```

后端位于其他目录时，可以在 Windows 用户环境变量中设置：

```text
LFT_INSTALL_DIR=<后端安装目录>
```

重新启动 PixInsight 后生效。

## 验证

在 PixInsight 中执行：

```text
Script > Execute Script File
```

选择仓库中的 `test/native-selftest.js`。脚本会验证三个 Process 的注册和
基本执行链路。测试需要可用的本地后端。

随后分别检查：

- Process Icon 创建与复用
- View 和 Preview 直接执行
- 原生 Real-Time Preview
- `Correct Only` 等参数联动
- SXT 星点图输出

## 常见问题

### 模块未出现在搜索结果中

确认 DLL 与 `.xsgn` 文件位于同一目录，并确认签名与 DLL 的版本完全匹配。

### 签名验证失败

公开发行包需要 CPD 签名。Local Signing Identity 生成的签名用于签名者本地
开发环境。

### 无法定位后端

确认 `LFT_INSTALL_DIR` 或 `bin\XTAltBackend` 指向包含以下文件的目录：

```text
runtime/python.exe
engine/run_models.py
engine/models/InDeConv.onnx
engine/models/InDeNoise.onnx
engine/models/InDeStars.onnx
tools/xt_alt_worker.py
```
