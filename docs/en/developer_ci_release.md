# CI & release

## CI overview

The GitHub Actions workflow `.github/workflows/forge-dist-verify.yml` is responsible for verifying that committed
`deliverables/<ver>/` artifacts match a clean reproducible `dist/<ver>/` build.

High level:

1. Select which versions to verify (based on changed paths, tags, or manual input).
2. Build the selected versions in a clean checkout (`ci-build/`).
3. Run `python3 forge_cli.py verify-dist --version <ver>` to diff committed `deliverables/<ver>` vs the reproducible `ci-build/dist/<ver>` tree.

## Tag convention

Tags matching `forge-*` are treated as release-like triggers. Examples:

- `forge-3.4.0-r1`
- `forge-3.5.0-r1`

For a committed `deliverables/<ver>/` release, tag the already-verified commit:

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
  - zip root matches the forge webroot (`mujoco.js`, `mujoco.wasm`, optional `pthreads/`, `version.json`, and `provenance.json`)
  - excludes `abi/`
  - includes `pthreads/` only when that variant exists under `deliverables/<ver>/`
  - preserves the pthread worker aliases (`mjwasm_forge.js` / `mjwasm_forge.wasm`) when present
  - carries the repository `LICENSE` and any existing `NOTICE*` / `notices/` material

- `dist-audit.zip`
  - audit/debug payload for maintainers
  - zip root contains `abi/`, `version.json`, `provenance.json`, and the same legal material

`version.json` keeps the legacy schema and points to the additive `provenance.json` manifest. The manifest records
the release id, asset kind, runtime-file SHA-256 map, and detected `single`/`pthreads` flavors. When a validated build
receipt is available under `deliverables/<ver>/abi/` (or is supplied with `--build-metadata`), it also records the
resolved upstream SHA and Emscripten SDK. Historical deliverables without a receipt retain explicit `null` fields and
`provenanceStatus: "unknown"`; the packager never derives these values from a version name or a local `C:\dev` path.

The local packaging command is:

```bash
python3 tools/package_release_assets.py \
  --dist-dir deliverables --version <ver> --dist-id forge-<ver>-rN \
  --git-sha <commit-sha> --out-dir release-assets \
  [--build-metadata deliverables/<ver>/abi/build_metadata.json]
```

Packaging is a release-asset step; it does not itself publish a tag or GitHub Release. The 3.9.0--3.15.0 upgrade
batch remains local until its deliverables are committed, verified, and intentionally released.

## Local reproduction of the CI verify step

Create a second clean checkout (or directory) and build into it, then run:

```bash
python3 forge_cli.py verify-dist --version <ver> --ci-build-dir ci-build
```
