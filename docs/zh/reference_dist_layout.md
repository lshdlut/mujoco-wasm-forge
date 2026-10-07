# Dist 布局参考

每个 MuJoCo 版本/ref 的 dev 构建产物都在 `dist/<ver>/` 下；验证后提升到源码检出时，内部结构不变，但持久路径改为 `deliverables/<ver>/`。

## 典型目录结构

```text
dist/<ver>/
  mujoco.js
  mujoco.wasm
  version.json             # release metadata
  provenance.json          # additive build/runtime provenance
  LICENSE                  # 发布包携带
  NOTICE* / notices/       # 源检出存在时携带
  pthreads/                # 可选（使用 --pthreads 构建时产生）
    mujoco.js
    mujoco.wasm
    mjwasm_forge.js        # 存在时保留的兼容 worker alias
    mjwasm_forge.wasm
    version.json
    provenance.json
    *.worker.*             # Emscripten 线程 worker 辅助文件（名称随工具链而变）
  abi/
    exports.lst
    exports_check.json
    exports_check.pthreads.json  # 可选（pthreads 变体的导出检查结果）
    exports_report_funcs.md
    nm_symbols.json
    nm_coverage.single.json       # 存在时的 single coverage
    nm_coverage.pthreads.json     # 存在时的 pthreads coverage
    js_signature_capabilities.json # JS/raw-call 分类（存在时）
    wrapper_exports_funcs.json
    functions_introspect_like.json
    structs_introspect_like.json
    enums_introspect_like.json
    mujoco_ast.json
```

引用已提交基线时，将开头的 `dist/` 替换为 `deliverables/`。

## 关键 ABI 文件

| 文件 | 作用 |
| --- | --- |
| `exports.lst` | 链接阶段使用的导出契约。 |
| `nm_symbols.json` | 实现侧符号清单（用于导出/覆盖率检查）。 |
| `wrapper_exports_funcs.json` | `abi_exports/*` 生成的 wrapper 导出集合。 |
| `exports_report_funcs.md` | 人类可读的导出状态报告。 |
| `provenance.json` | additive release manifest，包含 runtime SHA-256 与可选的已验证 build receipt 字段。 |
| `nm_coverage.<flavor>.json` | 与相应 runtime 产物绑定的 flavor-specific 符号覆盖率。 |
| `js_signature_capabilities.json` | 基于类型的 JS capability/raw-call 分类与 adapter 说明。 |
| `*_introspect_like.json` | headers 导出的声明信息（便于 diff/审计）。 |
| `mujoco_ast.json` | clang AST dump（体积大，更多用于排查与审计）。 |

已提交的 `deliverables/<ver>/` 是 release packaging 的输入；`dist/<ver>/` 只用于 dev 暂存。runtime zip 不包含
`abi/`，audit zip 包含 `abi/` 及相同的法律材料。pthreads worker alias 按源文件原样复制，packager 不会凭空生成 alias。
