# mujoco-wasm-forge

Standardized, reproducible build pipeline for compiling MuJoCo to WebAssembly. This repository focuses solely on producing versioned WASM artifacts from MuJoCo tags.

This repository is a fork/continuation of MuJoCo WASM efforts by stillonearth, zalo, and hashb. We acknowledge and build upon their ideas and prior work in the MuJoCo → WebAssembly space.

- Input: MuJoCo tag (e.g., v3.2.5, v3.3.7)
- Output: dist/mujoco-{version}.{js,wasm,wasm.map}, dist/version.json
- Fixed toolchain: pinned Emscripten; Qhull/LFS disabled; libccd static; no rendering

Roadmap and internal development notes are maintained locally and are intentionally not included in the repository history.

Status: experimental. The initial content migrates the ad-hoc scripts from sik-wasm/local_tools/mujoco_web_research into this repo.

GitHub: https://github.com/lshdlut/mujoco-wasm-forge
