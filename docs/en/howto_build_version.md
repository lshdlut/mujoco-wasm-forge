# How-to: build a specific MuJoCo version/ref

## Build

From the repo root (in WSL/Linux):

```bash
python3 forge_cli.py build --version 3.5.0
```

`--version` is passed to the upstream MuJoCo checkout under `external/mujoco`, so it can be:

- a released tag like `3.5.0`,
- a branch name,
- a commit hash.

## Outputs

Successful builds populate:

- `dist/<ver>/mujoco.js`
- `dist/<ver>/mujoco.wasm`
- `dist/<ver>/abi/*` (introspection + export/ABI manifests and reports)

## Build directory placement (recommended on synced filesystems)

By default, the forge CLI uses `<repo>/build` for intermediate build output. To move build trees elsewhere:

```bash
MJWF_BUILD_ROOT=/tmp/mjwf_build python3 forge_cli.py build --version 3.5.0
```

## Clean rebuild

If you want to force a full re-clone of upstream MuJoCo, remove `external/mujoco` and rebuild. The directory is not
tracked by git in this repo, and the forge CLI will recreate it on demand.

