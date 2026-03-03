# mujoco-wasm-forge

中文 | [English](README.md)

## 这是什么

`mujoco-wasm-forge` 是一套 **可复现的 MuJoCo→WebAssembly 构建流水线**，把“导出 ABI”当作可审计契约来管理。
它输出稳定命名的运行时产物（`mujoco.js` / `mujoco.wasm`），并在 `dist/<ver>/abi/` 下产出 ABI 审计材料。

它面向 **Simulate-style** 的 Web 应用（通过 `mujoco-wasm-play`），同时保证升级时能清楚看到：header、wrapper、exports、实现符号到底改了什么。

## 快速开始

```bash
python forge_cli.py build --version 3.5.0 --with-checks
```

产物位置：
- `dist/3.5.0/mujoco.js`, `dist/3.5.0/mujoco.wasm`
- `dist/3.5.0/abi/exports.lst` 等 ABI 产物
- 可选 pthreads 运行时：`dist/3.5.0/pthreads/`

## 为什么需要 forge

- **导出可控**：`dist/<ver>/abi/exports.lst` 作为唯一导出清单（传给 `-sEXPORTED_FUNCTIONS=@...`）。
- **ABI 可审计**：`dist/<ver>/abi/` 同时保存 introspect、wrapper、`nm` 视角，升级差异可解释可复核。
- **变体不改对外命名**：`dist/<ver>/mujoco.{js,wasm}`（single）与 `dist/<ver>/pthreads/mujoco.{js,wasm}`（pthreads）。
- **默认带插件**（例如 `mujoco.sensor.touch_grid`），避免 Simulate 工作负载“静默退化”。
- **质量门控**：smoke + mesh-smoke + 导出/ABI 检查可在本地与 CI 中跑。

## 官方 embind vs forge（要点快照）

下面是一个“你实际会拿到什么”的视角。具体测量数据请看 `bench/results/summary.md`。

| 维度 | 官方 embind（MuJoCo wasm/） | Forge |
| --- | --- | --- |
| 导出契约 | Embind 生成的 JS API；不产出 `exports.lst` | 显式 `dist/<ver>/abi/exports.lst` 驱动 `-sEXPORTED_FUNCTIONS` |
| ABI 审计产物 | 不随构建产出一个可审计包 | `dist/<ver>/abi/*`（introspect、wrappers、`nm` 视角、报告） |
| 线程默认（3.5.0） | `-pthread` + `PTHREAD_POOL_SIZE=navigator.hardwareConcurrency` | single 版本仍是一级产物；pthreads 作为可选变体 |
| 插件可用性（touch_grid） | 我们的 3.4.0/3.5.0 embind 构建中缺失 | 默认包含 |

## Bench（官方 embind vs forge + Play/Simulate HUD）

Bench 工具与汇总结果：
- `bench/README.md`
- `bench/results/summary.md`（由 `node bench/node/report.mjs` 生成）

## 流程（概览）

- 入口：`forge_cli.py`（`prepare → introspect → ABI → build → post-build checks`）。
- 输出：`dist/<ver>/mujoco.{js,wasm}` + `dist/<ver>/abi/*`。
- 可选 pthreads 运行时：`dist/<ver>/pthreads/`（ABI 仍共用 `dist/<ver>/abi/`）。

## 备注

- Simulate-style Web demo / 下游使用方：https://github.com/lshdlut/mujoco-wasm-play

## 致谢

本项目受到早期 MuJoCo→WASM 试验的启发，感谢他们验证可行性并总结经验：

- [stillonearth/MuJoCo-WASM](https://github.com/stillonearth/MuJoCo-WASM)
- [zalo/mujoco_wasm](https://github.com/zalo/mujoco_wasm)
- [hashb/mujoco_web](https://github.com/hashb/mujoco_web)

## 源起

仓库部分脚本和文档在生成阶段使用了生成式 AI，并由维护者复核。
