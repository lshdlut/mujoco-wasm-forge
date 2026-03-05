# 构建流水线（端到端）

权威入口是：

```bash
python3 forge_cli.py build --version <mjver>
```

它会为指定版本/ref 依次执行：

1. **准备上游源码**
   - 在 `external/mujoco` 下 clone/update 上游 MuJoCo。
   - 检出指定 ref（如 `3.5.0` 或 commit hash）。
   - 应用 Emscripten 相关的构建补丁（例如 qhull）。
2. **头文件 introspect**
   - 解析 `mujoco.h`，在 `dist/<ver>/abi/` 下生成权威 JSON：
     - `mujoco_ast.json`
     - `functions_introspect_like.json`
     - `structs_introspect_like.json`
     - `enums_introspect_like.json`
3. **收集实现侧符号**
   - 生成或刷新 `nm_symbols.json`，让流水线知道实现侧有哪些符号可用。
4. **生成 ABI 与 wrappers**
   - 生成 wrapper 源码与元数据（`mjwf_abi_funcs.*`、`mjwf_abi_structs.*`、wrapper export manifests）。
   - 产出最终导出清单 `dist/<ver>/abi/exports.lst`。
5. **构建 WASM 产物**
   - 通过 `emcmake cmake` 配置、`cmake --build` 编译链接。
   - `app/CMakeLists.txt` 会消费 wrapper 源码与导出清单，输出：
     - `dist/<ver>/mujoco.js`
     - `dist/<ver>/mujoco.wasm`
6. **Post-build 校验**
   - 校验导出/ABI 相关的 manifest（以及可选的覆盖率检查）。
7. **可选运行时 checks（`--with-checks`）**
   - 使用 `check/tests/*.mjs` 对 `dist/<ver>` 做 smoke 与质量门控。

## 迭代快路径（避免每次全量重编）

`python3 forge_cli.py build --version <mjver>` 的目标是“正确 + 可验证”，会产出完整、已校验的 `dist/<ver>`。
日常只改几行代码时，通常没必要每次都跑完整流水线。

### 1) 只改文档 / benchmark / Node checks

如果你只改了文档、benchmark，或 `check/tests/*.mjs`，一般**不需要重新编译 WASM**；直接基于已有的 `dist/<ver>` 运行对应的
Node 脚本即可。

### 2) 只改 wrapper/app 代码（`app/*`）

如果你只改了 `app/` 下的 wrapper 源码（或其它被 CMake 消费的本地代码），建议直接对既有 build 目录做增量构建，而不是重跑整条流水线。

- build 目录布局（默认）：`<repo>/build/forge/<short>/{single|pthreads}`
- 若设置了 `MJWF_BUILD_ROOT`：`<MJWF_BUILD_ROOT>/forge/<short>/{single|pthreads}`

示例（在 `EMSDK` 可用的同一套 shell 环境里执行）：

```bash
cmake --build "<build_dir>" -- -j "$(nproc)"
```

如果重跑 `forge_cli.py build`，它会刷新上游检出 + 重跑 introspect + 生成器，哪怕只改了两行本地代码也可能触发更大范围的重建。

### 3) 只改 ABI 生成器脚本（`abi_exports/*`）

如果你只改了生成器逻辑（例如 `abi_exports/gen_funcs.py`），可以先重跑生成器，再增量编译：

```bash
python3 -m abi_exports.gen_funcs --version <mjver>
cmake --build "<build_dir>" -- -j "$(nproc)"
```

如果你改的是其它生成器，也需要一并重跑（例如 `abi_exports/gen_structs.py`、`abi_exports/gen_enums.py`、
`abi_exports/gen_scene_geom_soa.py`）。

### 4) 改了上游 ref / `introspect/*` / MuJoCo patches

只要改动会影响上游头文件或 ABI 输入，建议直接跑完整流水线：

```bash
python3 forge_cli.py build --version <mjver>
```

### Debug 编译提示（需要符号/断言时）

`forge_cli.py build` 默认使用 `-DCMAKE_BUILD_TYPE=Release` 与 `-DMJWF_PROFILE=fast`（见 `app/CMakeLists.txt`）。如果你需要更多 debug
可观测性（断言、source map 等），可以对既有 build 目录用不同的 `MJWF_PROFILE` 重新配置，然后用 `cmake --build` 增量构建。注意 debug
profile 往往会增加构建时间与产物体积。

任意阶段失败通常意味着：

- 环境/工具链缺失或不匹配（Node/emcc/clang 等）；
- 上游 ref 变化导致 wrappers/introspection 需要更新；
- 导出清单漂移（之前导出的符号缺失或改名）；
- smoke tests 抓到了运行时回归。
