# How-to：集成到 Web Worker

`dist/<ver>/mujoco.js` 是 Emscripten 输出的 ES module（`-sEXPORT_ES6 -sMODULARIZE`），运行时需要加载
`mujoco.wasm`。通常需要提供 `locateFile` 来告诉模块 `.wasm` 的位置。

## 最小 module worker 示例

假设 `mujoco.js` 和 `mujoco.wasm` 位于同一目录并可被正常访问：

```js
import load_mujoco from "./mujoco.js";

const base = new URL(".", import.meta.url);
const Module = await load_mujoco({
  locateFile: (p) => new URL(p, base).toString(),
});

// Module.ccall / Module.cwrap 可用于调用导出的 C 函数。
```

## 常见坑

- bundler 改了 `.wasm` 的路径/文件名时，必须同步更新 `locateFile`。
- Worker 类型要一致：上述示例假设是 **module worker**（支持 `import` 和 `import.meta.url`）。

