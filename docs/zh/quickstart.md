# 快速开始

本页给出最短路径，让你拿到可用的 `dist/<ver>/mujoco.js` + `dist/<ver>/mujoco.wasm`。

## 前置条件

- Linux/WSL，或已配置好 Emscripten/Bash/CMake 的 Windows 环境用于构建。
- Python 3。
- Node.js ≥ 20。
- Emscripten SDK（emsdk）4.0.10（或兼容版本），并且 `EMSDK` 指向 emsdk 目录。
- CMake（通过 `emcmake` 调用）。
- Ninja（当前 recipe 必需；请安装 Ninja，或把可执行文件路径设为 `MJWF_NINJA`）。

## 一条命令构建（带 checks）

请在当前版本的本地、非 OneDrive 开发检出目录中执行。例如 Windows 下可以
新建 `C:\dev\mujoco-wasm-forge\build-checkout`：

```powershell
git clone https://github.com/lshdlut/mujoco-wasm-forge.git C:\dev\mujoco-wasm-forge\build-checkout
```

同步盘检出只用于源码和已提交的 deliverables，`forge_cli.py build` 会拒绝从
OneDrive 目录运行。

在该开发检出目录中执行：

```bash
python3 forge_cli.py build --version 3.15.0 --with-checks
```

如果希望把 build tree 放到另一个本地目录，设置 `MJWF_BUILD_ROOT`：

```bash
MJWF_BUILD_ROOT=/tmp/mjwf_build python3 forge_cli.py build --version 3.15.0 --with-checks
```

## 产物在哪里

- 主要产物：
  - `dist/3.15.0/mujoco.js`
  - `dist/3.15.0/mujoco.wasm`
- 审计/ABI 产物：
  - `dist/3.15.0/abi/`（导出清单、introspect JSON、符号清单、报告等）

## 下一步

- 想切版本/ref：看 `howto_build_version`。
- 想集成到 worker/Node：看 `howto_web_worker` / `howto_node`。
