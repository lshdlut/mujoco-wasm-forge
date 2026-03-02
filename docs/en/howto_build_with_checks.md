# How-to: build with checks

## Run build + checks in one command

```bash
python3 forge_cli.py build --version 3.5.0 --with-checks
```

This runs the build pipeline and then executes:

- `node check/tests/smoke.mjs`
- `node check/tests/mesh-smoke.mjs`
- `node check/tests/gates.mjs`

## Run checks manually

When running checks by hand, set `MJVER` (or `DIST_VERSION`) so the scripts use the right `dist/<ver>`:

```bash
MJVER=3.5.0 node check/tests/smoke.mjs
MJVER=3.5.0 node check/tests/mesh-smoke.mjs
MJVER=3.5.0 node check/tests/gates.mjs
```

To enforce quality gates (instead of warnings), set `QUALITY_ENFORCE=1` (see `reference_env_vars`).

