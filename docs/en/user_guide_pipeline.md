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

If any stage fails, it usually means one of:

- environment/toolchain mismatch (Node/emcc/clang not available),
- upstream ref change requiring wrapper/introspection updates,
- export list drift (a previously exported function is missing or renamed),
- runtime regression caught by the smoke tests.

