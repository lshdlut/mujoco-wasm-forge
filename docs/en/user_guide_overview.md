# Overview

## What is mujoco-wasm-forge?

`mujoco-wasm-forge` is a toolchain that builds a specific upstream MuJoCo ref into WebAssembly, while keeping the
exported symbols and wrapper surface explicit and reviewable.

The goal is not just “MuJoCo compiles to WASM”, but:

- build a given MuJoCo ref into a predictable `dist/<ver>/` output layout,
- keep exported symbols under control via a single export list (`exports.lst`),
- generate ABI/audit artifacts so upgrades can be reviewed with diffs,
- run automated checks (smoke + quality gates) so CI can catch drift early.

## What it guarantees (and what it doesn’t)

**Designed guarantees**

- A single entrypoint (`forge_cli.py build`) that regenerates the full pipeline for one version/ref.
- An explicit exported C symbol list (`dist/<ver>/abi/exports.lst`) used by the link step.
- A consistent “dist layout” (`dist/<ver>/...`) consumed by all checks.
- Gate scripts that can be run locally and in CI.

**Non-goals**

- Freezing upstream MuJoCo ABI across versions (MuJoCo itself can change).
- Providing a high-level ergonomic JS API (the core output is Emscripten `mujoco.js/.wasm` plus wrappers).
- Guaranteeing byte-for-byte identical artifacts across arbitrary toolchains (use `verify-dist` for CI-grade
  reproducibility checks).

## Key terms

See `glossary` for a short definition of ABI, wrappers, `exports.lst`, introspection, and gates.

