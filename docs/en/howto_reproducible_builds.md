# How-to: pin and reproduce builds

## Pin upstream MuJoCo

For strict reproducibility, prefer pinning a commit hash:

```bash
python3 forge_cli.py build --version <git-sha>
```

Also record your toolchain versions (Node, emsdk/emcc, Python).

## Verify a committed dist against a reproducible build

The forge CLI includes a verification command intended for CI-style checks:

```bash
python3 forge_cli.py verify-dist --version 3.5.0 --ci-build-dir ci-build
```

The idea is:

1. `ci-build/` builds `dist-ci` (or equivalent) from a clean checkout.
2. The repo root contains the committed `dist/<ver>`.
3. `verify-dist` normalizes metadata and diffs the two trees.

## CI helpers

- `python3 forge_cli.py collect-versions` (and `--github-output`) helps CI select which `dist/<ver>` directories to
  verify.

