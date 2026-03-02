# ABI contract (exports & wrappers)

`mujoco-wasm-forge` treats exported symbols as an explicit contract.

## The contract

- The definitive list of exported C symbols is `dist/<ver>/abi/exports.lst`.
- The WASM build consumes it via Emscripten `-sEXPORTED_FUNCTIONS=@.../exports.lst`.

That means “what is exported” is always reviewable as a plain text diff.

## A ∩ B = C (flat exports)

The export pipeline intentionally keeps three sets visible:

- **A**: header-derived declarations (introspection).
- **B**: implemented symbols (from `nm_symbols.json`).
- **C**: wrapper exports (the supported surface, typically `mjwf_*`).

Generators under `abi_exports/` merge these inputs and produce:

- wrapper sources (`mjwf_abi_funcs.*`, `mjwf_abi_structs.*`),
- wrapper export manifests (for auditing),
- `exports.lst` (used for linking).

## JS/Runtime surface

The primary output is Emscripten `mujoco.js`/`.wasm`. Runtime entrypoints such as `ccall`/`cwrap` are exported by the
link flags in `app/CMakeLists.txt`.

