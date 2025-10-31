# Structure & Conventions

- Must-keep directories: `.github`, `wrappers`, `scripts`, `tests`, `docs`.
- Must-ignore directories: `build/**`, `dist/**`, `external/**`, `local_tools/**` and common temp patterns (`tmp_*`, `*.tmp`, `coverage/**`).
- Single source of truth:
  - Generator: `wrappers/**/codegen/gen_exports.py`
  - Specs: `wrappers/**/codegen/spec_*.yaml`
  - ABI factory: `scripts/mujoco_abi/*` (scan, diff, gate, export generation)
- Internal/experimental docs and scratch files live under `local_tools/` and are ignored; public docs stay under `docs/`.

Compatibility & naming
- Unified handle prefix: `mjwf_`.
- Legacy `*_local` names may temporarily remain as thin forwarders; CI must not reference them.

Front-end demo
- On-going. Repo: https://github.com/lshdlut/mujoco-wasm-play.git

