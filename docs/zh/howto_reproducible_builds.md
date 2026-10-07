# How-to：固定与复现构建

## 固定上游 MuJoCo

如需严格复现，建议使用 commit hash：

```bash
python3 forge_cli.py build --version <git-sha>
```

同时记录你的工具链版本（Node、emsdk/emcc、Python）。

请安装 Ninja（或以 `MJWF_NINJA` 指定其可执行文件）；当前 recipe 在所有宿主显式使用
Ninja。CMake Makefiles 与 Ninja 可能采用不同的 OBJECT-library archive 成员顺序，
即使运行行为一致，也会产生不同 WASM 字节。Receipt 检查实际 generator。旧 Makefiles
cache 应通过 `MJWF_BUILD_ROOT` 选择新的自有目录；保留旧 tree，不 reset 或删除。

## 用 `verify-dist` 对比“已提交 deliverable”与“可复现构建”

forge CLI 提供了面向 CI 的对比命令：

```bash
python3 forge_cli.py verify-dist --version 3.15.0 --ci-build-dir ci-build
```

核心思路：

1. 在 `ci-build/` 里用干净检出先以 `--with-checks` 构建普通 `dist/3.15.0`，再以 `--pthreads --fresh-dependency --with-checks` 构建 pthreads 变体；
2. 仓库根目录的 `deliverables/<ver>` 是已提交基线；
3. `verify-dist` 会做必要的规范化，然后 diff 两棵目录树。

新版已检查基线需要匹配的 Forge 源码 recipe 与 Emscripten 4.0.10。编译时 source
prefix maps 让 structured log 使用仓库相对路径；每个 flavor 的审计报告仍绑定实际 WASM SHA。
历史基线没有 retrofit：应在独立 workspace 使用匹配的历史 Forge tag/commit、SDK 与验证命令
复现，不能用当前 generator 和 audit schema 宣称复现了旧版基线。

## CI 辅助

- `python3 forge_cli.py collect-versions`（以及 `--github-output`）可帮助 CI 自动选择需要验证的 `deliverables/<ver>`。
