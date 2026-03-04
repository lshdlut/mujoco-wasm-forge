import path from "node:path";
import { pathToFileURL } from "node:url";
import assert from "node:assert/strict";
import fs from "node:fs";
import { distDir, distVersion } from "../dist_paths.mjs";
import { ensureNodeEnv } from "./node_env.mjs";

ensureNodeEnv();

const distRoot = distDir();
const wasmURL = path.join(distRoot, "mujoco.wasm");
const jsURL = path.join(distRoot, "mujoco.js");

const versionLabel = distVersion();
console.log(`helper-handle-lifecycle: using dist/${versionLabel}`);

assert.ok(fs.existsSync(jsURL), `dist/${versionLabel}/mujoco.js missing`);
assert.ok(fs.existsSync(wasmURL), `dist/${versionLabel}/mujoco.wasm missing`);

const modFactory = (await import(pathToFileURL(jsURL).href)).default;
const locateFile = (p) => {
  if (p.endsWith(".wasm")) return wasmURL;
  const cand = path.join(distRoot, p);
  return fs.existsSync(cand) ? cand : p;
};
const Module = await modFactory({ locateFile });
if (Module.ready) await Module.ready;

const makeFromXml = Module.cwrap("mjwf_helper_make_from_xml", "number", ["string"]);
const freeHandle = Module.cwrap("mjwf_helper_free", null, ["number"]);
const validHandle = Module.cwrap("mjwf_helper_valid", "number", ["number"]);
const modelPtrOf = Module.cwrap("mjwf_helper_model_ptr", "number", ["number"]);
const dataPtrOf = Module.cwrap("mjwf_helper_data_ptr", "number", ["number"]);

const xml = `<?xml version="1.0"?>
<mujoco model="pendulum">
  <worldbody>
    <body name="b" pos="0 0 0">
      <joint name="hinge" type="hinge" axis="0 1 0"/>
      <geom type="capsule" fromto="0 0 0 0 0 1" size="0.05"/>
    </body>
  </worldbody>
</mujoco>`;

try {
  Module.FS.mkdir("/models");
} catch (err) {
  if (err?.errno !== 17) throw err;
}
const fsXmlPath = "/models/pendulum.xml";
Module.FS.writeFile(fsXmlPath, xml, { canRead: true, canWrite: true });

const h = makeFromXml(fsXmlPath) | 0;
assert.ok(h > 0, `expected make_from_xml to succeed; got handle=${h}`);
assert.equal(validHandle(h) | 0, 1, "expected mjwf_helper_valid(handle)=1 after creation");

const mp = modelPtrOf(h) | 0;
const dp = dataPtrOf(h) | 0;
assert.ok(mp !== 0, "expected non-null model pointer for a valid handle");
assert.ok(dp !== 0, "expected non-null data pointer for a valid handle");

freeHandle(h);
assert.equal(validHandle(h) | 0, 0, "expected mjwf_helper_valid(handle)=0 after free");
assert.equal(modelPtrOf(h) | 0, 0, "expected model_ptr(handle)=0 after free");
assert.equal(dataPtrOf(h) | 0, 0, "expected data_ptr(handle)=0 after free");

// Must not crash: double free and freeing invalid handles.
freeHandle(h);
freeHandle(0);
freeHandle(-1);
freeHandle(123456);

assert.equal(validHandle(h) | 0, 0, "expected mjwf_helper_valid(handle) to remain 0 after double free");
console.log("mjwf_helper handle lifecycle OK");

