# mujoco-wasm-forge documentation

`mujoco-wasm-forge` is a toolchain for producing MuJoCo WebAssembly builds and keeping their exports/ABI under control.
It builds a specific upstream MuJoCo ref into `dist/<ver>/mujoco.js` + `dist/<ver>/mujoco.wasm`, plus audit artifacts
under `dist/<ver>/abi/`.

## 30-second summary

- If you just want artifacts: use a committed `dist/<ver>/` or run `python forge_cli.py build --version <mjver>`.
- Exports are explicitly defined by `dist/<ver>/abi/exports.lst` and enforced at link time.
- `--with-checks` runs smoke + quality gates (`check/tests/*.mjs`) against the produced dist.
- Maintainers can use the ABI artifacts to audit diffs when upgrading MuJoCo.

## Mini glossary

| Term | Plain meaning |
| --- | --- |
| `dist/<ver>` | A build output directory for one MuJoCo version/ref. |
| `exports.lst` | The single source of truth for exported C symbols (fed to Emscripten `-sEXPORTED_FUNCTIONS=@...`). |
| wrapper | `mjwf_*` functions that form the supported C API surface for JS/Node/WebWorker. |
| introspect | Header/AST-based extraction of functions/structs/enums from `mujoco.h`. |
| gate | A check that fails (or warns) when ABI/exports/quality constraints are violated. |

## Where to start

- New users: read `quickstart`, then `howto_build_version`.
- Integrators: `howto_web_worker` and `howto_node`.
- Maintainers: `user_guide_pipeline`, `reference_abi_contract`, and `reference_checks_gates`.

```{toctree}
:maxdepth: 2
:caption: Quickstart

quickstart
```

```{toctree}
:maxdepth: 2
:caption: User Guide

user_guide_overview
user_guide_pipeline
user_guide_artifacts
user_guide_compatibility
```

```{toctree}
:maxdepth: 2
:caption: How-to

howto_build_version
howto_build_with_checks
howto_web_worker
howto_node
howto_reproducible_builds
howto_audit_abi
```

```{toctree}
:maxdepth: 2
:caption: Reference

reference_cli
reference_env_vars
reference_dist_layout
reference_abi_contract
reference_checks_gates
supported_versions
glossary
```

```{toctree}
:maxdepth: 2
:caption: Developer

developer_contributing
developer_repo_tour
developer_add_version
developer_modify_generation
developer_ci_release
```

