# How-to：构建指定的 MuJoCo 版本/ref

## 构建

在仓库根目录（WSL/Linux）执行：

```bash
python3 forge_cli.py build --version 3.5.0
```

`--version` 会用于 `external/mujoco` 的上游检出，因此它可以是：

- 版本 tag，例如 `3.5.0`；
- 分支名；
- commit hash。

## 产物

成功后会生成：

- `dist/<ver>/mujoco.js`
- `dist/<ver>/mujoco.wasm`
- `dist/<ver>/abi/*`（introspection + 导出/ABI manifests 与报告）

## build 目录放置（同步盘强烈建议）

默认情况下，forge CLI 会把中间产物放在 `<repo>/build`。如需把 build tree 放到仓库外：

```bash
MJWF_BUILD_ROOT=/tmp/mjwf_build python3 forge_cli.py build --version 3.5.0
```

## 强制干净重建

如需强制重新拉取上游 MuJoCo，可以删除 `external/mujoco` 后再 build。该目录在本仓库里不被 git 追踪，forge CLI 会自动重建。

