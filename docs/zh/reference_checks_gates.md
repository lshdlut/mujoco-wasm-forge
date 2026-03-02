# Checks 与 gates

本项目对构建做两层验证：

1. **Post-build 校验**（构建流水线中对导出/ABI manifests 的校验）。
2. **运行时 checks**（Node 下的 smoke tests 与质量门控）。

## Post-build 校验

`check/post_build.sh` 会校验构建过程中产出的导出相关 manifests。`forge_cli.py build` 会在跑运行时 checks 前执行它。

## 运行时 checks（`--with-checks`）

`forge_cli.py build --with-checks` 会运行：

- `check/tests/smoke.mjs`
  - 编译一个最小 pendulum XML，step 若干次并释放资源。
- `check/tests/mesh-smoke.mjs`
  - 覆盖 mesh 解析/编译的基本路径。
- `check/tests/gates.mjs`
  - 质量门控：wasm/js 体积阈值与模块初始化耗时。
  - 仅在 `QUALITY_ENFORCE=1` 时作为失败（否则只输出警告）。

仓库里还包含一些用于临时排查的脚本（默认不跑），例如：

- `check/tests/xml-missing-ref.mjs`（验证缺失资源时的错误信息可读性）。

## 调参

`gates.mjs` 相关环境变量见 `reference_env_vars`。

