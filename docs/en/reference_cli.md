# CLI reference (`forge_cli.py`)

All commands are invoked from the repo root.

## `build`

Build `dist/<ver>`:

```bash
python3 forge_cli.py build --version 3.5.0
```

Run smoke + quality gates after building:

```bash
python3 forge_cli.py build --version 3.5.0 --with-checks
```

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

Verify the committed `dist/<ver>` matches a reproducible build checkout:

```bash
python3 forge_cli.py verify-dist --version 3.5.0 --ci-build-dir ci-build
```

## Exit status

The CLI exits non-zero if any stage fails (toolchain errors, build failures, or gate/check failures).

