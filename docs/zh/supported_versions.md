# 已支持的 MuJoCo 版本（已提交 deliverables/）

构建 CLI 接受上游 MuJoCo ref；每个新 ref 仍需单独验证：

```bash
python3 forge_cli.py build --version <mjver>
```

为便于使用与 CI `verify-dist`，仓库会把验证过的 `deliverables/<ver>/` 基线提交到 git。

## `deliverables/` 中的历史已提交基线

- `3.3.7`
- `3.4.0`
- `3.5.0`
- `3.6.0`
- `3.7.0`
- `3.8.0`
- `3.8.1`

历史已提交 pthreads 产物的版本是 `3.5.0` 和 `3.8.1`。

## 已提交并经 CI 验证的升级批次（2026-10-07）

仓库另有已提交的 `3.9.0` 到 `3.15.0` 产物，每个版本都有 single 和
pthreads 变体。它们均通过 export、runtime/asset/plugin、scalar-width 和
强制 quality checks。3.14+ 通过嵌套 MJZ/OBJ 资源测试；3.10+ 的 pthreads 版本
创建了 engine thread pool 并推进模型。

全新 Linux CI 重建了两路产物并通过严格逐字节复现：
[3.15.0 canary](https://github.com/lshdlut/mujoco-wasm-forge/actions/runs/37605433870) 与
[3.9.0--3.14.0 matrix](https://github.com/lshdlut/mujoco-wasm-forge/actions/runs/37606392053)。

这些是仓库 deliverables，尚未作为新的 GitHub release 发布；已发布批次仍止于
`forge-3.8.1-r1`。旧 release 文件保持不变。独立的 3.8.1 兼容运行时再次通过检查，
但没有按最终 closing ABI 重建，也不计入新版 14 产物复现矩阵。
下游对新引擎特性的集成尚未验证；这些构建/运行门禁不证明不同引擎版本的数值等价。

## Release tag 约定

CI 会把匹配 `forge-*` 的 tag 视为 release-like 触发器，例如：

- `forge-3.4.0-r1`
- `forge-3.5.0-r1`

本轮 MuJoCo 3.6+ release 批次使用：

- `forge-3.6.0-r1`
- `forge-3.7.0-r1`
- `forge-3.8.0-r1`
- `forge-3.8.1-r1`
