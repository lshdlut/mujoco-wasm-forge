# mujoco-wasm-forge

中文 | [English](README.md)

## 概览

mujoco-wasm-forge 是生成 MuJoCo WASM 包并验证其导出的流水线。若要为某个版本构建，推荐在仓库根目录运行 `python forge_cli.py build --version <mjver>`。该 CLI 会准备好 `dist/<ver>`，依次走完 introspect、ABI、导出、Emscripten 构建以及门控检查。`dist_version.py` 与 `check/dist_paths.mjs` 会告诉其他脚本当前生效的是哪一套 `dist/<ver>`，让整个流水线在不同版本间切换时无需额外改动。

## 自动化运行入口

根目录下的 `forge_cli.py` 会接管从 prepare、introspect、ABI 生成、`emcmake` 构建、`check/post_build.sh` 到可选 smoke/mesh/gates 测试的全部步骤，并且只需传入 `--version`（可选 `--short` 与 `--with-checks`）。通过 `python forge_cli.py build --version 3.3.7 --with-checks`（或目标 MuJoCo 版本）即可在本地或 CI 中复刻完整的、版本无关流程。

## 版本切换

切换目标 MuJoCo 版本只需运行 `python forge_cli.py build --version <mjver>`。该命令会拉取或检出对应的 `external/mujoco` 版本，依次执行 introspect、ABI、导出、构建并将结果写入 `dist/<ver>`。`dist_version.py`、`check/dist_paths.mjs` 与 `check/tests/*.mjs` 都会基于 `MJVER`/`DIST_VERSION` 或已存在的 `dist/<ver>` 读取版本，因而后续的校验逻辑直接消费同一目录。需要快速复核时，在现有 dist 下执行 `node check/tests/*.mjs` 即可，它们会自动使用当前版本。

### 设计思想

- **自动化主导** – introspect、ABI 生成以及导出检查工具自动产出 JSON/源代码，确保每个阶段只消费上一个阶段的权威产物，无需人工干预。
- **版本无依赖** – 切换到任何 MuJoCo 版本只需设置 `MJVER`/`DIST_VERSION`（或在 `dist/` 下准备对应目录），所有脚本会自动对齐该目录。
- **扁平导出（A ∩ B = C）** – `abi_exports/gen_funcs.py` 同时输出 A（introspect 声明）、B（`nm_symbols.json` 实现）和 C（wrapper）集合，`exports.lst` 作为 `-sEXPORTED_FUNCTIONS=@dist/<ver>/abi/exports.lst` 的输入，给链接器提供完整的可控导出列表。
- **ABI 门控** – `check/post_build.sh`、`check/check_exports.mjs` 与 smoke/mesh/gates 脚本统一读取上述导出清单，以 A/B/C 差异及时发现接口变更。

## 流程

- **构建** – 在 WSL 中使用 Node ≥20 和 emsdk 4.0.10，通过 `emcmake`/`em++` 将 MuJoCo 与自动生成的封装、`MJWF_PROFILE` 配置链接，传入 `-sEXPORTED_FUNCTIONS=@dist/<ver>/abi/exports.lst`，输出 `dist/<ver>/mujoco.js` 与 `.wasm`。
- **Introspect** – `introspect/forge/scan_clang_introspect.py` 为 `external/mujoco/include/mujoco/mujoco.h` 生成 clang AST，调用官方 introspect codegen 生成 `FUNCTIONS/STRUCTS/ENUMS`，并将结果写成 JSON 存入 `dist/<ver>/abi/`。
- **ABI 实现** – `abi_exports/gen_structs.py` 依据 `structs_introspect_like.json` 产出 `mjwf_abi_structs.*` 与 `mjwf_abi_structs.lst`。`abi_exports/gen_funcs.py` 把 introspect 声明、`nm_symbols.json`、额外导出整合成 `mjwf_abi_funcs.*`、`wrapper_exports_funcs.json` 与 `exports.lst`。
- **ABI 导出** – `check/post_build.sh` 验证 `wrapper_exports.json`、`exports_check.json` 和可选 `nm_coverage.json` 与本地构建一致，并让 `check_exports.mjs` 使用该 manifest，保障导出符号符合 A/B/C 关系。
- **App 层** – `app/` 中含自动生成包装、`mjwf_handles.c`、`mjwf_stubs.c`，CMake 将它们与 MuJoCo 源码编译成 `_wasm/mjwasm_forge.js`，导出 `mjwf_*` API。
- **校验** – `check/tests` 下的 `smoke.mjs`、`mesh-smoke.mjs`、`gates.mjs` 统一调用 `distDir()`/`distVersion()`，CI 在构建后运行这些脚本，验证导出、smoke/mesh 运行及质量门与 `exports.lst`、nm 扫描保持一致。

## 备注

- 前端示例（进行中）：https://github.com/lshdlut/mujoco-wasm-play

## 致谢

本项目受到早期 MuJoCo→WASM 试验的启发，感谢他们验证可行性并总结经验：

- [stillonearth/MuJoCo-WASM](https://github.com/stillonearth/MuJoCo-WASM)
- [zalo/mujoco_wasm](https://github.com/zalo/mujoco_wasm)
- [hashb/mujoco_web](https://github.com/hashb/mujoco_web)

## 源起

仓库部分脚本和文档在生成阶段使用了生成式 AI，并由维护者复核。
