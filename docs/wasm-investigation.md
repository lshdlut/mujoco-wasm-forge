# MuJoCo WASM (forge vs. official embind) — Investigation Record

This document summarizes our recent experiments comparing the forge wrapper pipeline and the official Embind-based WASM build from MuJoCo (commit 40862617, targeting 3.3.8-alpha). It records build parity fixes, measurement methodology, and key findings to guide future choices.

## Context

- Goal: replicate CI locally, fix test aborts, and fairly compare performance between our wrapper and official Embind bindings.
- Toolchains used locally:
  - Emscripten: emsdk 4.0.10 (canonical baseline for the repo and CI)
  - Node: 22 (from emsdk). CI parity is Node 20; pin when byte-level reproducibility matters.
- Versions under test:
  - Forge: 3.2.5, 3.3.7, 3.3.8-alpha (focus of benchmarks: 3.3.8-alpha)
  - Official Embind: commit 4086261714d7cfbc1745d4c6cb0aa2116df45312 (introduces WASM + JS bindings)

## CI parity issues and fixes

1) Test abort: `HEAP8 was not exported`
   - Root cause: our tests access `Module.HEAP8` and other runtime methods. Emscripten requires these to be explicitly listed via `-sEXPORTED_RUNTIME_METHODS` when `MODULARIZE` + `EXPORT_ES6` builds are used.
  - Fix: in wrapper CMake for all versions, include `['cwrap','ccall','UTF8ToString','stringToUTF8','lengthBytesUTF8','FS','wasmExports','stackSave','stackRestore','stackAlloc','HEAP8','HEAPU8','HEAP32','HEAPU32','HEAPF32','HEAPF64']` in `-sEXPORTED_RUNTIME_METHODS`.

2) qhull linkage under Emscripten
   - Root cause: upstream qhull defaults to `SHARED` targets; Emscripten doesn’t support shared libs.
   - Fix: enforce static linkage in the build dir (patch `SHARED`→`STATIC`, set `BUILD_SHARED_LIBS=OFF`), as mirrored in CI.

3) Version pinning and environment
   - Node version differences can impact JIT warmup and micro-bench numbers. For CI parity, pin Node 20; for local iteration, Node 22 from emsdk is acceptable.

4) Direct exports vs cwrap fallback
   - For hot calls, using `Module.wasmExports._symbol` eliminates `cwrap` overhead. Our benchmark harness first tries `wasmExports` then falls back to `cwrap` if needed.

## Profiles used (forge)

We introduced a small set of wrapper profiles, selected via `-DMJWF_PROFILE=`:

- `default`: `-Oz`, assertions on, FS off, exceptions off, LTO on.
- `fast`: `-O3`, assertions off, FS off, exceptions off, LTO on. (Leanest runtime; best micro-benchmark results.)
- `official`: `-O3`, assertions on, FS on, exceptions on, debug symbols on, LTO off. (Closer to official Embind flags.)
- `fast_official`: `-O3`, assertions on, FS on, exceptions on, debug symbols on, LTO on. (Hybrid.)

## Benchmark setup

- Workload: minimal pendulum MJCF; measure `mj_step`, `mj_forward`, and micro functions (`mj_sensorPos`, `mj_energyPos`, `mj_fwdConstraint`).
- Harness: `scripts/benchmarks/profile_benchmark.mjs` (forge) and `scripts/benchmarks/profile_benchmark_official.mjs` (official Embind).
- Knobs:
  - `BENCH_PREFORWARD=1`: do a single `mj_forward()` before measuring micro functions to avoid re-advancing stages.
  - `BENCH_READBACK=1`: additionally copy a typed view from WASM memory to JS per iteration to expose JS↔WASM transfer cost.
  - `BENCH_BATCH=1` (forge only): run a C-side for-loop via `mjwf_bench_*` entry points to eliminate per-call JS overhead.

## Results (3.3.8-alpha, 100k iters, warmup 1k, Node 22, emsdk 4.0.10)

Key observations:

- `mj_step` / `mj_forward` dominated by physics; forge and official are within the same ballpark (sub-microsecond).
- Micro functions are sensitive to call boundary and memory views. With JS readback disabled and `wasmExports` direct call, forge `fast` reaches single-digit nanoseconds for some functions.
- `BENCH_BATCH=1` validates that most of the micro gap stems from JS↔WASM overhead rather than compute.
- Turning on FS/exceptions (to mimic official runtime) increases micro overhead; our `fast` profile remains leaner.

Illustrative numbers (avg per call):

- Forge `fast` (preforward): `mj_step` ~0.707µs, `mj_forward` ~0.477µs; `sensorPos` ~6.7ns, `energyPos` ~14.8ns, `fwdConstraint` ~10.9ns.
- Forge `fast` (preforward + batch): `sensorPos` ~3.0ns, `energyPos` ~13.1ns, `fwdConstraint` ~8.0ns.
- Official Embind (preforward): `mj_step` ~0.706µs, `mj_forward` ~0.512µs; `sensorPos` ~10.5ns, `energyPos` ~16.6ns, `fwdConstraint` ~15.5ns.

## Conclusions

1) The earlier CI abort was caused by missing exported runtime methods (not a core compute defect). Exporting `HEAP8` et al. fixed it.
2) For core steps (`mj_step`/`mj_forward`), both pipelines are performance-equivalent within noise.
3) The micro-function gap is largely the JS boundary and memory view overhead; direct `wasmExports` and batched C entry points close the gap.
4) Official Embind includes a richer runtime (FS, exceptions, TS types) which is great for general developer ergonomics but carries small fixed overheads.

## Recommendations

- Maintain two flows in parallel:
  - Official Embind (standard, feature-rich, broad developer appeal)
  - Forge wrapper (lean, customizable, tuned for micro overhead minimization)
- Document pre-forward and batch patterns for micro-function usage.
- Keep CI knobs to run both scripts and publish side-by-side summaries for regression tracking.

## Commands recap

Forge build (WSL):

```
TARGETS=338 MJVER_338=3.3.8-alpha MJREF_338=4086261714d7cfbc1745d4c6cb0aa2116df45312 \
  bash ./local_tools/wsl/build.sh

# Profile switches
#   PROFILE_FAST=1     -> fast
#   MJWF_PROFILE=official | fast_official
```

Forge benchmark:

```
node scripts/benchmarks/profile_benchmark.mjs \
  [BENCH_PREFORWARD=1] [BENCH_READBACK=1] [BENCH_BATCH=1] \
  [BENCH_ITERATIONS=100000] [BENCH_WARMUPS=1000]
```

Official Embind build (WSL):

```
bash ./local_tools/embind/build.sh
```

Official benchmark:

```
node scripts/benchmarks/profile_benchmark_official.mjs \
  [BENCH_PREFORWARD=1] [BENCH_READBACK=1] \
  [BENCH_ITERATIONS=100000] [BENCH_WARMUPS=1000]
```
