# Glossary

| Term | Meaning in this project |
| --- | --- |
| ABI | The callable surface exposed by the WASM build: exported C symbols + wrapper conventions. |
| `dist/<ver>` | Build outputs for one MuJoCo version/ref. |
| `exports.lst` | The explicit export list consumed by Emscripten (`-sEXPORTED_FUNCTIONS=@...`). |
| wrapper | Generated `mjwf_*` functions that form a stable, reviewable surface. |
| introspection | Automated extraction from `mujoco.h` into JSON (functions/structs/enums + AST). |
| gate | A check that detects export/ABI drift or quality regressions (size/init time). |
| smoke test | A minimal runtime test that compiles and steps a tiny model to catch major regressions. |

