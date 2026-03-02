# Dist 布局参考

每个 MuJoCo 版本/ref 的产物都在 `dist/<ver>/` 下。

## 典型目录结构

```text
dist/<ver>/
  mujoco.js
  mujoco.wasm
  abi/
    exports.lst
    exports_check.json
    exports_report_funcs.md
    nm_symbols.json
    wrapper_exports_funcs.json
    functions_introspect_like.json
    structs_introspect_like.json
    enums_introspect_like.json
    mujoco_ast.json
```

## 关键 ABI 文件

| 文件 | 作用 |
| --- | --- |
| `exports.lst` | 链接阶段使用的导出契约。 |
| `nm_symbols.json` | 实现侧符号清单（用于导出/覆盖率检查）。 |
| `wrapper_exports_funcs.json` | `abi_exports/*` 生成的 wrapper 导出集合。 |
| `exports_report_funcs.md` | 人类可读的导出状态报告。 |
| `*_introspect_like.json` | headers 导出的声明信息（便于 diff/审计）。 |
| `mujoco_ast.json` | clang AST dump（体积大，更多用于排查与审计）。 |

