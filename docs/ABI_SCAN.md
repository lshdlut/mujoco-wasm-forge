ABI Scan (MuJoCo)

Purpose
- Generate versioned ABI metadata from upstream MuJoCo headers to support Web/WASM bindings and acceptance probes.
- Produce JSON artifacts with file+line evidence for CI diffing and audits.

Outputs
- `functions.json` - `MJAPI`-exported functions (from `mujoco.h`) with groups.
- `structs.json` - `mjModel`/`mjData` fields with basic shape hints.
- `enums.json` - public enums under `include/mujoco/`.
- `mappings.json` - name/id mapping functions, object types, topology field candidates.
- `diagnostics.json` - error/warning hooks and version/layout symbols.
- `gate.json` - exposure policy: required/optional/excluded.
- `dim_map.json` - inferred count->array relationships (e.g., `nq -> qpos[nq]`).
- `extensions.json` - `mjplugin.h`/`mjspec.h` related enums/functions (tagged caution).
- `probe_spec.json` - minimal probe plan for Web acceptance.

Run
- Default (3.3.7):
  - `pwsh scripts/mujoco_abi/run.ps1`
- Custom ref:
  - `pwsh scripts/mujoco_abi/run.ps1 -Ref 3.2.5 -OutDir dist/3.2.5/abi`
- Custom repo path:
  - `pwsh scripts/mujoco_abi/run.ps1 -Repo local_tools/mujoco`

Notes
- The scanner reads files via `git show <ref>:<path>` and never checks out or mutates the local clone.
- Parsing is heuristic-first and intentionally conservative; refine mappings/grouping as needed.
- Place upstream clone under `local_tools/mujoco` (ignored by VCS) or pass a custom path.
- Grouping uses `scripts/mujoco_abi/functions_map.json` when present; otherwise falls back to heuristics.
- Exposure tagging uses `scripts/mujoco_abi/exposure_map.json` when present.

Gate Policy (summary)
- Required: load/teardown, step/forward/reset/time, name mapping, VFS, errors/version/layout.
- Optional: inverse/sub-stages, ray casting, mjspec, solver stats.
- Excluded: native viz/UI (`mjv/mjr/mjui`), threading (`mjthread`).

Diff
- Generate another version (e.g., `3.3.6`), then run:
  - `node scripts/mujoco_abi/diff.mjs dist/3.3.6/abi dist/3.3.7/abi`
- Produces `diff_report.json` with summary markdown and gate checks.

Wrapper export workflow
- Wrapper header (`wrappers/official_app_337/include/mjwf_exports.h`) is the single source of truth for the exported surface.
- After running the ABI scan, generate the wrapper whitelist + d.ts:
  - `node scripts/mujoco_abi/gen_exports_from_abi.mjs dist/3.3.7/abi --header wrappers/official_app_337/include/mjwf_exports.h --version 3.3.7`
  - Output: `build/exports_3.3.7.json`, `build/types_3.3.7.d.ts`, and `dist/3.3.7/abi/wrapper_exports.json`.
- CMake consumes `export_*.json` via `-DMJWF_EXPORTS_JSON=...` and emits `-sEXPORTED_FUNCTIONS=[...]`; runtime helpers (malloc, memset, stack helpers) are appended automatically.
- Post-build verification (hard gate):
  - `node scripts/mujoco_abi/check_exports.mjs dist/3.3.7/abi dist/3.3.7/mujoco.wasm dist/3.3.7/abi/wrapper_exports.json`
  - Fails when required symbols are missing or unexpected exports leak past the whitelist.

WSL mirroring hints
- Prefer `pwsh local_tools/wsl/run.ps1 -Sync` to mirror the Windows workspace into `~/dev/mujoco-wasm-forge`; the script now excludes `.git` and purges stale `?root?dev?...` folders automatically.
- Avoid mixing Windows-native `Copy-Item`/`cp` with absolute WSL paths (e.g. `/root/...`); use `wsl.exe bash -lc 'cp ...'` instead to prevent Windows from materialising question-mark directories.
