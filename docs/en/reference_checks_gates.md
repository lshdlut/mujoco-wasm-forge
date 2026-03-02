# Checks & gates

The project validates builds at two levels:

1. **Post-build validation** (export/ABI checks during the build pipeline).
2. **Runtime checks** (Node-based smoke tests and quality gates).

## Post-build validation

`check/post_build.sh` validates export-related manifests produced during the build. It is run by `forge_cli.py build`
before optional runtime checks.

## Runtime checks (`--with-checks`)

`forge_cli.py build --with-checks` runs these scripts:

- `check/tests/smoke.mjs`
  - Loads a minimal pendulum XML, compiles, steps, and frees resources.
- `check/tests/mesh-smoke.mjs`
  - Same idea, but covers mesh parsing/compilation.
- `check/tests/gates.mjs`
  - Quality gates: wasm/js size thresholds and module init time.
  - Enforced only when `QUALITY_ENFORCE=1` (otherwise it logs warnings).

Additional scripts exist for ad-hoc debugging (not run by default), e.g.:

- `check/tests/xml-missing-ref.mjs` (verifies error reporting for missing referenced assets).

## Tuning

See `reference_env_vars` for the environment variables that control `gates.mjs`.

