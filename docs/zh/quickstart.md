# 快速开始

本页给出最短路径，让你拿到可用的 `dist/<ver>/mujoco.js` + `dist/<ver>/mujoco.wasm`。

## 前置条件

- Linux 环境（Windows 推荐用 WSL）用于 Emscripten 构建。
- Python 3。
- Node.js ≥ 20。
- Emscripten SDK（emsdk）4.0.10（或兼容版本），并且 `EMSDK` 指向 emsdk 目录。
- CMake（通过 `emcmake` 调用）。

## 一条命令构建（带 checks）

在仓库根目录执行：

```bash
python3 forge_cli.py build --version 3.5.0 --with-checks
```

如果你希望把中间 build tree 放到仓库外（强烈建议在同步盘/OneDrive 上这样做），设置 `MJWF_BUILD_ROOT`：

```bash
MJWF_BUILD_ROOT=/tmp/mjwf_build python3 forge_cli.py build --version 3.5.0 --with-checks
```

## 产物在哪里

- 主要产物：
  - `dist/3.5.0/mujoco.js`
  - `dist/3.5.0/mujoco.wasm`
- 审计/ABI 产物：
  - `dist/3.5.0/abi/`（导出清单、introspect JSON、符号清单、报告等）

## 下一步

- 想切版本/ref：看 `howto_build_version`。
- 想集成到 worker/Node：看 `howto_web_worker` / `howto_node`。

