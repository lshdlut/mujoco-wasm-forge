import path from "node:path";
import { pathToFileURL } from "node:url";
import assert from "node:assert/strict";
import fs from "node:fs";
import { distDir, distVersion } from "../dist_paths.mjs";

const wasmURL = path.join(distDir(), "mujoco.wasm");
const jsURL = path.join(distDir(), "mujoco.js");

const versionLabel = distVersion();
console.log(`mesh-texture-smoke: using dist/${versionLabel}`);

assert.ok(fs.existsSync(jsURL), `dist/${versionLabel}/mujoco.js missing`);
assert.ok(fs.existsSync(wasmURL), `dist/${versionLabel}/mujoco.wasm missing`);

const modFactory = (await import(pathToFileURL(jsURL).href)).default;
const Module = await modFactory({ locateFile: (p) => (p.endsWith(".wasm") ? wasmURL : p) });
if (Module.ready) await Module.ready;

const makeFromXml = Module.cwrap("mjwf_helper_make_from_xml", "number", ["string"]);
const freeHandle = Module.cwrap("mjwf_helper_free", null, ["number"]);
const errnoLast = Module.cwrap("mjwf_helper_errno_last_global", "number", []);
const errmsgLast = Module.cwrap("mjwf_helper_errmsg_last_global", "string", []);

function ensureDir(p) {
  if (Module.FS.analyzePath(p).exists) return;
  if (typeof Module.FS.mkdirTree === "function") {
    Module.FS.mkdirTree(p);
    return;
  }
  const parts = p.split("/").filter(Boolean);
  let cur = "";
  for (const part of parts) {
    cur += `/${part}`;
    if (!Module.FS.analyzePath(cur).exists) {
      Module.FS.mkdir(cur);
    }
  }
}

const baseDir = "/mem/mesh_texture_smoke";
ensureDir(baseDir);

const obj = [
  "v 0 0 0",
  "v 1 0 0",
  "v 0 1 0",
  "v 0 0 1",
  "vt 0 0",
  "vt 1 0",
  "vt 0 1",
  "vt 1 1",
  "f 1/1 2/2 3/3",
  "f 1/1 2/2 4/4",
  "f 1/1 3/3 4/4",
  "f 2/2 3/3 4/4",
  "",
].join("\n");
Module.FS.writeFile(`${baseDir}/mesh.obj`, obj, { encoding: "utf8" });

// 1x1 RGBA PNG.
const png = Buffer.from(
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR4nGP4z8DwHwAFAAH/iZk9HQAAAABJRU5ErkJggg==",
  "base64",
);
Module.FS.writeFile(`${baseDir}/tex.png`, png);

const xml = `<?xml version="1.0"?>
<mujoco model="mesh_texture_smoke">
  <asset>
    <texture name="tex" type="2d" file="tex.png"/>
    <material name="mat" texture="tex" texrepeat="1 1" texuniform="true"/>
    <mesh name="tri" file="mesh.obj"/>
  </asset>
  <worldbody>
    <geom type="mesh" mesh="tri" material="mat" density="1000"/>
  </worldbody>
</mujoco>`;
Module.FS.writeFile(`${baseDir}/model.xml`, xml, { encoding: "utf8" });

let handle = -1;
try {
  handle = makeFromXml(`${baseDir}/model.xml`);
} catch (err) {
  throw new Error(`mjwf_helper_make_from_xml threw: ${err}`);
}
assert.ok(
  handle > 0,
  `mjwf_helper_make_from_xml failed: errno=${errnoLast()} errmsg=${errmsgLast()}`,
);
freeHandle(handle);

console.log("mesh texture smoke OK");
