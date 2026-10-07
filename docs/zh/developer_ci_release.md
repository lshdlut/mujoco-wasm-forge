# CI 与发布

## CI 概览

GitHub Actions 工作流 `.github/workflows/forge-dist-verify.yml` 用于验证：仓库里已提交的 `deliverables/<ver>/`
是否与干净检出后生成的 `dist/<ver>/` 完全一致。

高层逻辑：

1. 选择需要验证的版本（根据改动路径、tag 或手动输入）。
2. 在干净检出目录 `ci-build/` 中构建目标版本。
3. 运行 `python3 forge_cli.py verify-dist --version <ver>` 对比 `deliverables/<ver>` 与 `ci-build/dist/<ver>`。

## Tag 约定

匹配 `forge-*` 的 tag 会被视为 release-like 触发器，例如：

- `forge-3.4.0-r1`
- `forge-3.5.0-r1`

发布一个已提交的 `deliverables/<ver>/` 时，应在已经通过 verify 的 commit 上打 tag：

```bash
git tag forge-<ver>-r1
git push origin forge-<ver>-r1
```

对于已经检查过的 MuJoCo 3.9+ 批次，应在同一个已验证 commit 上创建 tag，
并逐个 push，使每个 tag 都产生独立的 workflow event：

```bash
git tag forge-3.9.0-r1
git tag forge-3.10.0-r1
git tag forge-3.11.0-r1
git tag forge-3.12.0-r1
git tag forge-3.13.0-r1
git tag forge-3.14.0-r1
git tag forge-3.15.0-r1
git push origin forge-3.9.0-r1
git push origin forge-3.10.0-r1
git push origin forge-3.11.0-r1
git push origin forge-3.12.0-r1
git push origin forge-3.13.0-r1
git push origin forge-3.14.0-r1
git push origin forge-3.15.0-r1
```

## Release 产物

对于 release-like tag，CI 会在同一个 GitHub Release 下发布两个 asset：

- `dist-runtime.zip`
  - 面向下游站点 / 应用的 runtime 产物
  - zip 根目录保持 forge webroot 形状（`mujoco.js`、`mujoco.wasm`、可选 `pthreads/`、`version.json`、`provenance.json`）
  - 不包含 `abi/`
  - 只有当 `deliverables/<ver>/` 下存在 pthreads 变体时才包含 `pthreads/`
  - 如果存在 `mjwasm_forge.js` / `mjwasm_forge.wasm`，保留 pthread worker aliases
  - 携带仓库 `LICENSE` 以及已有的 `NOTICE*` / `notices/` 材料

- `dist-audit.zip`
  - 面向维护者的审计 / 调试产物
  - zip 根目录包含 `abi/`、`version.json`、`provenance.json` 和同样的法律材料

`version.json` 保持 legacy schema，并指向 additive 的 `provenance.json`。manifest 记录 release id、asset kind、
runtime 文件 SHA-256 映射，以及检测到的 `single` / `pthreads` flavor。如果 `deliverables/<ver>/abi/` 下有已验证
的 build receipt（或通过 `--build-metadata` 指定），还会记录 resolved upstream SHA 与 Emscripten SDK。没有
receipt 的历史 deliverable 保留明确的 `null` 字段和 `provenanceStatus: "unknown"`；packager 不会从版本名或本机
`C:\dev` 路径推造这些值。

本地打包命令：

```bash
python3 tools/package_release_assets.py \
  --dist-dir deliverables --version <ver> --dist-id forge-<ver>-rN \
  --git-sha <commit-sha> --out-dir release-assets \
  [--build-metadata deliverables/<ver>/abi/build_metadata.json]
```

打包只是 release asset 步骤，不会自动创建 tag 或发布 GitHub Release。3.9.0--3.15.0
批次已经提交到 `main` 并通过 CI 验证；逐个 push 对应 tag 后，会启动对应版本的验证，
只有验证和打包都成功，GitHub 才会发布 release 资产。

## 本地复现 CI 的 verify 步骤

准备第二份干净检出（或目录）构建后，执行：

```bash
python3 forge_cli.py verify-dist --version <ver> --ci-build-dir ci-build
```
