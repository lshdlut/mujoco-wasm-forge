# Changelog

All notable changes to this project will be documented in this file.

## forge-3.2.5-r1

- First release of MuJoCo 3.2.5 to WASM artifacts
- Toolchain
  - Emscripten: 3.1.55
  - Node (CI): 20
  - Link flags (subset): `-sMODULARIZE -sEXPORT_ES6 -sENVIRONMENT=node,web -sALLOW_MEMORY_GROWTH`
- Exports
  - C: `_mjwf_init, _mjwf_step_demo, _mjwf_nq, _mjwf_qpos0, _mjwf_qvel0, _mjwf_term, _malloc, _free`
  - Runtime: `cwrap, ccall`
- Tests
  - Smoke (Node): loads a minimal pendulum XML, steps 200, finite-state check
  - Regression: WASM vs native harness on identical XML for N steps (see CI logs)
- Artifacts
- `dist/3.2.5/mujoco.{js,wasm[,wasm.map]}`
  - `dist/version.json` (contains sizes + sha256)
  - `dist/sbom.spdx.json`
