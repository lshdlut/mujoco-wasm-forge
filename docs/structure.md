# Structure & Conventions

- Must-keep directories: `.github`, `wrappers`, `scripts`, `tests`, `docs`.
- Must-ignore directories: `build/**`, `dist/**`, `external/**`, `local_tools/**` and common temp patterns (`tmp_*`, `*.tmp`, `coverage/**`).
  - Note: run builds/tests inside WSL ext4 (e.g., `~/dev/mujoco-wasm-forge`). Avoid `/mnt/c/...` and OneDrive paths.
- Single source of truth:
  - Generator: `wrappers/**/codegen/gen_exports.py`
  - Specs: `wrappers/**/codegen/spec_*.yaml`
  - ABI tooling: `scripts/mujoco_abi/*` (scan, diff, export generation)
- Internal/experimental docs and scratch files live under `local_tools/` and are ignored; public docs stay under `docs/`.

Compatibility & naming
- Prefix: `mjwf_`; prefer names that encode the full path to the underlying field/function (e.g., `mjwf_model_opt_timestep_ptr`).
- Avoid temporary forwarders; prefer a single, direct name per surface.

Front-end demo
- On-going. Repo: https://github.com/lshdlut/mujoco-wasm-play.git

