# CI & release

## CI overview

The GitHub Actions workflow `.github/workflows/forge-dist-verify.yml` is responsible for verifying that committed
`dist/<ver>/` artifacts match a clean reproducible build.

High level:

1. Select which versions to verify (based on changed paths, tags, or manual input).
2. Build the selected versions in a clean checkout (`ci-build/`).
3. Run `python3 forge_cli.py verify-dist --version <ver>` to diff committed `dist/<ver>` vs the reproducible build.

## Tag convention

Tags matching `forge-*` are treated as release-like triggers. Examples:

- `forge-3.4.0-r1`
- `forge-3.5.0-r1`

## Local reproduction of the CI verify step

Create a second clean checkout (or directory) and build into it, then run:

```bash
python3 forge_cli.py verify-dist --version <ver> --ci-build-dir ci-build
```

