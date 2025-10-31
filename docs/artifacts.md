# Artifacts & Metadata

Artifacts (per-version directory)
- `dist/<mjVer>/mujoco.wasm` - WebAssembly binary
- `dist/<mjVer>/mujoco.js` - ES module factory
- `dist/<mjVer>/mujoco.wasm.map` - optional source map
- `dist/<mjVer>/version.json` - build metadata
- `dist/<mjVer>/sbom.spdx.json` - SPDX SBOM

version.json schema (current)
- Fields: `mujocoVersion`, `emscripten`, `buildTime`, `gitSha`
- Provenance: `emsdk_root`, `emsdk_node`, `emsdk_python`, and `flags`
- Blocks: `features`, `size`, `hash`

Notes
- Schema is unified across 3.2.5 and 3.3.7. Provenance fields are present in CI builds and in canonical local builds when metadata is enabled.
