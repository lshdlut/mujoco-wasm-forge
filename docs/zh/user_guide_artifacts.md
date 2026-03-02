# 产物与保证

`mujoco-wasm-forge` 的输出统一写到 `dist/<ver>/`。

## 主要产物（大多数使用者只需要这些）

- `dist/<ver>/mujoco.js`
- `dist/<ver>/mujoco.wasm`

通常你只需要把这两个文件拷到你的 web/worker/node 项目里。

## ABI 与审计产物（维护者/审阅者用）

`dist/<ver>/abi/` 目录保存了“升级可审计”的证据链：

- `exports.lst`：唯一导出清单（链接契约）。
- `nm_symbols.json`：实现侧符号清单。
- `wrapper_exports_funcs.json`：wrapper 生成器产出的导出集合。
- `exports_check.json` / `exports_report_funcs.md`：导出检查结果与可读报告。
- `*_introspect_like.json`：从 headers 提取的声明信息（便于 diff/审计）。
- `mujoco_ast.json`：clang AST dump（体积大，更多用于排查与审计）。

## 版本选择（跑 checks 时很重要）

很多脚本通过环境变量确定当前生效的版本：

- `MJVER`（优先）
- `DIST_VERSION`

`forge_cli.py build --version <mjver>` 在启动子进程时会设置这些变量；如果你手动运行脚本（例如
`node check/tests/smoke.mjs`），建议显式设置 `MJVER`/`DIST_VERSION`。

