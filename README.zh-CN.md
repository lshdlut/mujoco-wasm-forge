# mujoco-wasm-forge

English | 中文说明

mujoco-wasm-forge 是一套可复现的构建流程，用来把 MuJoCo 发布版打包成 WebAssembly 产物。  
流程会扫描上游头文件、生成包装与导出列表、同时构建 WASM 与原生对照程序、执行 smoke/regression 检查，并输出带元数据的版本化产物。GitHub Actions 与本地脚本共享同一条流水线。

- **输入**：MuJoCo 标签（当前覆盖 3.2.5、3.3.7、3.3.8-alpha）  
- **输出**：`dist/<version>/{mujoco.js, mujoco.wasm[, mujoco.wasm.map], version.json, sbom.spdx.json}`  
- **工具链**：emsdk 4.0.10（含 Node 20），与 CI 保持一致  
- **范围**：仅包含仿真内核；可视化 / UI 系列刻意排除

### 官方 WASM（official Embind）

Google DeepMind 维护的官方 MuJoCo WebAssembly 绑定使用 Embind（见 [issue #2585](https://github.com/google-deepmind/mujoco/issues/2585#issuecomment-3473495118) 以及 [commit 40862617](https://github.com/google-deepmind/mujoco/commit/4086261714d7cfbc1745d4c6cb0aa2116df45312)）。  
本仓库在命名方式与暴露策略上对齐官方 Embind：尽量直接暴露公开 API、优先提供结构体字段的指针视图，同时仍然输出多版本构建。

仓库镜像：https://github.com/lshdlut/mujoco-wasm-forge

## 暴露策略（ABI 摘要）

- 与官方 Embind 对齐：尽可能完整暴露 MuJoCo C API，并为 `mjModel` / `mjData` 等结构体提供指针视图，方便 JavaScript 侧读写。
- 仍然排除可视化 / UI 家族（`mjv` / `mjr` / `mjui`）；聚焦仿真内核。
- 变参函数优先导出 `*_v` 版本；缺少 `_v` 实现的变参函数暂不发布。
- 详细导出报告位于 `dist/<ver>/abi/`。

## 产物

CI 与本地标准构建都会生成：

- `dist/<mjVer>/mujoco.wasm` —— WebAssembly 二进制
- `dist/<mjVer>/mujoco.js` —— ES Module 工厂 (`createMuJoCo`)
- `dist/<mjVer>/mujoco.wasm.map` —— 可选 source map
- `dist/<mjVer>/version.json` —— 元数据（MuJoCo 版本、emsdk、构建哈希、尺寸等）
- `dist/<mjVer>/sbom.spdx.json` —— SPDX SBOM

## 快速上手（Node ESM）

用法与英文版示例一致，可直接引入 `dist/<version>/mujoco.js` 并通过 `Module.cwrap` 访问导出的函数/指针。

## CI 与可重复性

唯一工作流：`.github/workflows/forge.yml`

- 构建矩阵覆盖 3.2.5 / 3.3.7 / 3.3.8-alpha
- 工具链固定为 emsdk 4.0.10 + Node 20
- 3.3.7 与 3.3.8-alpha 使用两阶段 `emcmake` 并强制 qhull 静态链接（Emscripten 限制）
- 校验项：类型声明再生成（DTS）以及运行时 smoke / regression / mesh 测试（RUN）
- 构建产物直接从 `dist/<mjVer>/` 上传

### ABI 驱动流水线（每个版本，基于 introspect）

当前 ABI 流程完全由 MuJoCo 官方 introspect 表驱动：

1. `python scripts/mujoco_abi/scan_clang_introspect.py` —— 调用官方 codegen，得到 FUNCTIONS / STRUCTS / ENUMS 的 JSON（写入 `dist/<ver>/abi`）。
2. `python scripts/mujoco_abi/build_mjapi_from_introspect.py` —— 生成 `mjapi.json`（A 集合函数声明）。
3. `node scripts/mujoco_abi/nm_coverage.mjs build/<short>/lib/libmujoco.a --out dist/<ver>/abi/nm_symbols.json` —— 收集实现符号（B 集合）。
4. `python wrappers/official_app_337/codegen/gen_structs.py` —— 生成结构体相关导出 `mjwf_exports_generated.{h,c}` 与 `mjwf_extra_exports.lst`。
5. `python wrappers/official_app_337/codegen/gen_funcs.py` —— 生成 `_mjwf_*` 函数包装与 `wrapper_exports_funcs.json` / `exports_report_funcs.md`。
6. `python scripts/mujoco_abi/gen_enums_from_introspect.py` —— 将枚举压平成 `enums.json` 供 JS/TS 使用。

Emscripten 再通过 `-sEXPORTED_FUNCTIONS=@...` 消费生成的导出列表（`wrapper_exports*.json` / `exports_*.lst`）。更多细节见 `docs/ABI_SCAN.md`。

## 本地构建（推荐流程）

建议在 WSL Ubuntu 22.04（或 Docker）中执行，步骤与 CI 一致。

1. **同步并构建（Windows 侧）**
   ```powershell
   pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass `
     -File local_tools/wsl/run.ps1 -Sync -Clean -Meta -PinNode20 -UseTemp -Jobs 6
   ```
   默认会同时构建 3.2.5 / 3.3.7 / 3.3.8-alpha；如果只想迭代部分版本，可用 `-Targets '337'` 等参数缩小范围。
   已同步的工作区可根据需要去掉 `-Sync` / `-UseTemp`。

2. **生成 ABI 描述（post_build 前，新流程）**
   ```powershell
   python scripts/mujoco_abi/scan_clang_introspect.py --header external/mujoco/include/mujoco/mujoco.h --out-dir dist/3.3.7/abi
   python scripts/mujoco_abi/build_mjapi_from_introspect.py --out dist/3.3.7/abi/mjapi.json
   python scripts/mujoco_abi/gen_enums_from_introspect.py
   ```

3. **在 WSL 内执行 post_build**
   ```bash
   source /root/emsdk/emsdk_env.sh >/dev/null 2>&1
   ./scripts/ci/post_build.sh --version 3.2.5 --short 325
   ./scripts/ci/post_build.sh --version 3.3.7 --short 337
   ./scripts/ci/post_build.sh --version 3.3.8-alpha --short 338
   ```

提示：

- 在 WSL ext4 目录（例如 `~/dev/mujoco-wasm-forge`）或 `/tmp`（`-UseTemp`）下构建，避免 `/mnt/c/...` 与 OneDrive 带来的 I/O 问题。
- 建议从干净工作区（`-UseTemp` 或全新 clone）开始，以确保与 CI 一致。
- 默认并行度为 6，可通过 `-Jobs` 重写。
- `-Sync` 会自动排除 `.git` 并清理异常目录，更适合同步 Windows 工作副本。

## 在其他项目中使用

- 构建后仅需拷贝 `dist/<mjVer>/`；无需携带 `build/` 或 `external/`。
- 直接加载 `dist/<mjVer>/mujoco.{js,wasm}`；3.3.7 已包含静态 qhull。
- 若启用 `-Meta`/`META=1`，会额外生成版本信息与 SBOM。
### ����ʱ�������ο�

- ����ڶ��δ���ڵĵ������ݣ���ʹ�� `stackSave + stackAlloc + stackRestore` ����ջ��ռ䡣
- ��Ҫ�ֲ�ͬ����ʹ�õ� scratch (���� `mj_contactForce` �� 6×`mjtNum` ����) ���� `_malloc` ��ȡ���ڴ棬Ȼ��ͨ�� `_free` �ͷ�

���� `mjtNum` �� `double`���Ա� 8 �ֽڣ�

```ts
const scratchBytes = 6 * 8;
const scratchPtr = Module._malloc(scratchBytes);
Module._mjwf_mj_contactForce(modelPtr, dataPtr, contactIndex, scratchPtr);
const forces = Module.HEAPF64.subarray(scratchPtr / 8, scratchPtr / 8 + 6);
// ... ʹ�� forces ...
Module._free(scratchPtr);
```

Forge ģ���Ѿ��� `_malloc/_free/_realloc` ��ջ API �������嵥�����嶼���� Embind �����ĳ����÷�ģʽ��


## 版本约定

- 稳定标签：`forge-<mujocoVersion>-r<rev>`（如 `forge-3.3.7-r2`）
- 预发行：`forge-<mujocoVersion>-rc.<n>`
- 产物不可变；若需修正，增量 bump `-rN`

## 其他说明

- 前端示例（进行中）：https://github.com/lshdlut/mujoco-wasm-play

## 致谢

项目受到多份早期 MuJoCo→WASM 实验的启发，它们验证了可行性并总结了大量坑点：

- [stillonearth/MuJoCo-WASM](https://github.com/stillonearth/MuJoCo-WASM)
- [zalo/mujoco_wasm](https://github.com/zalo/mujoco_wasm)
- [hashb/mujoco_web](https://github.com/hashb/mujoco_web)

虽然 mujoco-wasm-forge 已经演化为独立工具链，仍对这些先行者心怀感谢。

## Provenance

仓库部分脚本和文档在生成阶段使用了生成式 AI，最终内容均由维护者审校。

