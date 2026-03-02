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

任意阶段失败通常意味着：

- 环境/工具链缺失或不匹配（Node/emcc/clang 等）；
- 上游 ref 变化导致 wrappers/introspection 需要更新；
- 导出清单漂移（之前导出的符号缺失或改名）；
- smoke tests 抓到了运行时回归。

