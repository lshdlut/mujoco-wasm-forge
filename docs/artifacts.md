# Artifacts & Metadata

Artifacts
- `dist/mujoco-<mjVer>.wasm` — WebAssembly binary
- `dist/mujoco-<mjVer>.js` — ES module factory
- `dist/mujoco-<mjVer>.wasm.map` — optional source map
- `dist/version.json` — build metadata
- `dist/sbom.spdx.json` — SPDX SBOM

version.json schema (current)
- Fields: `mujocoVersion`, `emscripten`, `buildTime`, `gitSha`
- Provenance: `emsdk_root`, `emsdk_node`, `emsdk_python`, and `flags`
- Blocks: `features`, `size`, `hash`

Notes
- Schema is unified across 3.2.5 and 3.3.7. Provenance fields are present in CI builds.

