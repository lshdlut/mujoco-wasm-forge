# mujoco-wasm-forge

English | [简体中文](README.zh-CN.md)

Reproducible build pipeline for compiling MuJoCo to WebAssembly. This repo focuses on producing versioned WASM artifacts from MuJoCo tags, together with minimal smoke/regression validation and machine-readable metadata.

This repository is a fork/continuation of MuJoCo to WebAssembly efforts by stillonearth, zalo, and hashb. We acknowledge and build upon their ideas and prior work in the MuJoCo to WebAssembly space.

- Input: MuJoCo tag (e.g., 3.2.5, 3.3.7)
- Output: `dist/<version>/{mujoco.js,mujoco.wasm[,mujoco.wasm.map],version.json,sbom.spdx.json}`
- Toolchain: pinned Emscripten; minimal exports; no rendering UI here

Status: usable. CI builds a single-module WASM + glue JS, runs Node smoke + a native-vs-wasm regression, and uploads artifacts.

Repository: https://github.com/lshdlut/mujoco-wasm-forge

## 导出规则与排除（必须阅读）

- 导出等式：**C = A ∩ B**  
  A=公开 C 头声明（mujoco.h / mjspec.h）  
  B=静态库实现（llvm-nm）
- 硬闸：`(A ∩ B) − C = ∅`，导出不得含 `mjv_/mjr_/mjui_` 或非 `_mjwf_*`。
- **特殊排除**：
  1) 仅接受 `mj_`、`mju_`、`mjs_` 前缀；  
  2) 变参函数仅在存在 `*_v` 变体时导出，无 `_v` 自动排除（记录为 `variadic_no_v`）。
- 详见 `dist/<ver>/abi/exports_report.md`（如需 JSON 报告，设环境变量 `EMIT_JSON=1`）。

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

Forge now treats auto-generated wrappers as the single source of truth for the WASM surface. Every CI/local build executes the following pipeline:

1. **扫描声明集 (A)**  
   `node scripts/mujoco_abi/autogen_wrappers.mjs --include external/mujoco/include --out build/<short>/mjapi_<ver>.json`  
   （由 CMake 自动触发，用于记录公开 C API）
2. **枚举实现集 (B)**  
   `node scripts/mujoco_abi/nm_coverage.mjs build/<short>/lib/libmujoco.a --out build/<short>/nm_<ver>.json`
3. **生成 C = A ∩ B 导出面**  
   `node scripts/mujoco_abi/gen_exports_from_abi.mjs --names-json build/<short>/mjapi_<ver>.json --impl-json build/<short>/nm_<ver>.json --header wrappers/auto/mjwf_auto_exports.h --source wrappers/auto/mjwf_auto_exports.c --version <ver> --out build/<short> --abi dist/<ver>/abi`  
   产出 `exports_<ver>.{json,lst,d.ts}`, `wrapper_exports.json` 以及 `exports_report.md`（`EMIT_JSON=1` 时附带 JSON 报告）。
4. **使用白名单编译**  
   CMake 注入自动生成的文件，并传递 `-sEXPORTED_FUNCTIONS=@build/<short>/exports_<ver>.lst`。
5. **导出面硬闸**  
   `node scripts/mujoco_abi/check_exports.mjs --abi dist/<ver>/abi --expected dist/<ver>/abi/wrapper_exports.json --wasm dist/<ver>/mujoco.wasm`
6. **可选审计快照**  
   `node scripts/mujoco_abi/nm_coverage.mjs build/<short>/lib/libmujoco.a --out dist/<ver>/abi/nm_coverage.json`

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




