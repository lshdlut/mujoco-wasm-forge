import path from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";
import assert from "node:assert/strict";
import fs from "node:fs";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const distDir = path.resolve(__dirname, "../dist/3.2.5");
const wasmURL = path.join(distDir, "mujoco.wasm");
const jsURL = path.join(distDir, "mujoco.js");

assert.ok(fs.existsSync(jsURL), "dist/3.2.5/mujoco.js missing");
assert.ok(fs.existsSync(wasmURL), "dist/3.2.5/mujoco.wasm missing");

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
const makeData = Module.cwrap("mjwf_mj_makeData", "number", ["number"]);
const resetData = Module.cwrap("mjwf_mj_resetData", null, ["number", "number"]);
const step = Module.cwrap("mjwf_mj_step", null, ["number", "number"]);
const deleteData = Module.cwrap("mjwf_mj_deleteData", null, ["number"]);
const deleteModel = Module.cwrap("mjwf_mj_deleteModel", null, ["number"]);

const stackTop = Module.stackSave();
const errBufSize = 1024;
const errBuf = Module.stackAlloc(errBufSize);
Module.HEAP8.fill(0, errBuf, errBuf + errBufSize);

const xml = `<?xml version="1.0"?>
<mujoco model="pendulum">
  <option timestep="0.002" gravity="0 0 -9.81"/>
  <worldbody>
    <body name="link" pos="0 0 0.1">
      <joint name="hinge" type="hinge" axis="0 1 0" damping="0.01"/>
      <geom type="capsule" fromto="0 0 0 0 0 0.2" size="0.02" density="1000"/>
    </body>
  </worldbody>
</mujoco>`;

const specPtr = parseXMLString(xml, 0, errBuf, errBufSize);
assert.notStrictEqual(specPtr, 0, `mj_parseXMLString failed: ${Module.UTF8ToString(errBuf)}`);

const modelPtr = compile(specPtr, 0);
assert.notStrictEqual(modelPtr, 0, "mj_compile returned null");

deleteSpec(specPtr);

const dataPtr = makeData(modelPtr);
assert.notStrictEqual(dataPtr, 0, "mj_makeData returned null");

resetData(modelPtr, dataPtr);
for (let i = 0; i < 16; i += 1) {
  step(modelPtr, dataPtr);
}

deleteData(dataPtr);
deleteModel(modelPtr);
Module.stackRestore(stackTop);

console.log("smoke(3.2.5) ok");
