# Contributing

## Scope and expectations

- Keep changes focused (one intent per PR when possible).
- If you change pipeline code, update at least one committed `dist/<ver>` (or run `verify-dist` to show no drift).
- If you change documentation structure, update **both** `docs/en/` and `docs/zh/`.

## Local workflow (typical)

1. Build a target version:
   - `python3 forge_cli.py build --version <mjver> --with-checks`
2. Review changes under `dist/<ver>/abi/` (exports list + reports).
3. If committing `dist/<ver>`, ensure CI can reproduce it (`verify-dist`).

## What not to commit

- `external/` (upstream checkouts)
- intermediate build trees (default `<repo>/build` or `MJWF_BUILD_ROOT`)
- local caches/logs

