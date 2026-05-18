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
console.log(`external-stl-smoke: using dist/${versionLabel}`);

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

function makeBinaryStl() {
  const faces = [
    [[0, 0, 0], [1, 0, 0], [0, 1, 0]],
    [[0, 0, 0], [0, 0, 1], [1, 0, 0]],
    [[0, 0, 0], [0, 1, 0], [0, 0, 1]],
    [[1, 0, 0], [0, 0, 1], [0, 1, 0]],
  ];
  const buf = Buffer.alloc(84 + faces.length * 50);
  buf.write("mjwf external STL smoke", 0, "ascii");
  buf.writeUInt32LE(faces.length, 80);
  let offset = 84;
  for (const face of faces) {
    offset += 12;
    for (const vertex of face) {
      for (const value of vertex) {
        buf.writeFloatLE(value, offset);
        offset += 4;
      }
    }
    buf.writeUInt16LE(0, offset);
    offset += 2;
  }
  return buf;
}

const baseDir = "/mem/external_stl_smoke";
ensureDir(baseDir);
Module.FS.writeFile(`${baseDir}/mesh.stl`, makeBinaryStl());

const xml = `<?xml version="1.0"?>
<mujoco model="external_stl_smoke">
  <asset>
    <mesh name="tetra" file="mesh.stl"/>
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

console.log("external STL smoke OK");
