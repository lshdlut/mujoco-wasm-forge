# 环境变量与构建开关

## 版本选择

这些变量用于指定当前脚本应消费哪个 `dist/<ver>`：

- `MJVER`
- `DIST_VERSION`

它们被 `dist_version.py` 与 `check/dist_paths.mjs` 使用。

## 工具链

- `EMSDK`：emsdk 目录路径（需包含 `emsdk_env.sh`）。Emscripten 的 CMake 构建需要它。

## build tree 放置

- `MJWF_BUILD_ROOT`：中间构建目录的根（默认 `<repo>/build`）。

## 导出清单覆盖

- `MJWF_EXPORTS_LIST`：覆盖链接阶段使用的导出清单路径（默认 `dist/<ver>/abi/exports.lst`）。

## CMake profile

`app/CMakeLists.txt` 定义了 CMake cache 变量 `MJWF_PROFILE`：

- `default`
- `fast`（当前 forge CLI 默认使用）
- `official`
- `fast_official`

它主要影响优化/调试 flags、filesystem 设置以及相关 link options。

## 质量门控（`check/tests/gates.mjs`）

`gates.mjs` 会检查 wasm/js 体积和初始化耗时。

- `QUALITY_ENFORCE=1`：把超阈值从“警告”变成“失败”。
- `WASM_MAX_BYTES`（默认：16_000_000）
- `JS_MAX_BYTES`（默认：2_000_000）
- `MAX_INIT_MS`（默认：8000）

