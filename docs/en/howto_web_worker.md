# How-to: integrate in a Web Worker

The output `dist/<ver>/mujoco.js` is an Emscripten-built ES module (`-sEXPORT_ES6 -sMODULARIZE`) and expects to load
`mujoco.wasm`. You typically need to provide `locateFile` so the module can find the `.wasm` file.

## Minimal module worker example

Assuming `mujoco.js` and `mujoco.wasm` are served from the same directory:

```js
import load_mujoco from "./mujoco.js";

const base = new URL(".", import.meta.url);
const Module = await load_mujoco({
  locateFile: (p) => new URL(p, base).toString(),
});

// Module.ccall / Module.cwrap are available for calling exported C functions.
```

## Common pitfalls

- If your bundler moves/renames the `.wasm` asset, `locateFile` must be updated accordingly.
- Worker type matters: the example assumes a **module worker** so `import` and `import.meta.url` work.

