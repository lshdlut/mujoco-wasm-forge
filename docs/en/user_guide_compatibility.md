# Compatibility model

`mujoco-wasm-forge` treats each MuJoCo version/ref as a separate, versioned output under `dist/<ver>/`.

## What “compatibility” means here

When upgrading MuJoCo, you should expect changes in:

- header declarations (introspection output),
- available implementations (symbol inventory),
- wrapper surface (`mjwf_*` exports),
- performance/size characteristics (quality gates).

The “compatibility promise” is not that MuJoCo never changes, but that changes are:

- **detectable** (gates fail or report drift),
- **reviewable** (audit artifacts in `dist/<ver>/abi/`),
- **actionable** (you can update wrappers/exports with a clear diff).

## How upgrades are validated

Recommended flow:

1. Build the new ref: `python3 forge_cli.py build --version <new> --with-checks`.
2. Review `dist/<new>/abi/exports.lst` and the reports under `dist/<new>/abi/`.
3. Run the checks in “enforced” mode when needed (see `reference_env_vars` and `reference_checks_gates`).

Common failure categories:

- **Build/toolchain**: Emscripten/Node/clang is missing or incompatible.
- **ABI/export drift**: a required wrapper export disappeared or changed.
- **Runtime regression**: smoke tests fail to compile/step a minimal model.
- **Quality regressions**: `check/tests/gates.mjs` reports size/init-time breaches.

