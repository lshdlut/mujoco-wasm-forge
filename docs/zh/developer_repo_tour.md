# 仓库导览

## 顶层结构

- `forge_cli.py`：本地 + CI 的单一入口。
- `app/`：WASM wrapper 构建的 CMake 与源码（链接 MuJoCo + wrappers）。
- `introspect/`：基于 clang 的 header 扫描与 introspection glue。
- `abi_exports/`：生成 wrappers、manifests 与 `exports.lst`。
- `abi_impl/`：实现侧符号清单/覆盖率相关工具。
- `check/`：post-build 校验，以及 Node 下的 smoke/quality gates。
- `dist/`：已提交的产物目录（`dist/<ver>/...`）。

## 外部源码

- `external/mujoco`：上游 MuJoCo 检出（在本仓库里不被 git 追踪；由 forge CLI 自动创建/更新）。

