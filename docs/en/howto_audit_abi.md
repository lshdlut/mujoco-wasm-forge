# How-to: audit ABI and investigate diffs

When a MuJoCo upgrade changes exports, the fastest way to debug is to follow the pipeline’s “paper trail” in
`dist/<ver>/abi/`.

## Key files

- `exports.lst`: what the wasm build exports (link-time contract).
- `wrapper_exports_funcs.json`: what the wrapper generator intends to export.
- `nm_symbols.json`: what the implementation side provides.
- `exports_report_funcs.md`: a human-readable report of export status.
- `functions_introspect_like.json`: what the headers declare (via introspection).

## Typical questions

| Question | Where to look |
| --- | --- |
| “Is this function declared in headers?” | `functions_introspect_like.json` |
| “Is it implemented in the build?” | `nm_symbols.json` |
| “Do we generate a wrapper?” | `wrapper_exports_funcs.json` |
| “Is it actually exported?” | `exports.lst` |

## Common causes

- The function exists upstream but is not available under the current build configuration.
- The wrapper generator intentionally excludes it (contract decision).
- A rename happened upstream and wrappers/exports need an update.

