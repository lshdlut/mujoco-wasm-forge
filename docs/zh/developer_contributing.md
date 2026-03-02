# 贡献指南

## 预期与边界

- 尽量保持变更聚焦（一个 PR 一个意图）。
- 修改流水线代码时，尽量同步更新至少一个已提交的 `dist/<ver>`（或用 `verify-dist` 证明没有漂移）。
- 修改文档结构时，**同时**更新 `docs/en/` 与 `docs/zh/`。

## 常见本地流程

1. 构建目标版本（带 checks）：
   - `python3 forge_cli.py build --version <mjver> --with-checks`
2. 审阅 `dist/<ver>/abi/` 下的变化（导出清单与报告）。
3. 如果要提交 `dist/<ver>`，确保 CI 可复现（`verify-dist`）。

## 不应提交的内容

- `external/`（上游检出目录）
- 中间 build trees（默认 `<repo>/build` 或 `MJWF_BUILD_ROOT`）
- 本地 cache/log 等

