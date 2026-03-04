# Bench: forge vs official embind (Node + Play/Simulate)

This directory contains **public, reproducible benchmarks** that support the
project’s documentation claims. The intent is to make it easy to answer:

- How do `init`, memory, XML compilation, and stepping performance compare?
- What are the deployment/feature trade-offs between **official MuJoCo Embind**
  and **mujoco-wasm-forge** artifacts?
- How does the forge runtime behave in a **Simulate-style** browser workflow
  (`mujoco-wasm-play`)?

> Numbers vary with hardware, browser, power settings, and background load.
> The scripts capture enough metadata to make runs comparable.

## Prerequisites

- Node.js >= 20
- For official Embind builds: Emscripten (emsdk)
- For browser bench: `mujoco-wasm-play` checkout next to this repo

## 1) Build official Embind artifacts (3.4.0, 3.5.0)

The helper script builds Google DeepMind’s official `mujoco_wasm.js/.wasm`
bundle at a specific MuJoCo ref and copies `wasm/dist/` into an output folder.

### Windows (recommended when using `C:\emsdk`)

```powershell
# From the mujoco-wasm-forge repo root:
powershell -ExecutionPolicy Bypass -File tools/build_official_embind.ps1 -Ref 3.4.0 -OutDir C:\dev\mjwf-bench\official\3.4.0 -Clean
powershell -ExecutionPolicy Bypass -File tools/build_official_embind.ps1 -Ref 3.5.0 -OutDir C:\dev\mjwf-bench\official\3.5.0
```

### WSL/Linux (requires a Linux-native emsdk)

If you run builds from WSL, use a **Linux-native** emsdk checkout (ELF toolchain).

Run in WSL:

```bash
# From the mujoco-wasm-forge repo root:
EMSDK_ROOT=$HOME/emsdk bash tools/build_official_embind.sh --ref 3.4.0 --out /mnt/c/dev/mjwf-bench/official/3.4.0 --clean
EMSDK_ROOT=$HOME/emsdk bash tools/build_official_embind.sh --ref 3.5.0 --out /mnt/c/dev/mjwf-bench/official/3.5.0
```

## 2) Node bench (official vs forge)

This runs the matrix (5 runs per row by default) and writes:

- raw JSON runs to `bench/results/raw/`
- aggregated summaries to `bench/results/`:
  - `summary.md` (human-readable tables)
  - `summary.json` (machine-readable aggregates)
  - `tables/*.csv` (report-friendly exports)

```bash
node bench/node/run_matrix.mjs \
  --official340 C:/dev/mjwf-bench/official/3.4.0 \
  --official350 C:/dev/mjwf-bench/official/3.5.0
```

Notes:
- `bench/node/run_matrix.mjs` clears `bench/results/raw/` by default. Use `--append` to keep prior JSON runs.
- The Node bench stages required model assets into Emscripten FS (MEMFS) to avoid relying on `NODEFS` support in the wasm build.

> `bench/results/` is intended as a local output directory and is gitignored.

### Variable control (and known confounders)

What the Node bench tries to control:
- same host machine / OS / Node binary (run the whole matrix from the same `node`)
- same model inputs (same XML + assets staged into MEMFS)
- same measurement points (module init, XML compile/load, stepping loop)

Known confounders (must be stated when interpreting results):
- Official embind and forge use different JS-level APIs to load XML (embind types vs forge helper wrappers).
- Official builds may enable pthreads by default in 3.5.0; we approximate pool behavior by overriding `navigator.hardwareConcurrency` (`--hc`).
- Plugin availability is a **product difference** (official may fail plugin-based models; forge is expected to pass).

The generated summary includes an “apples-to-apples” table that only compares the common `status=ok` subset for selected pairs.

## 3) Browser bench (Play/Simulate workflow via Playwright)

This uses `mujoco-wasm-play`’s existing Playwright toolchain to measure the
Simulate-style HUD `CPU (ms/step)` and `FPS` across forge dist variants.

PowerShell (Windows):

```powershell
powershell -ExecutionPolicy Bypass -File bench/browser/run_playwright_bench.ps1
```

The script writes raw JSON runs under `bench/results/raw_browser/`.
Each browser JSON contains a `status` field (`ok` / `error` / `skipped`) plus HUD-derived metrics when available.

### Browser bench knobs

The Playwright bench is driven by env vars (set before running `bench/browser/run_playwright_bench.ps1`):

- `MJWF_BENCH_RUNS` (default: `3`)
- `MJWF_BENCH_WARMUP_MS` (default: `15000`)
- `MJWF_BENCH_SAMPLE_MS` (default: `8000`)
- `MJWF_BENCH_HEADLESS` (`1` for headless; default: `0`)
- `MJWF_BENCH_VARIANTS` (comma-separated ids; e.g. `forge-3.5.0,forge-3.5.0-pthreads`)
- `MJWF_BENCH_MODELS` (comma-separated ids; e.g. `raj,cards`)

## Notes

- Prefer putting official build outputs under a local cache directory outside
  OneDrive (e.g. `C:/dev/...`). This avoids file-lock churn and sync overhead.
- The Node bench mounts host model directories via Emscripten `NODEFS` when
  available, so large model assets do not inflate wasm heap usage.

## Helper checks (Simulate-critical)

Forge ships helper entrypoints that Play/Simulate relies on (e.g. `mjwf_helper_make_from_xml`).
These are exercised by `--with-checks` during `forge_cli.py build`:

- `check/tests/helper-make-from-xml.mjs` validates that invalid XML / missing plugin errors are reported via `errno/errmsg` and do not throw raw wasm exceptions.
