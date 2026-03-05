# Environment variables & build knobs

## Version selection

These control which `dist/<ver>` directory is “active” for helper scripts:

- `MJVER`
- `DIST_VERSION`

They are consumed by `dist_version.py` and `check/dist_paths.mjs`.

## Toolchain

- `EMSDK`: path to your emsdk directory (must contain `emsdk_env.sh`). Required by the Emscripten CMake build.
- `MJWF_BASH`: override the `bash` executable used by `forge_cli.py` for `bash -lc ...` invocations (useful on Windows).

## Upstream checkout hygiene

- `MJWF_GIT_CLEAN_IGNORED=1`: enable `git clean -fdx` when refreshing `external/mujoco` (default is `git clean -fd` to avoid OneDrive permission issues).

## Build tree placement

- `MJWF_BUILD_ROOT`: root directory for intermediate build trees (defaults to `<repo>/build`).

## Export list override

- `MJWF_EXPORTS_LIST`: override the export list path passed to the link step (defaults to `dist/<ver>/abi/exports.lst`).

## Dist variant selection

- `MJWF_DIST_VARIANT`: optional dist subdirectory selector consumed by `check/dist_paths.mjs` (e.g. `pthreads`).

## CMake profile

`app/CMakeLists.txt` defines a CMake cache variable `MJWF_PROFILE` with the following values:

- `default`
- `fast` (used by the current forge CLI build)
- `official`
- `fast_official`

Profiles mainly change optimization/debug flags, filesystem settings, and related link options.

## pthreads (WebAssembly threads)

These variables control pthread worker pool sizing and stability knobs for the `--pthreads` variant:

- `MJWF_PTHREAD_POOL_SIZE`: integer >=1 (default: 4). Sets Emscripten `-sPTHREAD_POOL_SIZE=<n>` to prewarm a fixed WebWorker pool.
- `MJWF_PTHREADS_COMPILER_MAX_THREADS`: cap MuJoCo XML compiler threads:
  - unset / empty: use default (for MuJoCo 3.5.0 pthreads, defaults to `MJWF_PTHREAD_POOL_SIZE`).
  - `0`: disable clamp (use upstream `hardware_concurrency()/2` behavior).
  - `N>=1`: clamp to `<=N`.

## Quality gates (`check/tests/gates.mjs`)

`gates.mjs` checks wasm/js size and init time.

- `QUALITY_ENFORCE=1`: treat breaches as failures (otherwise it only warns).
- `WASM_MAX_BYTES` (default: 16_000_000)
- `JS_MAX_BYTES` (default: 2_000_000)
- `MAX_INIT_MS` (default: 8000)
