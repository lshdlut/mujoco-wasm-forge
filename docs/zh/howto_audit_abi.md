# How-to：审计 ABI / 排查 diffs

当升级 MuJoCo 触发导出变化时，最快的排查方法是沿着 `dist/<ver>/abi/` 的“证据链”逐步定位来源。

## 关键文件

- `exports.lst`：最终 wasm 构建导出的符号（链接契约）。
- `wrapper_exports_funcs.json`：wrapper 生成器打算导出的集合。
- `nm_symbols.json`：实现侧提供的符号清单。
- `exports_report_funcs.md`：人类可读的导出状态报告。
- `functions_introspect_like.json`：headers 声明（introspection）。

## 常见问题定位

| 问题 | 看哪里 |
| --- | --- |
| “headers 里有声明吗？” | `functions_introspect_like.json` |
| “实现侧有这个符号吗？” | `nm_symbols.json` |
| “wrapper 生成了吗？” | `wrapper_exports_funcs.json` |
| “最终导出了吗？” | `exports.lst` |

## 常见原因

- 该函数上游存在，但在当前构建配置下不可用。
- wrapper 生成器有意排除它（契约选择）。
- 上游重命名/拆分导致 wrappers/exports 需要同步更新。

