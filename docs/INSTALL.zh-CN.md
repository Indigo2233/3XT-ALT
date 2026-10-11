# 3XT-ALT 安装说明

## 面向普通用户的发行包

此流程适用于带有有效 CPD 签名的 GitHub Release。

1. 下载 `XTAlt-<version>-windows-x64.zip` 和对应的
   `SHA256SUMS.txt`。
2. 校验压缩包的 SHA-256 值。
3. 将压缩包解压到长期保留的目录，例如：

   ```text
   C:\Users\<用户名>\AppData\Local\Programs\3XT-ALT\module
   ```

4. 关闭正在运行的 Real-Time Preview。
5. 在 PixInsight 中打开 `Process > Modules > Install Modules`。
6. 将 `Search Directory` 设置为解压目录，单击 `Search`。
7. 确认列表包含 `XTAlt-pxm.dll`，然后单击 `Install`。
8. 在 Process Explorer 的 `XTAlt` 分类中确认以下项目：

   - `BXTAlt`
   - `SXTAlt`
   - `NXTAlt`

## 后端目录

模块默认查找：

```text
%LOCALAPPDATA%\Programs\老佛爷的三件套
```

后端位于其他目录时，请在 Windows 用户环境变量中设置：

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

确认 `LFT_INSTALL_DIR` 指向包含以下文件的目录：

```text
runtime/python.exe
engine/run_models.py
tools/xt_alt_worker.py
```
