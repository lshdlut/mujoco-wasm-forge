# mujoco-wasm-forge

Reproducible build pipeline for compiling MuJoCo to WebAssembly. This repo focuses on producing versioned WASM artifacts from MuJoCo tags, together with minimal smoke/regression validation and machine-readable metadata.

This repository is a fork/continuation of MuJoCo WASM efforts by stillonearth, zalo, and hashb. We acknowledge and build upon their ideas and prior work in the MuJoCo → WebAssembly space.

• Input: MuJoCo tag (e.g., 3.2.5, 3.3.7)
• Output: `dist/mujoco-{version}.{js,wasm[,wasm.map]}`, `dist/version.json`, `dist/sbom.spdx.json`
• Toolchain: pinned Emscripten; minimal exports; no rendering UI here

Roadmap and internal development notes are maintained locally and are intentionally not included in the repository history.

Status: usable. CI builds a single-module WASM + glue JS, runs Node smoke + a native-vs-wasm regression, and uploads artifacts.

Repository: https://github.com/lshdlut/mujoco-wasm-forge

## Artifacts

After a successful build (locally or in CI), you get:

- `dist/mujoco-<mjVer>.wasm` — WebAssembly binary
- `dist/mujoco-<mjVer>.js` — ES module factory (`createMuJoCo`)
- `dist/mujoco-<mjVer>.wasm.map` — source map (optional)
- `dist/version.json` — metadata (MuJoCo tag, emscripten ver, sizes, sha256, git sha)
- `dist/sbom.spdx.json` — SPDX SBOM (lightweight)

## Quick start (Node ESM)

```
import createMuJoCo from './mujoco-3.2.5.js';

const Module = await createMuJoCo({
  locateFile: (p) => (p.endsWith('.wasm') ? './mujoco-3.2.5.wasm' : p),
});

// Minimal pendulum XML
const xml = `<?xml version="1.0"?>\n<mujoco model="pendulum">\n  <option timestep="0.002" gravity="0 0 -9.81"/>\n  <worldbody>\n    <body name="link" pos="0 0 0.1">\n      <joint name="hinge" type="hinge" axis="0 1 0" damping="0.01"/>\n      <geom type="capsule" fromto="0 0 0 0 0 0.2" size="0.02" density="1000"/>\n    </body>\n  </worldbody>\n</mujoco>`;

Module.FS.writeFile('/model.xml', new TextEncoder().encode(xml));
const init = Module.cwrap('mjw_init','number',['string']);
const step = Module.cwrap('mjw_step_demo', null, ['number']);
const qpos0 = Module.cwrap('mjw_qpos0','number',[]);

if (init('/model.xml') !== 1) throw new Error('init failed');
const before = qpos0();
step(200);
const after = qpos0();
console.log({ before, after });
```

## CI and reproducibility

Workflow `forge-325` (GitHub Actions):

- Pin emsdk (3.1.55) and Node (20)
- Fetch upstream MuJoCo tag (3.2.5)
- Build WASM module and a small native harness
- Run smoke test (Node ESM)
- Run regression: WASM vs native harness on the same XML for N steps
- Upload `dist/` as artifact + generate `version.json` and `sbom.spdx.json`

Quality gates (size/init-time) run in “warn” mode (not gating) for the first release.

## Versioning and tags

- Releases are tagged `forge-<mujocoVersion>-r<rev>`; example: `forge-3.2.5-r1`.
- Pre-releases use `forge-<mujocoVersion>-rc.<n>` and are marked as prerelease.
- Artifacts are immutable; fixes publish a new revision (e.g., `-r2`).

## Regression baseline

- Baseline: `native-3.2.5` ↔ `wasm-3.2.5` using the minimal pendulum XML.
- Determinism: fixed timestep, no randomization, warmstart disabled.

## Building locally

Prereqs: Emscripten SDK (3.1.55), CMake, Node 20.

```
git clone https://github.com/google-deepmind/mujoco external/mujoco -b v3.2.5 --depth=1
emcmake cmake -S wrappers/official_app_325 -B build/325 -DCMAKE_BUILD_TYPE=Release \
  -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF
cmake --build build/325 -j
```

Artifacts will be placed under `dist/` by the workflow; for local runs, copy from `build/325/_wasm/`.

## Notes

- This repo does not ship a front-end demo; for UI demos consider the maintainer’s fork: https://github.com/lshdlut/mujoco_webfork
- Local tools, caches, and experimental scripts are ignored via `.gitignore` by design.
