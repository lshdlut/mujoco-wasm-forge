# How-to：带 checks 构建

## 一条命令 build + checks

```bash
python3 forge_cli.py build --version 3.5.0 --with-checks
```

它会在构建完成后执行：

- `node check/tests/smoke.mjs`
- `node check/tests/mesh-smoke.mjs`
- `node check/tests/gates.mjs`

## 手动运行 checks

手动执行 checks 时，建议显式设置 `MJVER`（或 `DIST_VERSION`）来选择 `dist/<ver>`：

```bash
MJVER=3.5.0 node check/tests/smoke.mjs
MJVER=3.5.0 node check/tests/mesh-smoke.mjs
MJVER=3.5.0 node check/tests/gates.mjs
```

如需把质量门控从“警告”升级为“失败”，设置 `QUALITY_ENFORCE=1`（见 `reference_env_vars`）。

