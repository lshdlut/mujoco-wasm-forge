# Repo tour

## High-level layout

- `forge_cli.py`: the single entrypoint for local + CI builds.
- `app/`: CMake + C sources for the WASM wrapper build (links MuJoCo + generated wrappers).
- `introspect/`: clang-based header scanning and introspection glue.
- `abi_exports/`: generates wrappers, manifests, and `exports.lst`.
- `abi_impl/`: symbol inventory helpers (e.g. `nm` coverage).
- `check/`: post-build validation plus Node-based smoke and quality gates.
- `dist/`: committed build outputs (`dist/<ver>/...`).

## External sources

- `external/mujoco`: upstream MuJoCo checkout (not tracked by git in this repo; created/updated by the forge CLI).

