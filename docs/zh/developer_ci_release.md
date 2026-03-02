# CI 与发布

## CI 概览

GitHub Actions 工作流 `.github/workflows/forge-dist-verify.yml` 用于验证：仓库里已提交的 `dist/<ver>/`
是否能被干净检出可复现地重建出来。

高层逻辑：

1. 选择需要验证的版本（根据改动路径、tag 或手动输入）。
2. 在干净检出目录 `ci-build/` 中构建目标版本。
3. 运行 `python3 forge_cli.py verify-dist --version <ver>` 对比“已提交 dist”与“可复现构建”。

## Tag 约定

匹配 `forge-*` 的 tag 会被视为 release-like 触发器，例如：

- `forge-3.4.0-r1`
- `forge-3.5.0-r1`

## 本地复现 CI 的 verify 步骤

准备第二份干净检出（或目录）构建后，执行：

```bash
python3 forge_cli.py verify-dist --version <ver> --ci-build-dir ci-build
```

