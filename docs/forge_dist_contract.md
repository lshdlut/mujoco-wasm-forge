# Public dist layout & URL contract

This document describes the public `dist/<ver>/` layout and URL patterns for consumers such as `mujoco-wasm-play`.

## Versions, tags, and `dist/<ver>/`

- MuJoCo versions (ver): `3.2.5`, `3.3.7`, `3.3.8-alpha` (and future releases).
- Forge tags (git):  
  - Stable: `forge-<ver>-r<rev>` (e.g. `forge-3.3.7-r1`).  
  - Pre-release: `forge-<ver>-rc.<n>`.
- Dist directory: for a given MuJoCo version `<ver>`, all runtime artifacts live under:
  - `dist/<ver>/`

Example:

- Tag: `forge-3.3.7-r1`
- MuJoCo version: `3.3.7`
- Dist root in the tagged tree or release archive: `dist/3.3.7/`

## `dist/<ver>/` layout

For each `<ver>`, forge produces (at minimum):

- `dist/<ver>/mujoco.wasm`  
  - The WebAssembly module used by consumers.

- `dist/<ver>/mujoco.js`  
  - ES module / factory that loads `mujoco.wasm` (Emscripten `MODULARIZE` + `EXPORT_ES6`).

- `dist/<ver>/mujoco.wasm.map` (optional)  
  - Source map when debug/official profiles are enabled.

- `dist/<ver>/version.json`  
  - Build metadata. Current schema (see also `docs/artifacts.md`):
    - `mujocoVersion` – MuJoCo version string (e.g. `"3.3.7"`).  
    - `emscripten` – emsdk/emscripten version (e.g. `"4.0.10"`).  
    - `buildTime` – ISO8601 UTC build timestamp.  
    - `gitSha` – upstream MuJoCo commit used for the build.  
    - `emsdk_root`, `emsdk_node`, `emsdk_python` – provenance for the toolchain.  
    - `flags` – list of key compile-time flags (WASM, MODULARIZE, ES6, etc.).  
    - `features` – runtime feature block (qhull/libccd linkage, plugins/render, exceptions, filesystem, environment).  
    - `size` – `wasmBytes` and `jsBytes`.  
    - `hash` – SHA-256 of `mujoco.wasm` and `mujoco.js`.

- `dist/<ver>/sbom.spdx.json`  
  - SPDX document describing MuJoCo and emscripten components.

- `dist/<ver>/SHA256SUMS.txt`  
  - Text file with SHA-256 checksums for `mujoco.js` and `mujoco.wasm`.

- `dist/<ver>/RELEASE_NOTES.md`  
  - Short human-readable summary (emscripten version, sizes, basic quality gate status).

- `dist/<ver>/abi/`  
  - ABI metadata and export reports; intended for tools and consumers that need structural information (e.g. `mujoco-wasm-play`). Typical contents include:
    - `functions.json` – MuJoCo API functions grouped by purpose.  
    - `structs.json` – `mjModel` / `mjData` layouts and array fields.  
    - `enums.json` – public enums.  
    - `dim_map.json` – derived relationships between counts and array lengths.  
    - `mappings.json` – name/id mapping helpers and topology candidates.  
    - `diagnostics.json` – layout/ABI diagnostics.  
    - `extensions.json` – plugin-related symbols.  
    - `probe_spec.json` – minimal ABI probe plan for Web.  
    - `gate.json` – ABI gate thresholds and status.  
    - `mjapi.json` – header-based symbol inventory used for coverage checks.  
    - `nm_symbols.json`, `nm_coverage.json` – implementation-side symbol coverage.  
    - `wrapper_exports.json` – wrapper-driven export manifest (required/optional/runtime_keep sets).  
    - `exports_check.json`, `exports_report.md` – export coverage reports.
  - Consumers are expected to treat this directory as a structured-but-extensible surface: new files may appear, but existing filenames are kept stable.

### Environment maps (HDR)

Forge itself does not currently ship HDR environment maps under `dist/assets/env/`.  
Consumers that need skybox assets (e.g. `mujoco-wasm-play`) should:

- Either host HDRs in their own repo / CDN and reference them from the viewer.  
- Or agree on a future extension where forge publishes shared HDRs under:
  - `dist/assets/env/<name>.hdr`

If we standardise shared HDR assets in forge in the future, they will live under `dist/assets/env/` and be documented in this file.

## Hosting and public URLs

Forge’s CI builds the `dist/<ver>/` trees and attaches them to GitHub Releases as archives (e.g. `mujoco-3.3.7.zip`).  
There are two common hosting patterns for consumers:

1. **GitHub Releases (download first)**  
   - Download `mujoco-<ver>.zip` from the forge Release, extract `dist/<ver>/`, and serve it from your own static hosting (GitHub Pages, CDN, etc.).  
   - This is the most flexible option and does not require dist contents to be committed to the git tree.

2. **Static hosting from the git tree (for CDNs such as jsDelivr)**  
   - If a release tag includes `dist/<ver>/` in the git tree, then CDNs that mirror the repository (e.g. jsDelivr) can serve files directly.  
   - Canonical patterns in that case:
     - Raw GitHub:  
       `https://raw.githubusercontent.com/<owner>/mujoco-wasm-forge/<tag>/dist/<ver>/mujoco.wasm`  
       `https://raw.githubusercontent.com/<owner>/mujoco-wasm-forge/<tag>/dist/<ver>/mujoco.js`
     - jsDelivr:  
       `https://cdn.jsdelivr.net/gh/<owner>/mujoco-wasm-forge@<tag>/dist/<ver>/mujoco.wasm`  
       `https://cdn.jsdelivr.net/gh/<owner>/mujoco-wasm-forge@<tag>/dist/<ver>/mujoco.js`
   - Example template for consumers that know `<ver>` and `<tag>`:  
     `forgeBase = "https://cdn.jsdelivr.net/gh/<owner>/mujoco-wasm-forge@forge-{ver}-r1/dist/{ver}/";`

> Note: committing `dist/<ver>/` into the tagged tree is an explicit release decision, since `dist/` is ignored in normal development. For now, forge guarantees the relative layout under `dist/<ver>/`; how that tree is hosted (Release archives vs. committed files vs. a dedicated CDN repo) is up to the publisher.

### CORS and MIME

- GitHub’s raw endpoints and jsDelivr both serve `.wasm` files with `Content-Type: application/wasm` and standard CORS headers suitable for use from browsers.  
- Forge does not add any additional CORS logic; consumers are expected to host dist contents on origins that allow cross-origin fetches from their viewer (e.g. GitHub Pages).

## Helper snippets (example)

These are non-binding examples that downstream consumers can adapt.

### TypeScript helper

```ts
function normalizeVer(ver: string): string {
  return ver.replace(/^v/, "");
}

export function getForgeDistBase(ver: string, opts?: { owner?: string; rev?: string }): string {
  const v = normalizeVer(ver); // e.g. "3.3.7"
  const owner = opts?.owner ?? "<owner>";
  const rev = opts?.rev ?? "r1"; // forge-<ver>-r1, adjust as needed
  return `https://cdn.jsdelivr.net/gh/${owner}/mujoco-wasm-forge@forge-${v}-${rev}/dist/${v}/`;
}
```

### Version / tag mapping recap

- `ver = "3.3.7"` → tag `forge-3.3.7-r1` → dist base `dist/3.3.7/`.  
- `ver = "3.3.8-alpha"` → tag `forge-3.3.8-alpha-r1` (example) → dist base `dist/3.3.8-alpha/`.

Downstream projects such as `mujoco-wasm-play` can treat this document as the single source of truth for forge’s public dist layout and adjust their own `getForgeDistBase(ver)` helpers accordingly.

