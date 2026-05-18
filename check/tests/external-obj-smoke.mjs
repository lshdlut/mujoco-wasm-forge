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
console.log(`external-obj-smoke: using dist/${versionLabel}`);

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

const baseDir = "/mem/external_obj_smoke";
ensureDir(baseDir);

const obj = [
  "v 0 0 0",
  "v 1 0 0",
  "v 0 1 0",
  "v 0 0 1",
  "f 1 2 3",
  "f 1 2 4",
  "f 1 3 4",
  "f 2 3 4",
  "",
].join("\n");
Module.FS.writeFile(`${baseDir}/mesh.obj`, obj, { encoding: "utf8" });

const xml = `<?xml version="1.0"?>
<mujoco model="external_obj_smoke">
  <asset>
    <mesh name="tetra" file="mesh.obj"/>
  </asset>
  <worldbody>
    <geom type="mesh" mesh="tetra" density="1000"/>
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

console.log("external OBJ smoke OK");
