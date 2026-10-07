# Supported MuJoCo versions (committed deliverables/)

The build CLI accepts upstream MuJoCo refs; each new ref still requires validation:

```bash
python3 forge_cli.py build --version <mjver>
```

For convenience (and for CI `verify-dist`), validated `deliverables/<ver>/` baselines are committed to git.

## Historical committed baselines in `deliverables/`

- `3.3.7`
- `3.4.0`
- `3.5.0`
- `3.6.0`
- `3.7.0`
- `3.8.0`
- `3.8.1`

Historical pthreads artifacts are committed for `3.5.0` and `3.8.1`.

## Committed, CI-verified upgrade batch (2026-10-07)

The repository additionally has committed `3.9.0`, `3.10.0`, `3.11.0`, `3.12.0`, `3.13.0`,
`3.14.0`, and `3.15.0` artifacts, each with ordinary and pthreads variants. Every artifact
passed export checks, runtime/asset/plugin checks, scalar-width checks, and enforced
size/startup gates. Versions 3.14+ also passed an MJZ archive with a nested OBJ asset;
pthreads versions 3.10+ created an engine thread pool and stepped a model.

Fresh Linux CI rebuilt both flavors and passed strict byte-for-byte reproduction in the
[final 3.9.0--3.15.0 matrix](https://github.com/lshdlut/mujoco-wasm-forge/actions/runs/37609269055),
with the corresponding architecture gate passing in
[arch-review](https://github.com/lshdlut/mujoco-wasm-forge/actions/runs/37609269063).

These repository deliverables are the source for the matching tag-triggered release workflow;
the workflow publishes a GitHub Release only after the selected version's `verify-dist` and
release packaging jobs succeed. Existing 3.8.1 release files remain unchanged. The separate
3.8.1 compatibility runtime was rechecked without a final closing-ABI rebuild; it is not part
of the new 14-artifact reproduction matrix.
Downstream integration of new engine features is unverified, and passing these
build/runtime gates does not establish numerical equivalence across engine versions.

## Release tags (convention)

CI treats tags matching `forge-*` as release-like triggers, e.g.:

- `forge-3.4.0-r1`
- `forge-3.5.0-r1`

The checked MuJoCo 3.9+ release batch uses these matching tags:

- `forge-3.9.0-r1`
- `forge-3.10.0-r1`
- `forge-3.11.0-r1`
- `forge-3.12.0-r1`
- `forge-3.13.0-r1`
- `forge-3.14.0-r1`
- `forge-3.15.0-r1`
