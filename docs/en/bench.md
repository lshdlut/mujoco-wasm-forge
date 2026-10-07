# Bench: validating forge's flexibility without performance regression

This chapter describes the public benchmark suite shipped with `mujoco-wasm-forge`.
The suite exists to justify *why forge exists*:

- **Flexibility**: a minimal, configurable interface surface and multiple distribution variants (e.g. single-threaded vs pthreads).
- **Flatness**: flat handles and a predictable ABI that downstream tools and apps can build around (Worker / Node / Web).
- **Rule-based, auditable generation**: wrappers/exports are generated and checked by rules so changes are explicit and reviewable.

In forge, **flexibility** is framed as a complement to the official embind distribution. Official embind typically offers
a richer object-oriented and ergonomic binding surface, but that breadth also comes with a larger surface area and a more
standardized distribution shape, which can make fine-grained control over variants and exported ABI less direct. Forge
instead relies on the **flat ABI organization**
and the **automated output structure** (stable dist layout + ABI artifacts + rules/gates): most changes are
`change rules -> regenerate -> gate`, which is designed to be auditable and variant-friendly.

Forge is intended to be usable in **extreme deployment environments** (e.g. online demos with strict security and
resource constraints) and in **research prototyping** workflows, while still being a reasonable base for routine use.

The 3.5.0 tables below are a historical reference snapshot. They are retained for continuity with the original
benchmark discussion and are not a claim about the current 3.15.0 release state.

## 3.15.0 quick deployment comparison (reference snapshot)

This bounded comparison is preserved in the [public reference receipt](../../evidence/bench/mujoco-3.15-reference-20261007.json).
Its Forge hashes precede the closing ABI adapters and are not the final canary bytes.
It used four rows (`forge` single, official single, forge pthreads, and
official `mt`), three processes per row (12/12 passed), the same `humanoid` and `cards` models, warmup 10, and
2,000 measured steps. The Forge pthread and official `mt` rows used a pooled target of 4; the MuJoCo engine pool
was 0 in all rows. `strictApplesToApples` was `false` because this compares deployment bundles, not identical
binding implementations.

| Row | Runtime pair bytes | Init median (ms) | Humanoid ms/step | Cards ms/step | RSS peak (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| forge-3.15.0-single | 4,290,837 | 24.50 | 0.04993 | 0.34880 | 199.96 |
| official-3.15.0-single | 10,664,021 | 72.40 | 0.04058 | 0.42072 | 215.44 |
| forge-3.15.0-pthreads | 4,293,867 | 58.13 | 0.04607 | 0.34482 | 257.72 |
| official-3.15.0-mt | 10,789,205 | 94.50 | 0.03902 | 0.42376 | 266.45 |

The frozen reference artifact hashes are:

| Row | JS SHA-256 | WASM SHA-256 |
| --- | --- | --- |
| forge-3.15.0-single | `4ab960926cb4d5d315e8db0b411638542a6e15e767d7f415c6f92343ba8ee5b6` | `dc56872be3ed03610398b5ad441766c4f16eeac43f652e9a04c6f2e9649939b8` |
| forge-3.15.0-pthreads | `f2145ff5e51fb0118e627e1b505bc62ea6f9d6644c3a725d01e3623e06ad278a` | `6a74011ecd6ddc6c661a7fcc88b713c6e6c13a29d6fbadde39a49d28ba9bb20f` |
| official-3.15.0-single | `a5d5f00c4c16d327cf8275f88ddd47c857a5e9baec8ca4174d676fe299267a0c` | `d59c489dd521c80af6965756c5b90daff549fa281566544e4540c170d937633d` |
| official-3.15.0-mt | `75809df07e334932a0cde9b3dda9440783b247f2345c2de029e5db4fe471988` | `a65e46a955044a1d85bf02cdf41ea9d1d42a94ea70eac5ff5a91978cce3f2019` |

This is deployment-bundle evidence only: it does not isolate binding overhead, claim that Forge or official
bindings are universally better, or certify the final byte sizes after any later ABI regeneration. If the final
release artifacts change, rerun the bounded comparison and update this reference snapshot.

The benchmark asks a concrete question:

> Under these design constraints, does forge stay in the same performance class as the official MuJoCo embind WASM build
> for Simulate-style workloads? (And when it differs, is the reason attributable and controllable?)

This is **not** a claim that forge is universally better than official embind. The two distributions have different
goals; the bench is here to make trade-offs visible, reproducible, and discussable.

## What we measure (organized by user needs)

| Need / scenario | Why we care | What we measure (metric) | Notes |
| --- | --- | --- | --- |
| Ship fast (web demo / docs) | Download + caching affects real users | Artifact size (`.js` + `.wasm`) | Measured from produced bundles |
| Page becomes interactive | Cold start is a hard UX constraint | Init time (`modFactory() -> Module.ready`) | Dominated by threading/runtime initialization |
| First meaningful output (Simulate) | First step / first snapshot is when users feel success | TTFS (ready -> first step / first scene snapshot) | Browser TTFS is end-to-end; Node TTFS isolates engine load/step |
| Predictable resource budget | Thread pools + compilation can cause spikes | RSS after init + **peak** RSS | Node provides more reliable RSS sampling than browsers |
| Smooth model load | XML compilation and mesh processing dominate load time | XML compile/load latency | Sensitive to threading defaults and asset shape |
| Stable steady-state simulation | After load, engine hot-loop dominates | `ms/step` / steps/sec | Requires comparable threading configuration for strict comparisons |
| Low JS <-> WASM overhead | Flat/handle-based APIs should reduce crossing cost | FFI microbench (ns/call for lightweight getters) | Microbench is intentionally small and repeatable |
| Robust lifecycle (reset/reload) | Demos frequently reset or reload models | Reload loop time + RSS drift | Amplifies leaks and binding overhead |
| Diagnosable failures | Public demos must fail with readable errors | Errmsg/errno gates (bad XML, missing plugin, missing assets) | Forge has explicit helper gates for this |
| Plugin baseline | Some Simulate models require plugins | Plugin availability smoke (e.g. `touch_grid`) | Treated as a product-level contract difference |

## Comparison matrix (what we compare)

| Label | Intended meaning | Threading policy |
| --- | --- | --- |
| `official-3.4.0` | Official embind baseline | As built upstream |
| `official-3.5.0-hc4` / `official-3.5.0-hc32` | Official embind (3.5.0) under different effective pool sizing | Pthreads enabled; pool approximated via `hardwareConcurrency` |
| `forge-3.4.0-single` | Forge baseline | Single-threaded |
| `forge-3.5.0-single` | Forge single-threaded variant | Single-threaded |
| `forge-3.5.0-pthreads` | Forge pthread variant | Pthreads enabled; pool defaults to 4 (override via `MJWF_PTHREAD_POOL_SIZE`; for 3.5.0 default clamp=pool) |

For strict (thread-matched) comparisons, we primarily compare:
`forge-3.5.0-pthreads (pool=4)` vs `official-3.5.0-hc4`.

## Summary findings (reference run)

The suite is designed to be rerun on different machines; numbers vary. The key question is whether forge's organization
and flexibility introduces a performance penalty.

| Dimension | Observation (selected numbers) | Interpretation (what it means) | Caveats |
| --- | --- | --- | --- |
| Distribution size | `forge-3.5.0-single`: wasm 3.33 MiB / JS 256.1 KiB; `official-3.5.0-hc4`: wasm 8.24 MiB / JS 296.7 KiB | Minimal/flat surface can reduce shipping footprint | Not guaranteed across future upstreams |
| Init + memory | `forge-3.5.0-pthreads`: init 52.7ms / peak RSS 462.8 MiB; `official-3.5.0-hc4`: init 84.4ms / peak 659.2 MiB; `official-3.5.0-hc32`: RSS after init 507.9 MiB | Threading policy and pool sizing can dominate cost | RSS is platform-dependent; browsers differ |
| Plugin baseline | `sensor`/`touch_grid`: forge=ok, official=error | Forge can treat plugin availability as part of the dist contract | Official plugin packaging may evolve upstream |
| `ms/step` (thread-matched) | e.g. `raj` 0.086 vs 0.103 ms/step; `cards` 0.325 vs 0.343 (forge pthreads vs official hc4) | Forge organization does not inherently degrade simulation throughput | Small models can be dominated by measurement overhead |
| FFI microbench | `model_nq`: 14.71ns vs 33.56ns/call; `mj_version`: 4.23ns vs 5.24ns | Flat handles reduce high-frequency boundary cost | Microbench != whole-app performance |
| TTFS (Simulate) | `humanoid` ready->snapshot: 77ms (pthreads) vs 111ms (single); `cards`: 505ms (pthreads) vs 225ms (single) | End-to-end pthread benefit depends on model/load path | HUD metrics depend on environment/COI configuration |
| Reload / lifecycle | 50 iterations: 0.840ms/iter (forge pthreads) vs 1.003ms/iter (official hc4); RSS drift: +1.9 MiB vs +7.0 MiB | Lifecycle pressure can amplify binding overhead differences | Not a proxy for every workload |

### Reference snapshot tables

Reference run environment (for the snapshot tables below): Windows x64 / Node `v22.16.0` / 32 logical CPUs.
Unless noted otherwise:
- Node metrics are medians of 5 runs.
- Browser HUD metrics default to 3 runs (median).
- Sizes and memory are reported in KiB/MiB (1024-based).

### Snapshot (Node: bundle / init / memory)

| Label | JS (KiB) | WASM (MiB) | Init (ms) | RSS after init (MiB) | RSS peak (MiB) |
| --- | --- | --- | --- | --- | --- |
| `forge-3.5.0-single` | 256.1 | 3.33 | 19.4 | 59.8 | 208.8 |
| `forge-3.5.0-pthreads` | 272.6 | 3.33 | 52.7 | 110.7 | 462.8 |
| `official-3.5.0-hc4` | 296.7 | 8.24 | 84.4 | 125.5 | 659.2 |
| `official-3.5.0-hc32` | 296.7 | 8.24 | 169.8 | 507.9 | 873.9 |

### Model load + stepping (thread-matched, Node: `pool=4` vs `hc=4`)

| Model | Forge compile/load (ms) | Official compile/load (ms) | Forge steady `ms/step` | Official steady `ms/step` |
| --- | --- | --- | --- | --- |
| `cards` | 90.8 | 109.6 | 0.325 | 0.343 |
| `raj` | 303.4 | 384.3 | 0.086 | 0.103 |
| `humanoid` | 5.4 | 7.6 | 0.020 | 0.021 |
| `flex_bunny` | 59.5 | 102.5 | 0.426 | 0.465 |

### Functional baseline (Node: plugin-dependent models)

| Model | Forge 3.5.0 single | Official 3.5.0 (hc=4) |
| --- | --- | --- |
| `sensor` | ok | error |
| `touch_grid` | ok | error |

### FFI microbench (Node: apples-to-apples subset)

> This is a "1e6 calls" microbench. It is meant for comparing relative organization/binding overhead, not for
> extrapolating whole-app throughput.

| Metric | Forge 3.5.0 pthreads ns/call (median) | Official 3.5.0 ns/call (median) |
| --- | --- | --- |
| `mj_version` | 4.23 | 5.24 |
| `model_nq` | 14.71 | 33.56 |

### Simulate TTFS (Browser: forge variants only)

> Browser numbers come from `mujoco-wasm-play` + Playwright HUD sampling (default: 3 runs, median).
> In some environments, the pthread variant may not expose HUD `CPU (ms/step)`, but TTFS/FPS is still sampled.

| Model | Variant | TTFS ready->snapshot median (ms) | TTFS nav->snapshot median (ms) | FPS median |
| --- | --- | --- | --- | --- |
| `humanoid` | single | 111 | 322 | 120 |
| `humanoid` | pthreads | 77 | 287 | 120 |
| `cards` | single | 225 | 499 | 120 |
| `cards` | pthreads | 505 | 791 | 120 |
| `raj` | single | 338 | 570 | 121 |
| `raj` | pthreads | 270 | 510 | 45.5 |

### Reload / lifecycle (thread-matched, Node: `pool=4` vs `hc=4`)

| Label | Iterations | ms/iter | RSS drift (MiB) | RSS peak (MiB) |
| --- | --- | --- | --- | --- |
| `forge-3.5.0-pthreads` | 50 | 0.840 | 1.9 | 462.8 |
| `official-3.5.0-hc4` | 50 | 1.003 | 7.0 | 659.2 |

## Limitations (what this bench may still miss)

- Official embind and forge necessarily use different JS-level APIs, even when the underlying engine version is comparable.
- Browser pthread deployments require COOP/COEP + `SharedArrayBuffer`; deployment constraints are part of the trade-off.
- Some metrics (especially browser memory) are harder to measure precisely and are currently more reliable in Node.
- Fastest possible is not the goal; the goal is forge capabilities without unacceptable regression.

## Reproducing the bench

See `bench/README.md` for the full procedure. The short version:

```powershell
powershell -ExecutionPolicy Bypass -File tools/build_official_embind.ps1 -Ref 3.4.0 -OutDir C:\dev\mjwf-bench\official\3.4.0 -Clean
powershell -ExecutionPolicy Bypass -File tools/build_official_embind.ps1 -Ref 3.5.0 -OutDir C:\dev\mjwf-bench\official\3.5.0
```

```bash
node bench/node/run_matrix.mjs --official340 C:/dev/mjwf-bench/official/3.4.0 --official350 C:/dev/mjwf-bench/official/3.5.0
node bench/node/report.mjs
```

```powershell
powershell -ExecutionPolicy Bypass -File bench/browser/run_playwright_bench.ps1
```
