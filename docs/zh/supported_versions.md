# 已支持的 MuJoCo 版本（已提交 dist/）

本仓库支持通过下面命令构建 **任意** 上游 MuJoCo ref：

```bash
python3 forge_cli.py build --version <mjver>
```

为便于使用与 CI `verify-dist`，仓库会把部分 `dist/<ver>/` 产物提交到 git。

## 当前 `dist/` 里已有的版本

- `3.3.7`
- `3.4.0`
- `3.5.0`
- `3.6.0`
- `3.7.0`
- `3.8.0`
- `3.8.1`

当前已提交 pthreads 产物的版本是 `3.5.0` 和 `3.8.1`。

## Release tag 约定

CI 会把匹配 `forge-*` 的 tag 视为 release-like 触发器，例如：

- `forge-3.4.0-r1`
- `forge-3.5.0-r1`

本轮 MuJoCo 3.6+ release 批次使用：

- `forge-3.6.0-r1`
- `forge-3.7.0-r1`
- `forge-3.8.0-r1`
- `forge-3.8.1-r1`
