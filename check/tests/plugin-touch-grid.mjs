import path from "node:path";
import { pathToFileURL } from "node:url";
import assert from "node:assert/strict";
import fs from "node:fs";
import { distDir, distVersion } from "../dist_paths.mjs";

const wasmURL = path.join(distDir(), "mujoco.wasm");
const jsURL = path.join(distDir(), "mujoco.js");

const versionLabel = distVersion();
console.log(`plugin-touch-grid: using dist/${versionLabel}`);

assert.ok(fs.existsSync(jsURL), `dist/${versionLabel}/mujoco.js missing`);
assert.ok(fs.existsSync(wasmURL), `dist/${versionLabel}/mujoco.wasm missing`);

const modFactory = (await import(pathToFileURL(jsURL).href)).default;
const Module = await modFactory({ locateFile: (p) => (p.endsWith(".wasm") ? wasmURL : p) });
if (Module.ready) await Module.ready;

const parseXMLString = Module.cwrap(
  "mjwf_mj_parseXMLString",
  "number",
  ["string", "number", "number", "number"],
);
const compile = Module.cwrap("mjwf_mj_compile", "number", ["number", "number"]);
const deleteSpec = Module.cwrap("mjwf_mj_deleteSpec", null, ["number"]);
const deleteModel = Module.cwrap("mjwf_mj_deleteModel", null, ["number"]);

const stackTop = Module.stackSave();
const errBufSize = 1024;
const errBuf = Module.stackAlloc(errBufSize);
Module.HEAP8.fill(0, errBuf, errBuf + errBufSize);

const xml = `<?xml version="1.0"?>
<mujoco model="touch_grid_smoke">
  <compiler autolimits="true"/>
  <extension>
    <plugin plugin="mujoco.sensor.touch_grid"/>
  </extension>
  <worldbody>
    <body name="b" pos="0 0 0">
      <geom type="sphere" size="0.1"/>
      <site name="touch" pos="0 0 0"/>
    </body>
  </worldbody>
  <sensor>
    <plugin name="touch" plugin="mujoco.sensor.touch_grid" objtype="site" objname="touch">
      <config key="size" value="2 2"/>
      <config key="fov" value="45 45"/>
      <config key="gamma" value="0"/>
      <config key="nchannel" value="1"/>
    </plugin>
  </sensor>
</mujoco>`;

const specPtr = parseXMLString(xml, 0, errBuf, errBufSize);
assert.notStrictEqual(specPtr, 0, `mj_parseXMLString(touch_grid) failed: ${Module.UTF8ToString(errBuf)}`);

let modelPtr = 0;
try {
  modelPtr = compile(specPtr, 0);
} catch (err) {
  throw new Error(`mj_compile(touch_grid) threw: ${err}`);
}
assert.notStrictEqual(modelPtr, 0, "mj_compile(touch_grid) returned null");

deleteSpec(specPtr);
deleteModel(modelPtr);
Module.stackRestore(stackTop);

console.log("touch_grid plugin OK");

