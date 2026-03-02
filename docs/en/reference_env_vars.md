# Environment variables & build knobs

## Version selection

These control which `dist/<ver>` directory is “active” for helper scripts:

- `MJVER`
- `DIST_VERSION`

They are consumed by `dist_version.py` and `check/dist_paths.mjs`.

## Toolchain

- `EMSDK`: path to your emsdk directory (must contain `emsdk_env.sh`). Required by the Emscripten CMake build.

## Build tree placement

- `MJWF_BUILD_ROOT`: root directory for intermediate build trees (defaults to `<repo>/build`).

## Export list override

- `MJWF_EXPORTS_LIST`: override the export list path passed to the link step (defaults to `dist/<ver>/abi/exports.lst`).

## CMake profile

`app/CMakeLists.txt` defines a CMake cache variable `MJWF_PROFILE` with the following values:

- `default`
- `fast` (used by the current forge CLI build)
- `official`
- `fast_official`

Profiles mainly change optimization/debug flags, filesystem settings, and related link options.

## Quality gates (`check/tests/gates.mjs`)

`gates.mjs` checks wasm/js size and init time.

- `QUALITY_ENFORCE=1`: treat breaches as failures (otherwise it only warns).
- `WASM_MAX_BYTES` (default: 16_000_000)
- `JS_MAX_BYTES` (default: 2_000_000)
- `MAX_INIT_MS` (default: 8000)

