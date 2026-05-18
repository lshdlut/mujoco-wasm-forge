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

For a committed `dist/<ver>/` release, tag the already-verified commit:

```bash
git tag forge-<ver>-r1
git push origin forge-<ver>-r1
```

For the MuJoCo 3.6+ batch:

```bash
git tag forge-3.6.0-r1
git tag forge-3.7.0-r1
git tag forge-3.8.0-r1
git tag forge-3.8.1-r1
git push origin forge-3.6.0-r1 forge-3.7.0-r1 forge-3.8.0-r1 forge-3.8.1-r1
```

## Release assets

For release-like tags, CI publishes two assets under the same GitHub Release:

- `dist-runtime.zip`
  - runtime-only payload for downstream web hosts / apps
  - zip root matches the forge webroot (`mujoco.js`, `mujoco.wasm`, optional `pthreads/`, `version.json`)
  - excludes `abi/`
  - includes `pthreads/` only when that variant exists under `dist/<ver>/`

- `dist-audit.zip`
  - audit/debug payload for maintainers
  - zip root contains `abi/` plus `version.json`

## Local reproduction of the CI verify step

Create a second clean checkout (or directory) and build into it, then run:

```bash
python3 forge_cli.py verify-dist --version <ver> --ci-build-dir ci-build
```
