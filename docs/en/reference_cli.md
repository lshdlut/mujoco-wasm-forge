# CLI reference (`forge_cli.py`)

All commands are invoked from the repo root.

## `build`

Build `dist/<ver>`:

```bash
python3 forge_cli.py build --version 3.5.0
```

Build the pthreads variant (browsers require COOP/COEP + `SharedArrayBuffer`):

```bash
python3 forge_cli.py build --version 3.5.0 --pthreads --fresh-dependency
```

Run smoke + quality gates after building:

```bash
python3 forge_cli.py build --version 3.5.0 --with-checks
```

Existing dirty dependencies are preserved by default and stop the build. For a
new version, variant, or rebuild, pass `--fresh-dependency`: the complete existing
checkout moves to `<build-root>/dependency-snapshots/mujoco-<unique-id>` before a
fresh clone is prepared. A failed clone leaves that recoverable snapshot intact.

Dimension accessors keep the legacy i32/JavaScript Number contract. Values outside
the signed i32 range return `-1` with a diagnostic; dimensions typed as `mjtSize`
also provide `<name>_i64` accessors returning exact JavaScript BigInt. Direct
`mjtSize` pointer views require `BigInt64Array`, and `mjtBool` views use `Uint8Array`.

## `collect-versions`

List `dist/*` versions:

```bash
python3 forge_cli.py collect-versions
```

Emit GitHub Actions-friendly outputs:

```bash
python3 forge_cli.py collect-versions --github-output
```

## `verify-dist`

Verify the committed `deliverables/<ver>` matches a reproducible build checkout:

```bash
python3 forge_cli.py verify-dist --version 3.5.0 --ci-build-dir ci-build
```

## Exit status

The CLI exits non-zero if any stage fails (toolchain errors, build failures, or gate/check failures).
