# Artifacts and guarantees

`mujoco-wasm-forge` writes its outputs under `dist/<ver>/`.

## Primary artifacts (what most users need)

- `dist/<ver>/mujoco.js`
- `dist/<ver>/mujoco.wasm`

These are the two files you typically copy into your web/worker/node project.

## ABI & audit artifacts (what maintainers and reviewers use)

The `dist/<ver>/abi/` directory contains the “paper trail” that makes upgrades reviewable:

- `exports.lst`: exported C symbols (link-time contract).
- `nm_symbols.json`: symbol inventory from the implementation side.
- `wrapper_exports_funcs.json`: wrapper exports produced by the generators.
- `exports_check.json` / `exports_report_funcs.md`: export check results / human-readable report.
- `*_introspect_like.json`: header-derived declarations for functions/structs/enums.
- `mujoco_ast.json`: clang AST dump (large, mostly for debugging and auditing).

## Version selection (important for checks)

Many scripts resolve the active version via environment variables:

- `MJVER` (preferred)
- `DIST_VERSION`

When you run `forge_cli.py build --version <mjver>`, the CLI sets those variables for the subprocesses it launches.
If you run scripts manually (for example `node check/tests/smoke.mjs`), you should set `MJVER`/`DIST_VERSION`
explicitly.

