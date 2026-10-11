# CLI 参考（`forge_cli.py`）

所有命令均在仓库根目录执行。

## `build`

构建 `dist/<ver>`：

```bash
python3 forge_cli.py build --version 3.5.0
```

构建 pthreads 变体（浏览器端需要 COOP/COEP + `SharedArrayBuffer`）：

```bash
python3 forge_cli.py build --version 3.5.0 --pthreads --fresh-dependency
```

构建后执行 smoke + quality gates：

```bash
python3 forge_cli.py build --version 3.5.0 --with-checks
```

已有 dirty dependency 默认会被保留并使构建停止。构建新版本、变体或重建时传入
`--fresh-dependency`：完整的现有 checkout 会先移动到
`<build-root>/dependency-snapshots/mujoco-<unique-id>`，再准备 fresh clone；clone 失败时该 snapshot 仍可恢复。

Dimension accessor 继续使用 legacy i32 / JavaScript Number 契约。超出 signed i32 范围的值返回 `-1` 并给出诊断；
声明为 `mjtSize` 的 dimension 另提供 `<name>_i64`，返回精确的 JavaScript BigInt。直接的 `mjtSize` pointer view
需要 `BigInt64Array`，`mjtBool` view 使用 `Uint8Array`。

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

对比“已提交 deliverables”与“可复现构建检出”：

```bash
python3 forge_cli.py verify-dist --version 3.5.0 --ci-build-dir ci-build
```

## 退出码

任意阶段失败（工具链/构建/门控/检查）都会让 CLI 返回非 0。
