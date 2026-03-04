# CLI 参考（`forge_cli.py`）

所有命令均在仓库根目录执行。

## `build`

构建 `dist/<ver>`：

```bash
python3 forge_cli.py build --version 3.5.0
```

构建 pthreads 变体（浏览器端需要 COOP/COEP + `SharedArrayBuffer`）：

```bash
python3 forge_cli.py build --version 3.5.0 --pthreads
```

构建后执行 smoke + quality gates：

```bash
python3 forge_cli.py build --version 3.5.0 --with-checks
```

## `collect-versions`

列出 `dist/*` 的版本：

```bash
python3 forge_cli.py collect-versions
```

输出 GitHub Actions 友好的结果：

```bash
python3 forge_cli.py collect-versions --github-output
```

## `verify-dist`

对比“已提交 dist”与“可复现构建检出”：

```bash
python3 forge_cli.py verify-dist --version 3.5.0 --ci-build-dir ci-build
```

## 退出码

任意阶段失败（工具链/构建/门控/检查）都会让 CLI 返回非 0。
