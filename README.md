# mujoco-wasm-forge

English | [简体中文](README.zh-CN.md)

## Overview

mujoco-wasm-forge is a toolchain for producing MuJoCo WebAssembly builds and keeping their exports under control. To build a specific MuJoCo release, run `python forge_cli.py build --version <mjver>` from the repo root. The CLI prepares `dist/<ver>`, runs the introspect/ABI/export steps, then launches the Emscripten build and gate checks for that version. Helpers such as `dist_version.py` and `check/dist_paths.mjs` let every script discover the active `dist/<ver>` so the rest of the pipeline simply consumes the right artifacts.

To build the thread-enabled variant, pass `--pthreads`. The runtime artifacts are written under `dist/<ver>/pthreads/` (ABI stays shared under `dist/<ver>/abi/`).

## Automated run entrypoint

`forge_cli.py` sequences every stage of the pipeline for the requested version (`prepare` → `introspect` → ABI generation → `emcmake` build → `check/post_build.sh`), and can optionally run the smoke, mesh-smoke, and gates tests when invoked with `--with-checks`. Calling `python forge_cli.py build --version 3.3.7 --with-checks` (or whichever MuJoCo ref you are targeting) lets developers and CI targets reproduce the full, version-agnostic flow without touching any of the underlying scripts.

## Version switching

Changing the target MuJoCo build is driven by `python forge_cli.py build --version <mjver>`. The build command fetches or checks out the requested ref inside `external/mujoco`, rebuilds the introspect/ABI/export artifacts, and drops the generated WASM bundle and metadata under `dist/<ver>`. `dist_version.py`, `check/dist_paths.mjs`, and the scripts under `check/tests/` all read the active version from `MJVER`/`DIST_VERSION` or the populated `dist` folder, so every downstream gate just points at the same directory. For ad-hoc verification you can run `node check/tests/*.mjs` after a build and they will pick up whatever version lives in `dist/<ver>`.

### Design pillars

- **Automation-first** – The introspect scripts, ABI generators, and export checkers sequentially feed JSON artifacts into each other. There’s no manual editing of function lists; every stage regenerates its outputs based on the previous stage’s canonical JSON.
- **Version independence** – Because no file names or directories besides `dist/<ver>` mention 3.3.7, spinning up another MuJoCo release is as simple as setting `MJVER`/`DIST_VERSION` or pre-populating `dist/<ver>` before running the usual pipeline.
- **Flat exports (A∩B=C)** – `abi_exports/gen_funcs.py` produces the A-set (introspect declarations), B-set (`nm_symbols.json` implementations), and C-set (wrappers) plus `exports.lst`. That list becomes the sole input to `-sEXPORTED_FUNCTIONS=@dist/<ver>/abi/exports.lst`, giving us a completely transparent view of exported symbols.
- **ABI gating** – `check/post_build.sh`, `check/check_exports.mjs`, and the smoke/mesh/gates scripts all point at the same manifest, making it easy to spot added/removed exports or ABI drift.

## Workflow highlights

- **Build** – The WASM bundle is compiled inside WSL using Node ≥20 and emsdk 4.0.10. `app/CMakeLists.txt` links the MuJoCo sources with the generated wrappers, restricts the exported symbols via `exports.lst`, and emits `dist/<ver>/mujoco.js` and `.wasm`.
- **Introspect** – `introspect/forge/scan_clang_introspect.py` dumps clang’s AST for `external/mujoco/include/mujoco/mujoco.h`, runs the official codegen for functions/structs/enums, and converts their outputs into JSON under `dist/<ver>/abi/`.
- **ABI implementation** – `abi_exports/gen_structs.py` consumes `structs_introspect_like.json` to emit `mjwf_abi_structs.*` plus `mjwf_abi_structs.lst`. `abi_exports/gen_funcs.py` merges introspect declarations, NM symbols, and extra exports to produce `mjwf_abi_funcs.*`, `wrapper_exports_funcs.json`, and `exports.lst`.
- **ABI exports** – `check/post_build.sh` validates `wrapper_exports.json`, `exports_check.json`, and optional `nm_coverage.json` against the native build before continuing. Those manifests feed `check/check_exports.mjs` so the gate logic understands the exact A/B/C sets.
- **App** – The `app/` directory houses the generated wrappers, helper stubs, and `MJWF_PROFILE` configuration. CMake combines them with MuJoCo to produce `_wasm/mjwasm_forge.js`, exposing the `mjwf_*` API surface.
- **Check** – `check/tests/smoke.mjs`, `check/tests/mesh-smoke.mjs`, and `check/tests/gates.mjs` read `distDir()`/`distVersion()` directly, so CI can rerun these scripts after every build to verify exports, smoke/regression flows, and quality gates regardless of the version tag.

## Notes

- Front-end demo (work in progress): https://github.com/lshdlut/mujoco-wasm-play

## Acknowledgements

This project draws inspiration from earlier MuJoCo-to-WASM experiments that proved the concept and documented key pitfalls:

- stillonearth/MuJoCo-WASM
- zalo/mujoco_wasm
- hashb/mujoco_web

We remain grateful for their pioneering work.

## Provenance

Portions of this repository's scripts and documentation were authored or refined with the help of generative AI, then reviewed by a human maintainer.
