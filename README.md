# mujoco-wasm-forge

English | [README.zh-CN.md](README.zh-CN.md)

mujoco-wasm-forge is a reproducible build harness that turns MuJoCo releases into WebAssembly bundles.
It scans upstream headers, emits wrappers and exports, compiles both WASM and native comparison binaries, runs smoke/regression checks, and publishes versioned artifacts with metadata. The GitHub Actions workflow mirrors this sequence.

- Input: MuJoCo tag (currently 3.2.5, 3.3.7, and 3.3.8-alpha)
- Output: `dist/<version>/{mujoco.js, mujoco.wasm[, mujoco.wasm.map], version.json, sbom.spdx.json}`
- Toolchain: emsdk 4.0.10 and Node 20 (parity with CI)
- Scope: simulation core only (visualization/UI families are intentionally excluded)

### Investigations

- We maintain an ongoing comparison between the forge wrapper and the official Embind-based WASM path. Findings, build parity notes, and benchmark methodology are recorded in `docs/wasm-investigation.md`.
- We keep two flows in parallel: a lean, customizable forge wrapper and the official Embind build (feature-rich, broadly compatible). See the investigation doc for rationale and commands.

Repository mirror: https://github.com/lshdlut/mujoco-wasm-forge

### Upstream WASM (official Embind)

Google DeepMind maintains official MuJoCo WebAssembly bindings via Embind (see issue #2585 and commit 40862617). This repository aligns naming and exposure philosophy with the official Embind surface: favor broad, direct exposure and pointer-first access for struct fields, while continuing to produce multi-version builds.

## Exposure policy (ABI summary)

- Align with official Embind: broad, direct exposure of the public C API and pointer-first access to struct fields.
- Visualization/UI families (`mjv/mjr/mjui`) remain out of scope; simulation core stays in.
- Variadics: export `*_v` variants when available; otherwise omit the variadic form.
- Reports live under `dist/<ver>/abi/`.

## Artifacts

Both CI and canonical local builds produce:

- `dist/<mjVer>/mujoco.wasm` – WebAssembly binary
- `dist/<mjVer>/mujoco.js` – ES module factory (`createMuJoCo`)
- `dist/<mjVer>/mujoco.wasm.map` – optional source map
- `dist/<mjVer>/version.json` – metadata (MuJoCo tag, emsdk, sizes, sha256, git sha)
- `dist/<mjVer>/sbom.spdx.json` – SPDX SBOM (lightweight)

## Quick start (Node ESM)

```ts
import createMuJoCo from './dist/3.2.5/mujoco.js';

const Module = await createMuJoCo({
  locateFile: (p) => (p.endsWith('.wasm') ? './dist/3.2.5/mujoco.wasm' : p),
});

// Minimal pendulum XML
const xml = `<?xml version="1.0"?>\n<mujoco model="pendulum">\n  <option timestep="0.002" gravity="0 0 -9.81"/>\n  <worldbody>\n    <body name="link" pos="0 0 0.1">\n      <joint name="hinge" type="hinge" axis="0 1 0" damping="0.01"/>\n      <geom type="capsule" fromto="0 0 0 0 0 0.2" size="0.02" density="1000"/>\n    </body>\n  </worldbody>\n</mujoco>`;

const parseXMLString = Module.cwrap('mjwf_mj_parseXMLString', 'number', ['string', 'number', 'number', 'number']);
const compile = Module.cwrap('mjwf_mj_compile', 'number', ['number', 'number']);
const deleteSpec = Module.cwrap('mjwf_mj_deleteSpec', null, ['number']);
const makeData = Module.cwrap('mjwf_mj_makeData', 'number', ['number']);
const resetData = Module.cwrap('mjwf_mj_resetData', null, ['number', 'number']);
const step = Module.cwrap('mjwf_mj_step', null, ['number', 'number']);
const deleteData = Module.cwrap('mjwf_mj_deleteData', null, ['number']);
const deleteModel = Module.cwrap('mjwf_mj_deleteModel', null, ['number']);

const stackTop = Module.stackSave();
const errBufSize = 1024;
const errBuf = Module.stackAlloc(errBufSize);
Module.HEAP8.fill(0, errBuf, errBuf + errBufSize);

const specPtr = parseXMLString(xml, 0, errBuf, errBufSize);
if (!specPtr) throw new Error(`parse failed: ${Module.UTF8ToString(errBuf)}`);

const modelPtr = compile(specPtr, 0);
if (!modelPtr) throw new Error('compile failed');
deleteSpec(specPtr);

const dataPtr = makeData(modelPtr);
if (!dataPtr) throw new Error('makeData failed');

resetData(modelPtr, dataPtr);
step(modelPtr, dataPtr);

deleteData(dataPtr);
deleteModel(modelPtr);
Module.stackRestore(stackTop);
```

## CI and reproducibility

Single workflow: `.github/workflows/forge.yml`

- Matrix covers MuJoCo 3.2.5, 3.3.7, and 3.3.8-alpha
- Toolchain pinned to emsdk 4.0.10 + Node 20
- 3.3.7 and 3.3.8-alpha perform a two-stage configure and force qhull to static (Emscripten requirement)
- Checks: Type surface regeneration (DTS) and runtime smoke/regression/mesh (RUN)
- Artifacts uploaded directly from `dist/<mjVer>/`

### ABI-driven pipeline (per build, introspect-based)

The current pipeline is driven entirely by MuJoCo's official introspect tables:

1. `python introspect/forge/scan_clang_introspect.py` – run MuJoCo's codegen to obtain FUNCTIONS / STRUCTS / ENUMS (JSON under `dist/<ver>/abi`).
2. `node abi_impl/nm_coverage.mjs build/<short>/lib/libmujoco.a --out dist/<ver>/abi/nm_symbols.json` – collect implementation symbols (B-set).
3. `python abi_exports/gen_structs.py` – generate struct-related exports `mjwf_abi_structs.{h,c}` and `mjwf_abi_structs.lst` from `structs_introspect_like.json`.
4. `python abi_exports/gen_funcs.py` – generate `_mjwf_*` function wrappers (`mjwf_abi_funcs.{h,c}`) and `wrapper_exports_funcs.json` / `exports_report_funcs.md` from `functions_introspect_like.json` + `nm_symbols.json`.
5. `python abi_exports/gen_enums.py` – flatten enums to `enums.json` for JS/TS consumers.

Emscripten then consumes the generated export lists (`wrapper_exports*.json` / `exports_*.lst`) via `-sEXPORTED_FUNCTIONS=@...`. See `docs/ABI_SCAN.md` for more details.

## Building locally (canonical flow)

Preferred environment: WSL Ubuntu 22.04 (or Docker) mirroring the GitHub Actions workflow.

1. Mirror & build (Windows host):
   ```powershell
   pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass `
     -File local_tools/wsl/run.ps1 -Sync -Clean -Meta -PinNode20 -UseTemp -Jobs 6
   ```
   The helper builds 3.2.5, 3.3.7, and 3.3.8-alpha by default; override with `-Targets '337'` or similar when iterating.
   For incremental builds (already mirrored), drop `-Sync` and `-UseTemp` if not needed.

2. Generate ABI descriptors (must run before post_build, new introspect flow):
   ```powershell
  python introspect/forge/scan_clang_introspect.py --header external/mujoco/include/mujoco/mujoco.h --out-dir dist/3.3.7/abi
  python abi_exports/gen_enums.py
   ```

3. Run post-build checks inside WSL (after `build.sh`):
   ```bash
   source /root/emsdk/emsdk_env.sh >/dev/null 2>&1
   ./check/post_build.sh --version 3.2.5 --short 325
   ./check/post_build.sh --version 3.3.7 --short 337
   ./check/post_build.sh --version 3.3.8-alpha --short 338
   ```

Notes:

- Always run builds on WSL ext4 (e.g., `~/dev/mujoco-wasm-forge`) or use `-UseTemp` to build under `/tmp`. Avoid `/mnt/c/...` / OneDrive to prevent I/O issues.
- Starting from a fresh workspace (`-UseTemp` or clean clone) best mirrors CI.
- Default parallelism is 6 (`-Jobs` overrides).
- The sync helper excludes `.git` and cleans stray `?root?...` folders; prefer it over manual copying.

## Using artifacts in other projects

- After building, copy only `dist/<mjVer>/` into the consumer repo.
- Load `dist/<mjVer>/mujoco.{js,wasm}` directly (avoid copying `build/` or `external/`).
- 3.3.7 enforces static qhull; artifacts already reflect that configuration.
- With `-Meta`/`META=1`, metadata files accompany the JS/WASM output.

### Allocating temporary buffers

Forge modules export both the engine stack helpers (`stackSave/stackAlloc/stackRestore`) and Emscripten's general-purpose allocator (`_malloc/_free/_realloc`):

- Use `stackSave` + `stackAlloc` + `stackRestore` for short-lived temporaries inside a single call (e.g., parsing errors, scratch strings).
- Use `_malloc/_free` for scratch that must persist across frames or be reused (e.g., the 6×`mjtNum` buffer required by `mj_contactForce`/`mj_contactTorque`).

Example (mjtNum is `double`, so 8 bytes):

```ts
const scratchBytes = 6 * 8;
const scratchPtr = Module._malloc(scratchBytes);
Module._mjwf_mj_contactForce(modelPtr, dataPtr, contactIndex, scratchPtr);
const forces = Module.HEAPF64.subarray(scratchPtr / 8, scratchPtr / 8 + 6);
// ... consume forces ...
Module._free(scratchPtr);
```

This pattern mirrors what Embind-based builds expose, so downstream renderers can rely on it without resorting to MuJoCo's internal stack APIs.

## Versioning

- Stable tags: `forge-<mujocoVersion>-r<rev>` (e.g., `forge-3.3.7-r2`)
- Pre-release tags: `forge-<mujocoVersion>-rc.<n>`
- Artifacts are immutable; fixes publish a new revision (increment `-rN`)

## Notes

- Front-end demo (work in progress): https://github.com/lshdlut/mujoco-wasm-play

## Acknowledgements

This project draws inspiration from earlier MuJoCo-to-WASM experiments that proved the concept and documented key pitfalls:

- stillonearth/MuJoCo-WASM
- zalo/mujoco_wasm
- hashb/mujoco_web

While mujoco-wasm-forge has since evolved into an independent toolchain, we remain grateful for their pioneering work.

## Provenance

Portions of this repository's scripts and documentation were authored or refined with the help of generative AI, then reviewed by a human maintainer.
