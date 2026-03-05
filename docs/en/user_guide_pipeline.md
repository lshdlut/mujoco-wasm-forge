# Build pipeline (end-to-end)

The canonical entrypoint is:

```bash
python3 forge_cli.py build --version <mjver>
```

That command orchestrates the full pipeline for the requested version/ref:

1. **Prepare upstream sources**
   - Clone or update upstream MuJoCo under `external/mujoco`.
   - Check out the requested ref (e.g. `3.5.0` or a commit hash).
   - Apply build-system patches needed for Emscripten (e.g. qhull).
2. **Introspect headers**
   - Parse `mujoco.h` and produce canonical JSON under `dist/<ver>/abi/`:
     - `mujoco_ast.json`
     - `functions_introspect_like.json`
     - `structs_introspect_like.json`
     - `enums_introspect_like.json`
3. **Collect implemented symbols**
   - Create or refresh `nm_symbols.json` so the pipeline knows what MuJoCo actually implements for the build.
4. **Generate ABI and wrappers**
   - Generate wrapper sources and metadata (`mjwf_abi_funcs.*`, `mjwf_abi_structs.*`, wrapper export manifests).
   - Produce the final export list `dist/<ver>/abi/exports.lst`.
5. **Build WASM bundle**
   - Configure with `emcmake cmake` and build with `cmake --build`.
   - `app/CMakeLists.txt` consumes the generated wrapper sources and the export list, then emits:
     - `dist/<ver>/mujoco.js`
     - `dist/<ver>/mujoco.wasm`
6. **Post-build validation**
   - Validate exports/ABI manifests and (optionally) coverage against native symbols.
7. **Optional runtime checks (`--with-checks`)**
   - Run Node-based smoke + quality gates (`check/tests/*.mjs`) against the active `dist/<ver>`.

## Fast paths for iteration (avoid full rebuilds)

`python3 forge_cli.py build --version <mjver>` optimizes for correctness and produces a fully validated `dist/<ver>` tree.
For day-to-day editing you can often skip most stages and rebuild only what changed.

### 1) Docs / benchmarks / Node checks only

If you only changed documentation, benchmarks, or `check/tests/*.mjs`, you do **not** need to rebuild WASM. Re-run the
relevant Node scripts against an existing `dist/<ver>` tree.

### 2) Wrapper/app code only (`app/*`)

If you only changed wrapper sources under `app/` (or other non-upstream code consumed by the CMake build), rebuild the
existing build directory directly instead of rerunning the full pipeline.

- Build directory layout (default): `<repo>/build/forge/<short>/{single|pthreads}`
- If you set `MJWF_BUILD_ROOT`, it becomes: `<MJWF_BUILD_ROOT>/forge/<short>/{single|pthreads}`

Example (run in the same shell environment where `EMSDK` is available):

```bash
cmake --build "<build_dir>" -- -j "$(nproc)"
```

Running the full forge command will re-run upstream refresh + introspection + generators, and may force a rebuild even
when you only edited a couple of local files.

### 3) ABI generator scripts only (`abi_exports/*`)

If you only changed generator logic (for example `abi_exports/gen_funcs.py`), regenerate the ABI outputs and then build:

```bash
python3 -m abi_exports.gen_funcs --version <mjver>
cmake --build "<build_dir>" -- -j "$(nproc)"
```

If you changed other generators, rerun them as well (e.g. `abi_exports/gen_structs.py`, `abi_exports/gen_enums.py`,
`abi_exports/gen_scene_geom_soa.py`).

### 4) Upstream ref / `introspect/*` / MuJoCo patches

If you changed anything that affects upstream headers or ABI inputs, run the full pipeline:

```bash
python3 forge_cli.py build --version <mjver>
```

### Debug-friendly builds (when you need symbols/assertions)

`forge_cli.py build` configures with `-DCMAKE_BUILD_TYPE=Release` and `-DMJWF_PROFILE=fast` by default (see
`app/CMakeLists.txt`). If you need more debug visibility (assertions, source maps, etc.), reconfigure the existing build
directory with a different `MJWF_PROFILE` and rebuild via `cmake --build`. Note that debug profiles may increase build
time and output size.

If any stage fails, it usually means one of:

- environment/toolchain mismatch (Node/emcc/clang not available),
- upstream ref change requiring wrapper/introspection updates,
- export list drift (a previously exported function is missing or renamed),
- runtime regression caught by the smoke tests.
