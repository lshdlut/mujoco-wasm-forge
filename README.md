# mujoco-wasm-forge

English | [简体中文](README.zh-CN.md)

Reproducible build pipeline for compiling MuJoCo to WebAssembly. This repo focuses on producing versioned WASM artifacts from MuJoCo tags, together with minimal smoke/regression validation and machine-readable metadata.

This repository is a fork/continuation of MuJoCo to WebAssembly efforts by stillonearth, zalo, and hashb. We acknowledge and build upon their ideas and prior work in the MuJoCo to WebAssembly space.

- Input: MuJoCo tag (e.g., 3.2.5, 3.3.7)
- Output: `dist/<version>/{mujoco.js,mujoco.wasm[,mujoco.wasm.map],version.json,sbom.spdx.json}`
- Toolchain: pinned Emscripten; minimal exports; no rendering UI here

Status: usable. CI builds a single-module WASM + glue JS, runs Node smoke + a native-vs-wasm regression, and uploads artifacts.

Repository: https://github.com/lshdlut/mujoco-wasm-forge

## Artifacts

CI (and canonical local builds using WSL with metadata enabled) produce:

- `dist/<mjVer>/mujoco.wasm` - WebAssembly binary
- `dist/<mjVer>/mujoco.js` - ES module factory (`createMuJoCo`)
- `dist/<mjVer>/mujoco.wasm.map` - source map (optional)
- `dist/<mjVer>/version.json` - metadata (MuJoCo tag, emscripten version, sizes, sha256, git sha)
- `dist/<mjVer>/sbom.spdx.json` - SPDX SBOM (lightweight)

## Quick start (Node ESM)

```
import createMuJoCo from './dist/3.2.5/mujoco.js';

const Module = await createMuJoCo({
  locateFile: (p) => (p.endsWith('.wasm') ? './dist/3.2.5/mujoco.wasm' : p),
});

// Minimal pendulum XML
const xml = `<?xml version="1.0"?>\n<mujoco model="pendulum">\n  <option timestep="0.002" gravity="0 0 -9.81"/>\n  <worldbody>\n    <body name="link" pos="0 0 0.1">\n      <joint name="hinge" type="hinge" axis="0 1 0" damping="0.01"/>\n      <geom type="capsule" fromto="0 0 0 0 0 0.2" size="0.02" density="1000"/>\n    </body>\n  </worldbody>\n</mujoco>`;

Module.FS.writeFile('/model.xml', new TextEncoder().encode(xml));
const init = Module.cwrap('mjwf_init','number',['string']);
const step = Module.cwrap('mjwf_step_demo', null, ['number']);
const qpos0 = Module.cwrap('mjwf_qpos0','number',[]);

if (init('/model.xml') !== 1) throw new Error('init failed');
const before = qpos0();
step(200);
const after = qpos0();
console.log({ before, after });
```

## CI and reproducibility

Unified workflow entrance (GitHub Actions):

- Single entry `.github/workflows/forge.yml` with a version matrix (3.2.5, 3.3.7)
- Pinned emsdk (3.1.55) and Node (20)
- Two-stage configure for 3.3.7 to statically link qhull (replace SHARED->STATIC and force BUILD_SHARED_LIBS=OFF); 3.2.5 does not require the qhull patch
- Quality gates: [GATE:SYM] Sym-from-JSON, [GATE:DTS] d.ts drift, [GATE:RUN] runtime smoke/regression/mesh-smoke
  - If a gate is not yet implemented in code, the workflow logs it as `skipped` without changing behavior.
- Artifacts: upload `dist/` (+ `version.json`, `sbom.spdx.json`)

### ABI-driven export pipeline

Forge now treats the wrapper header as the single source of truth for the WASM surface. Every CI/local build runs the following steps before compilation:

1. **Scan upstream headers**  
   `pwsh scripts/mujoco_abi/run.ps1 -Ref 3.3.7 -OutDir dist/3.3.7/abi`
2. **Generate wrapper whitelist + d.ts**  
   `node scripts/mujoco_abi/gen_exports_from_abi.mjs dist/3.3.7/abi --header wrappers/official_app_337/include/mjwf_exports.h --version 3.3.7`
   - Produces `build/exports_3.3.7.json`, `build/types_3.3.7.d.ts`, and `dist/3.3.7/abi/wrapper_exports.json`.
3. **Build with whitelist**  
   CMake consumes `-DMJWF_EXPORTS_JSON=...` and emits `-sEXPORTED_FUNCTIONS=[...]` (runtime helpers auto-appended).
4. **Post-build export check (hard gate)**  
   `node scripts/mujoco_abi/check_exports.mjs dist/3.3.7/abi dist/3.3.7/mujoco.wasm dist/3.3.7/abi/wrapper_exports.json`
   - Fails when required symbols are missing or unexpected exports leak past the whitelist.

Refer to `docs/ABI_SCAN.md` for details and additional options.

## Versioning and tags

- Stable releases use `forge-<mujocoVersion>-r<rev>`; example: `forge-3.2.5-r3`, `forge-3.3.7-r2`.
- Pre-releases use `forge-<mujocoVersion>-rc.<n>` and are marked as pre-release.
- Artifacts are immutable; fixes publish a new revision (e.g., `-r2`).

### Release status (current)
- Recommended (stable):
  - `forge-3.2.5-r3`
  - `forge-3.3.7-r2`
- Deprecated (superseded by unified workflow):
  - `forge-3.2.5-r1`, `forge-3.2.5-r2`
  - `forge-3.3.7-r1`, all `forge-3.3.7-rc.*`

## Regression baseline

- Baselines: `native-3.2.5` <-> `wasm-3.2.5`, and `native-3.3.7` <-> `wasm-3.3.7`
- Determinism: fixed timestep, no randomization, warmstart disabled

## Building locally (canonical)

Preferred path: WSL Ubuntu 22.04 (or Docker) fully replicating `.github/workflows/forge.yml`.

- Windows entrypoint (from repo root):
  - First mirror repository into WSL ext4 and build:
    - `pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File local_tools/wsl/run.ps1 -Sync -Clean -Meta -PinNode20 -UseTemp -Jobs 6`
  - Subsequent builds (already mirrored):
    - `pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File local_tools/wsl/run.ps1 -Clean -Meta -PinNode20 -Jobs 6`
  - Flags:
    - `-Clean` removes old caches for a clean reconfigure
    - `-Meta` generates `version.json`, `sbom.spdx.json`, `SHA256SUMS.txt`, `RELEASE_NOTES.md`
    - `-PinNode20` attempts to run tests with Node 20 for CI parity
     - `-WslWorkDir` sets WSL workspace (default `~/dev/mujoco-wasm-forge`)

- WSL entrypoint (equivalent):
  - `CLEAN=1 META=1 PIN_NODE20=1 TARGETS=325,337 MJVER_337=3.3.7 MJVER_325=3.2.5 bash ./local_tools/wsl/build.sh`

Notes:
- Run all builds and tests inside WSL ext4 (e.g., `~/dev/mujoco-wasm-forge`), or use `-UseTemp` to build in `/tmp`. Avoid `/mnt/c/...` and OneDrive paths to prevent slow I/O and sync overhead. No files are written to Windows `~`.
- Default parallel jobs is 6; override with `-Jobs` if needed.
- The sync helper now excludes `.git` and purges accidental `?root?dev?...` folders that can appear if WSL paths are copied from Windows. Prefer `-Sync` over manual `cp /root/...` on Windows.

## Using artifacts in other repos (e.g., mujoco-wasm-play)

- Build here in WSL as above; artifacts land under `dist/<mjVer>/`.
- In WSL, copy only the needed artifacts to the play repo:
  - `cp -r ./dist/3.3.7 /path/to/mujoco-wasm-play/dist/3.3.7`
- In the play repo, load from `dist/<mjVer>/mujoco.{js,wasm}` in your loader; avoid copying `build/` or `external/`.
- For MuJoCo 3.3.7, the build performs a two-stage configuration to enforce static qhull under Emscripten (SHARED->STATIC, BUILD_SHARED_LIBS=OFF) before the final configure.
- Artifacts (including metadata) will be under `dist/<mjVer>/` when `-Meta`/`META=1` is enabled.

## Notes

- Front-end demo: on-going. Repo: https://github.com/lshdlut/mujoco-wasm-play.git

## Provenance

Portions of this repository's configurations, CI workflows, and documentation were authored or adapted with the assistance of generative AI, and were subsequently reviewed and validated by a human maintainer.




