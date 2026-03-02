# 兼容性模型

`mujoco-wasm-forge` 把每个 MuJoCo 版本/ref 视为一套独立的、版本化的产物：`dist/<ver>/`。

## 这里的“兼容性”指什么

升级 MuJoCo 时，你应预期变化可能来自：

- headers 声明（introspection 输出）；
- 实现侧可用符号（符号清单）；
- wrapper 接口面（`mjwf_*` 导出）；
- 体积/初始化耗时等质量指标（quality gates）。

本项目的承诺不是“上游永不变化”，而是让变化：

- **可检测**（门控失败或输出漂移报告）；
- **可审计**（`dist/<ver>/abi/` 有证据链）；
- **可行动**（能定位到 wrappers/exports 的具体差异并修复）。

## 升级如何被验证

推荐流程：

1. 构建新版本：`python3 forge_cli.py build --version <new> --with-checks`。
2. 审阅 `dist/<new>/abi/exports.lst` 以及 `dist/<new>/abi/` 下的报告。
3. 必要时启用“强制门控”（见 `reference_env_vars` 与 `reference_checks_gates`）。

常见失败类型：

- **构建/工具链**：Emscripten/Node/clang 缺失或不兼容。
- **ABI/导出漂移**：某个 wrapper 导出消失或变化。
- **运行时回归**：smoke tests 无法编译/step 最小模型。
- **质量回退**：`check/tests/gates.mjs` 报告体积/初始化耗时超阈值。

