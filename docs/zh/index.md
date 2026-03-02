# mujoco-wasm-forge 文档

`mujoco-wasm-forge` 是一套用于生成 MuJoCo WebAssembly 构建产物，并把导出符号/ABI 变成“可控、可审计”的工具链。
它会把指定的上游 MuJoCo ref 构建成 `dist/<ver>/mujoco.js` + `dist/<ver>/mujoco.wasm`，并在 `dist/<ver>/abi/` 下产出审计产物。

## 给使用者的 30 秒版本

- 只想拿到可用产物：直接使用仓库里已提交的 `dist/<ver>/`，或运行 `python forge_cli.py build --version <mjver>` 自己构建。
- 导出符号由 `dist/<ver>/abi/exports.lst` 显式定义，并在链接阶段强制执行。
- `--with-checks` 会在构建后跑 smoke + 质量门控（`check/tests/*.mjs`）。
- 维护者可以用 `dist/<ver>/abi/*` 快速审计升级时的接口变化与差异来源。

## 术语小抄

| 术语 | 通俗解释 |
| --- | --- |
| `dist/<ver>` | 针对某个 MuJoCo 版本/ref 的构建产物目录。 |
| `exports.lst` | 唯一的导出清单（传给 Emscripten：`-sEXPORTED_FUNCTIONS=@...`）。 |
| wrapper | 以 `mjwf_*` 为主的一层封装函数，构成可审计的 C 接口面。 |
| introspect | 从 `mujoco.h` 自动提取函数/结构体/枚举信息（含 AST）。 |
| gate | 门控检查：用于发现 ABI/导出漂移或质量回退（体积/初始化耗时等）。 |

## 从哪里开始

- 新用户：先看 `quickstart`，再看 `howto_build_version`。
- 集成方：看 `howto_web_worker` 和 `howto_node`。
- 维护者：看 `user_guide_pipeline`、`reference_abi_contract`、`reference_checks_gates`。

```{toctree}
:maxdepth: 2
:caption: 快速开始

quickstart
```

```{toctree}
:maxdepth: 2
:caption: 用户指南

user_guide_overview
user_guide_pipeline
user_guide_artifacts
user_guide_compatibility
```

```{toctree}
:maxdepth: 2
:caption: How-to（任务式）

howto_build_version
howto_build_with_checks
howto_web_worker
howto_node
howto_reproducible_builds
howto_audit_abi
```

```{toctree}
:maxdepth: 2
:caption: 参考（权威可查）

reference_cli
reference_env_vars
reference_dist_layout
reference_abi_contract
reference_checks_gates
supported_versions
glossary
```

```{toctree}
:maxdepth: 2
:caption: 开发者

developer_contributing
developer_repo_tour
developer_add_version
developer_modify_generation
developer_ci_release
```

