# mujoco-wasm-forge

English | 中文说明

mujoco-wasm-forge 提供一套可复现的构建流程，将 MuJoCo 官方发布版本转换为 WebAssembly 产物。  
流水线会扫描上游头文件、推导包装导出、分别编译 WASM 与对比用原生二进制、执行冒烟/回归检测，并生成带元数据的版本化产物。GitHub Actions 与本地流程保持一致。

- **说明**：MuJoCo 标签当前涵盖 3.2.5、3.3.7 及 3.3.8-alpha。
- **输出**：`dist/<version>/{mujoco.js, mujoco.wasm[, mujoco.wasm.map], version.json, sbom.spdx.json}`  
- **工具链**：emsdk 4.0.10、Node 20（与 CI 相同）  
- **范围**：仅包含仿真核心，视觉/UI 系符号会被排除

### 官方 WASM 预览

Google DeepMind 已经软发布了 MuJoCo 的官方 WebAssembly 绑定（参见 [issue #2585 评论](https://github.com/google-deepmind/mujoco/issues/2585#issuecomment-3473495118) 和 [commit 40862617](https://github.com/google-deepmind/mujoco/commit/4086261714d7cfbc1745d4c6cb0aa2116df45312)）。该构建依赖 Embind 并提供 MuJoCo 3.3.8。我们正在评估差异（API 覆盖、运行时体积、工具链），后续会同步调查结果；当前仓库继续提供自动化的 `_mjwf_*` 流水线与多版本构建。

仓库镜像：https://github.com/lshdlut/mujoco-wasm-forge

## 导出规则（ABI 摘要）

- 等式：**C = A ∩ B**  
  - A → `mujoco.h`（以及存在时的 `mjspec.h`）中的公共 C API 声明  
  - B → `libmujoco.a` 中通过 `llvm-nm` 枚举出的实际实现符号
- 硬闸：`(A ∩ B) − C = 0`；仅允许 `mj_`、`mju_`、`mjs_`、`mjd_` 前缀进入导出集合
- 特殊排除  
  1. 排除视觉/UI/插件族 (`mjv_`, `mjr_`, `mjui_`, `mjp_`, `mjc_`)  
  2. 变参函数必须存在对应的 `*_v` 实现；否则记录为 `variadic_no_v`
- 完整报表位于 `dist/<ver>/abi/exports_report.md`（需要 JSON 时，可设置 `EMIT_JSON=1`）

## 产物一览

本地与 CI 都会生成：

- `dist/<mjVer>/mujoco.wasm` — WebAssembly 二进制
- `dist/<mjVer>/mujoco.js` — ES 模块工厂 (`createMuJoCo`)
- `dist/<mjVer>/mujoco.wasm.map` — 可选 source map
- `dist/<mjVer>/version.json` — 元信息（MuJoCo 标签、emsdk、大小、sha256、git sha 等）
- `dist/<mjVer>/sbom.spdx.json` — 轻量级 SPDX SBOM

## 快速上手（Node ESM）

同英文版示例，可直接导入 `dist/<version>/mujoco.js` 并通过 `Module.cwrap` 调用包装函数。

## CI 与可复现性

唯一入口：`.github/workflows/forge.yml`。

- 矩阵覆盖 3.2.5 / 3.3.7 / 3.3.8-alpha
- 工具链固定为 emsdk 4.0.10 + Node 20
- 3.3.7 及 3.3.8-alpha 会执行两阶段配置并强制 qhull 静态链接（Emscripten 限制）
- 质量闸：`[GATE:SYM]`、`[GATE:DTS]`、`[GATE:RUN]`（尚未实现的闸会标记为 skipped）
- `dist/<mjVer>/` 中的产物直接上传

### ABI 驱动流水线（每次构建）

1. `node scripts/mujoco_abi/autogen_wrappers.mjs` → 扫描 C API（A 集合）  
2. `node scripts/mujoco_abi/nm_coverage.mjs build/<short>/lib/libmujoco.a` → 枚举实现符号（B 集合）  
3. `node scripts/mujoco_abi/gen_exports_from_abi.mjs` → 生成包装、导出白名单、TypeScript 声明与报表  
4. CMake 读取 `exports_<ver>.lst`，在编译阶段指定 `-sEXPORTED_FUNCTIONS=@...`  
5. `node scripts/mujoco_abi/check_exports.mjs ...` → 校验 `(A ∩ B) − C = 0` 且无黑名单前缀泄漏  
6. `node scripts/mujoco_abi/nm_coverage.mjs ... --out dist/<ver>/abi/nm_coverage.json` → 记录实现覆盖率

详情参阅 `docs/ABI_SCAN.md`。

## 本地构建流程（推荐）

建议使用 WSL Ubuntu 22.04（或 Docker），执行顺序与 CI 保持一致。

1. **镜像并构建（Windows 主机）：**
   ```powershell
   pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass `
     -File local_tools/wsl/run.ps1 -Sync -Clean -Meta -PinNode20 -UseTemp -Jobs 6
   ```
   增量构建可去掉 `-Sync`/`-UseTemp`。

2. **生成 ABI 描述（post_build 前置步骤）：**
  ```powershell
  pwsh scripts/mujoco_abi/run.ps1 -Repo external/mujoco -Ref 3.2.5 -OutDir dist/3.2.5/abi
  pwsh scripts/mujoco_abi/run.ps1 -Repo external/mujoco -Ref 3.3.7 -OutDir dist/3.3.7/abi
  pwsh scripts/mujoco_abi/run.ps1 -Repo external/mujoco -Ref 3.3.8-alpha -OutDir dist/3.3.8-alpha/abi
  ```

3. **在 WSL 内执行 post_build 检查：**
   ```bash
   source /root/emsdk/emsdk_env.sh >/dev/null 2>&1
   ./scripts/ci/post_build.sh --version 3.2.5 --short 325
   ./scripts/ci/post_build.sh --version 3.3.7 --short 337
   ```

注意事项：

- 请在 WSL ext4（如 `~/dev/mujoco-wasm-forge`）或 `/tmp` (`-UseTemp`) 下构建，避免 `/mnt/c/...` 与 OneDrive 带来的 I/O 问题。
- 为保证与 CI 完全一致，推荐从干净目录开始（`-UseTemp` 或重新克隆）。
- 默认并行度为 6，可通过 `-Jobs` 覆盖。
- `-Sync` 会自动排除 `.git` 等目录，并清理 `?root?...` 异常路径，优先使用该方式同步。

## 在其他项目中使用产物

- 构建完成后，仅复制 `dist/<mjVer>/` 即可；不要拷贝 `build/` 或 `external/`。  
- 直接加载 `dist/<mjVer>/mujoco.{js,wasm}`；3.3.7 已包含强制静态 qhull 的结果。  
- 开启 `-Meta` / `META=1` 时，会额外生成元数据和 SBOM。

## 版本与发布

- 稳定标签：`forge-<mujocoVersion>-r<rev>`（例如 `forge-3.3.7-r2`）  
- 预发布：`forge-<mujocoVersion>-rc.<n>`  
- 产物不可变，修复需递增 `-rN` 再发布。

## 注意事项

- 前端示例（开发中）：https://github.com/lshdlut/mujoco-wasm-play

## 致谢

本项目灵感来源于更早的 MuJoCo → WASM 实验，它们验证了整体可行性并记录了关键注意事项：

- [stillonearth/MuJoCo-WASM](https://github.com/stillonearth/MuJoCo-WASM)
- [zalo/mujoco_wasm](https://github.com/zalo/mujoco_wasm)
- [hashb/mujoco_web](https://github.com/hashb/mujoco_web)

虽然如今已经演化为独立工具链，mujoco-wasm-forge 仍然感谢这些项目的开创性探索。

## Provenance

仓库部分脚本和文档由生成式 AI 协助撰写，随后由维护者审核确认。


