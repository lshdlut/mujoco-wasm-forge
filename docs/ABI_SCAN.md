ABI Scan (MuJoCo)

Purpose
- Generate versioned ABI metadata from upstream MuJoCo headers to support Web/WASM bindings and acceptance probes.
- Produce JSON artifacts with file+line evidence for CI diffing and audits.

Outputs (canonical, introspect-driven)
- `functions_introspect_like.json` – MuJoCo API functions with full type info (from official introspect).
- `structs_introspect_like.json` – `mjModel` / `mjData` / related structs with field types and extents.
- `enums_introspect_like.json` – enum declarations and values as seen by introspect.
- `functions_introspect_like.json` – function declaration A-set used for export coverage (derived from introspect).
- `nm_symbols.json`, `nm_coverage.json` – implementation-side symbol coverage (B-set).
- `wrapper_exports.json` – final export manifest for WASM (`_mjwf_*` + data ptrs + runtime_keep).
- `exports_report.md` / `exports_report_funcs.md` – A/B/C export coverage reports.
- `enums.json` – flattened enums table for JS/TS consumers (generated from `enums_introspect_like.json`).

Run (3.3.7 example)
- Generate introspect-based snapshots (FUNCTIONS / STRUCTS / ENUMS):
  - `python introspect/forge/scan_clang_introspect.py --header external/mujoco/include/mujoco/mujoco.h --out-dir dist/3.3.7/abi`
- Build implementation symbol set (B):
  - `node abi_impl/nm_coverage.mjs build/337/lib/libmujoco.a --out dist/3.3.7/abi/nm_symbols.json`
// Generate struct exports (data ptrs / dims / derived views):
//   - mjwf_abi_structs.{h,c} + mjwf_abi_structs.lst
  - `python abi_exports/gen_structs.py app/mjwf_abi_structs.h app/mjwf_abi_structs.c`
- Generate function wrappers + export manifests:
  - `python abi_exports/gen_funcs.py`
- Generate flattened enums table for JS/TS:
  - `python abi_exports/gen_enums.py`

Notes
- All ABI scanning rules (what to expose, how to parse types/arrays/nullable) come from MuJoCo's official introspect codegen; forge only performs Python→JSON translation and wrapper generation.
- Legacy header-parsing pipeline (`scan.mjs`, `run.ps1`, `functions.json`, `structs.json`, `dim_map.json`) is kept only for historical reference and should not be used for new flows.

Exposure Policy (aligned with official Embind)
- Favor broad, direct exposure matching upstream Embind naming and coverage.
- Prefer pointer-first access for struct fields to enable read/write from JS.
- Visualization/UI families (`mjv/mjr/mjui`) remain excluded to keep scope on simulation core.

Diff
- Generate another version (e.g., `3.3.6`), then run:
  - `node check/diff.mjs dist/3.3.6/abi dist/3.3.7/abi`
- Produces `diff_report.json` summarizing changes in functions/structs/enums.

Wrapper export overview
// Struct exports (C): `gen_structs.py` writes `mjwf_abi_structs.{h,c}` + `mjwf_abi_structs.lst` (flat ptr + dims + derived views).
// Function exports (C): `gen_funcs.py` reads `functions_introspect_like.json` + `nm_symbols.json`, applies prefix rules, and writes `mjwf_abi_funcs.{h,c}` + `wrapper_exports_funcs.json` / `exports_report_funcs.md`.
- Final export manifest: wrapper exports and runtime helpers are collected into `wrapper_exports.json` and `exports_*.{json,lst}` (used by Emscripten `-sEXPORTED_FUNCTIONS=@...`). Existing CMake targets still drive this step; new codegen uses introspect-based inputs.
WSL mirroring hints
- Prefer `pwsh local_tools/wsl/run.ps1 -Sync` to mirror the Windows workspace into `~/dev/mujoco-wasm-forge`; the script now excludes `.git` and purges stale `?root?dev?...` folders automatically.
- Avoid mixing Windows-native `Copy-Item`/`cp` with absolute WSL paths (e.g. `/root/...`); use `wsl.exe bash -lc 'cp ...'` instead to prevent Windows from materialising question-mark directories.

