# Structure & Conventions

- Must-keep directories: `.github`, `wrappers`, `scripts`, `tests`, `codegen`, `docs`.
- Must-ignore directories: `build/**`, `dist/**`, `external/**`, `local_tools/**` and common temp patterns (`tmp_*`, `*.tmp`, `coverage/**`).
- Single source of truth:
  - Generator: `codegen/gen_exports_list.py`
  - Specs: `wrappers/**/codegen/spec_*.yaml`
- Internal/experimental docs and scratch files live under `local_tools/` and are ignored; public docs stay under `docs/`.

Compatibility & naming
- Unified handle prefix: `mjwf_`.
- Script entrypoints (recommended):
  - `scripts/symcheck_from_list.ps1`
  - `scripts/smoke/smoke.mjs`, `scripts/det/det.mjs`, `scripts/smoke/smoke_ext.mjs`
- Legacy `*_local` names may temporarily remain as thin forwarders; the CI must not reference them.

