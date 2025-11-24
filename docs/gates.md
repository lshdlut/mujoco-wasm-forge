# Validation

Intent
- Ensure exported surfaces cover the official Embind-equivalent API (presence checks).
- Verify generated TypeScript definitions remain in sync with the exported surface.
- Exercise runtime smoke/regression/mesh tests for functional parity.

Inputs & outputs
- DTS: inputs = `spec_*.yaml`; output = zero-diff against generated `types/*.d.ts`.
- RUN: inputs = test fixtures and harness; output = deterministic counters and invariants.

Failure localization
- DTS: non-empty diff; check spec and generator.
- RUN: mismatch between native and WASM or invariants violated.

Notes
- Presence checks allow extra exports; the repository favors broad exposure aligned with upstream Embind.

