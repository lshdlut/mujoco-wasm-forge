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

Dimension functions preserve signed i32 / JavaScript Number results for existing
consumers. Out-of-range values return `-1` with a diagnostic. Dimensions declared
as `mjtSize` additionally expose `<name>_i64` functions returning exact BigInt.
Direct `mjtSize*` views are 64-bit (`BigInt64Array`); `mjtBool*` views are bytes
(`Uint8Array`). Function wrappers for `mju_warning`/`mju_error`/`mju_info` accept literal text,
including percent characters, rather than a C varargs format string.

Nested struct field pointers refer directly to MuJoCo memory. For arrays of
structs (for example `mjData.contact`), a pointer addresses the first element's
field; fields in successive elements have the C struct stride and are not a
contiguous field-only array. The `scene_geoms_*` exports separately provide
packed SoA buffers.

## Typed calls and additive POD adapters

`abi/js_signature_capabilities.json` classifies each raw function signature as
scalar, pointer, i64, aggregate, callback, opaque, or unknown. An exported symbol
does not imply JavaScript object marshalling. Pointer calls require valid aligned
WASM memory and the native lifetime contract; callback and opaque lifecycle APIs
remain explicitly marked as requiring a bridge or policy review.

Raw WASM i32 returns are signed JavaScript Numbers even for unsigned C types.
The generated type policy specifies `rawResult >>> 0` for u32 results (including
wasm32 `size_t`) and `BigInt.asUintN(64, rawResult)` for u64 results. Raw exports
remain unchanged; callers apply the documented result normalization.

Only records whose complete introspected fields are known scalars, fixed arrays,
or nested POD records receive type-driven adapters. Aggregate inputs use an
additive `_ptr` suffix; aggregate outputs use `_out` (combined `_ptr_out` when
needed). Existing raw symbols and signatures remain unchanged. Callers allocate
aligned native record memory and pass its address; this is not a JavaScript SDK.
Generated `sizeof`, `alignof`, and field-offset helpers cover only records used by
these adapters. The latest camera and logging-record adapters are exercised by
`check/tests/aggregate-abi.mjs`.

i64 and u64 arguments require JavaScript BigInt. A raw u64 return uses the signed
WASM i64 bit pattern and must be interpreted with `BigInt.asUintN(64, result)`;
the machine-readable signature metadata records this normalization. No Number
conversion or narrowing is applied to these signatures.

Existing AoS-derived field pointers have additive `<pointer_name>_stride`
functions returning the byte stride of the native record. Their record types and
symbols are listed in `abi/aos_record_strides.json`. These helpers do not turn
the field views into packed arrays. The aggregate smoke test reads two actual
contacts using this stride.

## Audit evidence and reproducibility

Each flavor has a required `abi/nm_coverage.single.json` or
`abi/nm_coverage.pthreads.json` report. Its schema, flavor, successful scan status,
and SHA256 of the corresponding WASM must match. The shared legacy
`nm_coverage.json` alias must exactly equal its selected flavor report. Missing,
failed, mismatched, or stale evidence fails `verify-dist`; a missing static
archive also fails independent post-build auditing.

`abi/build_metadata.json` records the resolved upstream Git SHA, actual configured
Emscripten compiler version, and separate flavor settings and WASM hashes. Each
flavor also records whether the dependency is dirty and a newline-normalized
tracked patch hash: the upstream SHA is not a claim of pristine source inputs.
No host paths or timestamps are included. Historical artifacts without this
receipt retain explicit unknown provenance in release packaging.

The reproducible recipe maps configured source and dependency roots with
`-ffile-prefix-map` for C and C++. Structured logging introduced retained
`__FILE__` strings in the engine: readable repository-relative locations avoid
embedding host checkout paths in WASM. This is compile-time path canonicalization,
not binary post-processing or a relaxed comparison. Patch hashes use Git
`--full-index` so repository-size-dependent abbreviated object IDs cannot alter
the receipt. An actual two-checkout compiled fixture proves byte-identical WASM
and readable relative source locations.

Historical deliverables retain their original generator recipe. New baselines
must be committed with their matching generator and pipeline changes; strict
fresh Linux comparison is not replaced by the local runtime checks.
