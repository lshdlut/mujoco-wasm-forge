# 概览

## mujoco-wasm-forge 是什么？

`mujoco-wasm-forge` 是一套把指定的上游 MuJoCo ref 构建成 WebAssembly 的流水线，同时把“导出符号/封装层/ABI”
做成显式、可对比、可门控的产物。

目标不只是“能编成 WASM”，而是：

- 针对某个 ref 产出可预测的 `dist/<ver>/` 目录布局；
- 通过 `exports.lst` 让导出符号清单成为唯一真相，并在链接阶段强制执行；
- 产出 `dist/<ver>/abi/*` 作为升级审计的证据链；
- 通过 smoke + gates 让 CI 能尽早发现漂移与回归。

## 它保证什么（以及不保证什么）

**设计上要保证的**

- 单一入口：`forge_cli.py build` 可复现整条流水线。
- 显式导出清单：`dist/<ver>/abi/exports.lst` 参与链接，导出可审计。
- dist 布局一致：所有检查都消费同一个 `dist/<ver>`。
- 门控可本地复现：checks/gates 可在本地与 CI 运行。

**非目标**

- 不承诺“跨 MuJoCo 大版本 ABI 永不变化”（上游会变）。
- 不追求提供“高层易用的 JS 封装库”（核心产物是 Emscripten 的 `mujoco.js/.wasm` + wrappers）。
- 不承诺在任意工具链组合下字节级完全一致（CI 使用 `verify-dist` 做可复现性验证）。

## 关键术语

ABI、wrapper、`exports.lst`、introspect、gates 等术语见 `glossary`。

