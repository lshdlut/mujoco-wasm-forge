# Quickstart

This page shows the shortest path to a working `dist/<ver>/mujoco.js` + `dist/<ver>/mujoco.wasm`.

## Prerequisites

- Linux/WSL, or Windows with a configured Emscripten/Bash/CMake environment for the build.
- Python 3.
- Node.js ≥ 20.
- Emscripten SDK (emsdk) 4.0.10 (or a compatible version), with `EMSDK` pointing to the emsdk directory.
- CMake (invoked via `emcmake`).
- Ninja (required by the current recipe; install it or set `MJWF_NINJA` to its executable path).

## Build (single command)

Run this from a current, local, non-OneDrive development checkout. For example, on
Windows use a fresh checkout such as `C:\dev\mujoco-wasm-forge\build-checkout`:

```powershell
git clone https://github.com/lshdlut/mujoco-wasm-forge.git C:\dev\mujoco-wasm-forge\build-checkout
```

The synced source checkout is for source and committed deliverables, and
`forge_cli.py build` refuses to run from OneDrive.

From that development checkout:

```bash
python3 forge_cli.py build --version 3.15.0 --with-checks
```

If you want build trees in another local directory, set `MJWF_BUILD_ROOT`:

```bash
MJWF_BUILD_ROOT=/tmp/mjwf_build python3 forge_cli.py build --version 3.15.0 --with-checks
```

## Find the outputs

- Primary artifacts:
  - `dist/3.15.0/mujoco.js`
  - `dist/3.15.0/mujoco.wasm`
- ABI/audit artifacts:
  - `dist/3.15.0/abi/` (export list, introspection JSON, symbol inventory, reports)

## Next

- If you need another version/ref: `howto_build_version`.
- If you want to consume it in a worker or Node: `howto_web_worker` / `howto_node`.
