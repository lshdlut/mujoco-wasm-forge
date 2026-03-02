# How-to：集成到 Node.js

生成的 `mujoco.js` 是 ES module，并会加载 `mujoco.wasm`。在 Node.js 下通常需要 `locateFile` 指向本地的
`.wasm` 文件路径。

## 最小示例（Node ESM）

```js
import { fileURLToPath } from "node:url";
import load_mujoco from "./mujoco.js";

const base = new URL(".", import.meta.url);
const Module = await load_mujoco({
  locateFile: (p) => fileURLToPath(new URL(p, base)),
});

console.log("Loaded MuJoCo WASM module");
```

如果你的项目仍是 CommonJS，建议将该集成文件改为 ESM（或使用动态 `import()`）。

