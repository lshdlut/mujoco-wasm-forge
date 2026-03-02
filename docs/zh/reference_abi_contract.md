# ABI 契约（exports & wrappers）

`mujoco-wasm-forge` 把导出符号视为显式契约来管理。

## 契约是什么

- 绝对权威的导出清单是 `dist/<ver>/abi/exports.lst`。
- WASM 构建会通过 Emscripten `-sEXPORTED_FUNCTIONS=@.../exports.lst` 强制使用它。

因此，“导出了什么”永远可以用文本 diff 直接审阅。

## A ∩ B = C（扁平导出）

流水线会把三类集合显式化，便于审计：

- **A**：来自 headers 的声明（introspection）。
- **B**：来自实现侧的符号清单（`nm_symbols.json`）。
- **C**：来自 wrappers 的导出集合（通常以 `mjwf_*` 为主）。

`abi_exports/` 下的生成器会把这些输入合并，产出：

- wrapper 源码（`mjwf_abi_funcs.*`、`mjwf_abi_structs.*`）；
- wrapper 导出 manifests（用于审计）；
- `exports.lst`（用于链接）。

## JS/Runtime 接口面

主要产物是 Emscripten 的 `mujoco.js`/`.wasm`。运行时入口（如 `ccall`/`cwrap`）由 `app/CMakeLists.txt`
里的链接参数导出。

