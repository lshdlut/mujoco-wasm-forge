# How-to: integrate in Node.js

The generated `mujoco.js` is an ES module that loads `mujoco.wasm`. In Node.js you almost always need `locateFile`
to point to the on-disk `.wasm` path.

## Minimal example (Node ESM)

```js
import { fileURLToPath } from "node:url";
import load_mujoco from "./mujoco.js";

const base = new URL(".", import.meta.url);
const Module = await load_mujoco({
  locateFile: (p) => fileURLToPath(new URL(p, base)),
});

console.log("Loaded MuJoCo WASM module");
```

If your project uses CommonJS, consider switching this integration file to ESM (or use dynamic `import()`).

