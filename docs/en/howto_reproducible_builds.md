# How-to: pin and reproduce builds

## Pin upstream MuJoCo

For strict reproducibility, prefer pinning a commit hash:

```bash
python3 forge_cli.py build --version <git-sha>
```

Also record your toolchain versions (Node, emsdk/emcc, Python).

Install Ninja (or set `MJWF_NINJA` to its executable); the current recipe explicitly
uses Ninja on every host. CMake Makefiles and Ninja can order OBJECT-library
archive members differently and produce different WASM bytes despite identical
runtime behavior. Receipts check the actual generator. For an existing Makefiles
cache, select a new owned directory with `MJWF_BUILD_ROOT`; preserve the old tree
rather than resetting or deleting it.

## Verify a committed deliverable against a reproducible build

The forge CLI includes a verification command intended for CI-style checks:

```bash
python3 forge_cli.py verify-dist --version 3.15.0 --ci-build-dir ci-build
```

The idea is:

1. `ci-build/` builds ordinary `dist/3.15.0` with `--with-checks`, then the pthreads variant with `--pthreads --fresh-dependency --with-checks`.
2. The repo root contains the committed `deliverables/<ver>` baseline.
3. `verify-dist` normalizes metadata and diffs the two trees.

Use the matching Forge source recipe and Emscripten 4.0.10 for the new checked
baselines. Compile-time source prefix maps keep structured log locations relative
to the repository; per-flavor audit reports remain bound to the actual WASM SHA.
Historical baselines were not retrofitted: reproduce them in a separate workspace
using their matching historical Forge tag/commit, SDK, and verification command,
not the current generator and audit schema.

## CI helpers

- `python3 forge_cli.py collect-versions` (and `--github-output`) helps CI select which `deliverables/<ver>` directories to
  verify.
