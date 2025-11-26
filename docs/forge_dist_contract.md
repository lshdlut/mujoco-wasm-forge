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

Forge follows **Mode 1** for public distribution: the `dist/<ver>/` tree is committed into the git history for release tags such as `forge-3.3.7-r1`. CI builds a fresh copy in a clean environment and verifies that it matches the committed `dist/<ver>/`, but CI-built artifacts are not treated as a separate source of truth.

Canonical hosting for external consumers (including `mujoco-wasm-play`) is via the tagged git tree mirrored on CDNs such as jsDelivr:

- Raw GitHub:  
  `https://raw.githubusercontent.com/<owner>/mujoco-wasm-forge/<tag>/dist/<ver>/mujoco.wasm`  
  `https://raw.githubusercontent.com/<owner>/mujoco-wasm-forge/<tag>/dist/<ver>/mujoco.js`
- jsDelivr:  
  `https://cdn.jsdelivr.net/gh/<owner>/mujoco-wasm-forge@<tag>/dist/<ver>/mujoco.wasm`  
  `https://cdn.jsdelivr.net/gh/<owner>/mujoco-wasm-forge@<tag>/dist/<ver>/mujoco.js`

Template used by downstream consumers that know `<ver>` and `<tag>` (e.g. `forge-3.3.7-r1`):

- `forgeBase = "https://cdn.jsdelivr.net/gh/<owner>/mujoco-wasm-forge@forge-{ver}-r1/dist/{ver}/";`

GitHub Releases may additionally attach zip archives containing the same `dist/<ver>/` contents as a convenience, but the tagged git tree remains the authoritative source for public URLs.

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

## Maintainer flow and CI verify (Mode 1)

This section describes how forge maintainers build and publish `dist/<ver>/` while keeping CI as a reproducibility gate only.

### Local release flow

For a given MuJoCo version `<ver>` (e.g. `3.3.7`):

1. Make sure you are on WSL / Linux with emsdk available (see `README.md` for environment notes).
2. (Optional) Install Node dependencies once per workspace (currently there are no external deps, but this keeps the flow future-proof):  
   `npm install`
3. Build the canonical dist tree for the target version:  
   `npm run build:forge -- <ver>`  
   Examples:  
   - `npm run build:forge -- 3.2.5`  
   - `npm run build:forge -- 3.3.7`  
   - `npm run build:forge -- 3.3.8-alpha`
4. Inspect `dist/<ver>/` locally (at minimum `mujoco.{js,wasm}`, `version.json`, `sbom.spdx.json`, `SHA256SUMS.txt`, `RELEASE_NOTES.md`, and `abi/*`).
5. Commit the new dist tree:  
   `git add dist/<ver>`  
   `git commit -m "Publish dist/<ver> for forge-<ver>-rN"`
6. Tag the release revision and push:  
   `git tag forge-<ver>-rN` (e.g. `forge-3.3.7-r1`)  
   `git push && git push --tags`

From this point, CDNs such as jsDelivr can serve the committed `dist/<ver>/` directly using the patterns above.

### CI’s role

The workflow `.github/workflows/forge-dist-verify.yml` keeps `dist/<ver>/` reproducible without publishing its own artifacts:

- On `pull_request` to `main`/`master`, on `push` to those branches, and on `forge-*` tags, CI checks whether any `dist/<ver>/` directories are present in the tree.
- For each known MuJoCo version (`3.2.5`, `3.3.7`, `3.3.8-alpha`) where `dist/<ver>/` exists, CI:
  - Checks out a fresh copy of the repo into `ci-build/`.
  - Runs `npm ci` and then `npm run build:forge -- <ver>` in that clean workspace.
  - Compares `dist/<ver>/` (committed) against `ci-build/dist/<ver>/` using a recursive `diff`.
- Any difference between the committed dist and the CI rebuild causes the job to fail; matching trees pass.
- CI does not overwrite or publish `dist/<ver>/`; the git tree remains the single source of truth for public artifacts.

### Recommended stable version for consumers

For external consumers such as `mujoco-wasm-play`, the recommended stable version is:

- `ver = "3.3.7"`, tag `forge-3.3.7-r1`, dist base `dist/3.3.7/`.

Example jsDelivr template:

- `https://cdn.jsdelivr.net/gh/<owner>/mujoco-wasm-forge@forge-3.3.7-r1/dist/3.3.7/`
