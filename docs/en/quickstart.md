# Quickstart

This page shows the shortest path to a working `dist/<ver>/mujoco.js` + `dist/<ver>/mujoco.wasm`.

## Prerequisites

- A Linux environment (WSL is recommended on Windows) for the Emscripten build.
- Python 3.
- Node.js ≥ 20.
- Emscripten SDK (emsdk) 4.0.10 (or a compatible version), with `EMSDK` pointing to the emsdk directory.
- CMake (invoked via `emcmake`).

## Build (single command)

From the repo root:

```bash
python3 forge_cli.py build --version 3.5.0 --with-checks
```

If you want build trees outside the repo (recommended on synced filesystems), set `MJWF_BUILD_ROOT`:

```bash
MJWF_BUILD_ROOT=/tmp/mjwf_build python3 forge_cli.py build --version 3.5.0 --with-checks
```

## Find the outputs

- Primary artifacts:
  - `dist/3.5.0/mujoco.js`
  - `dist/3.5.0/mujoco.wasm`
- ABI/audit artifacts:
  - `dist/3.5.0/abi/` (export list, introspection JSON, symbol inventory, reports)

## Next

- If you need another version/ref: `howto_build_version`.
- If you want to consume it in a worker or Node: `howto_web_worker` / `howto_node`.

