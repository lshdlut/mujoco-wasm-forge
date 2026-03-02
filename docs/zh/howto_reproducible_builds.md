# How-to：固定与复现构建

## 固定上游 MuJoCo

如需严格复现，建议使用 commit hash：

```bash
python3 forge_cli.py build --version <git-sha>
```

同时记录你的工具链版本（Node、emsdk/emcc、Python）。

## 用 `verify-dist` 对比“已提交 dist”与“可复现构建”

forge CLI 提供了面向 CI 的对比命令：

```bash
python3 forge_cli.py verify-dist --version 3.5.0 --ci-build-dir ci-build
```

核心思路：

1. 在 `ci-build/` 里用干净检出构建目标版本；
2. 仓库根目录的 `dist/<ver>` 是已提交产物；
3. `verify-dist` 会做必要的规范化，然后 diff 两棵目录树。

## CI 辅助

- `python3 forge_cli.py collect-versions`（以及 `--github-output`）可帮助 CI 自动选择需要验证的 `dist/<ver>`。

