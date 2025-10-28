# Quality Gates

Intent
- [GATE:SYM] Symbol integrity from JSON: ensure exported symbols match the generated allowlist.
- [GATE:DTS] Type surface drift: regenerate TypeScript definitions from specs and diff against repository versions.
- [GATE:RUN] Runtime validation: smoke (basic init/step), regression (WASM vs native harness), and mesh smoke (3.3.7).

Inputs & outputs
- SYM: inputs = `exports_<short>.json`; output = pass/fail list of missing symbols.
- DTS: inputs = `spec_*.yaml`; output = zero-diff against `types/mjw<short>.d.ts`.
- RUN: inputs = test fixtures and harness; output = deterministic counters and invariants.

Failure localization
- SYM: missing `_mjwf_*` entries or mismatch with Emscripten mangling.
- DTS: non-empty diff; check spec and generator.
- RUN: mismatch between native and WASM or invariants violated.

Notes
- If a gate is not yet scripted, the workflow logs `[GATE:*] skipped` without changing behavior.

