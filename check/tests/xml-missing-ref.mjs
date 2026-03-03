import path from "node:path";
import { pathToFileURL } from "node:url";
import assert from "node:assert/strict";
import fs from "node:fs";
import { distDir, distVersion } from "../dist_paths.mjs";
import { ensureNodeEnv } from "./node_env.mjs";

ensureNodeEnv();

const versionLabel = distVersion();
const distRoot = distDir();
const wasmURL = path.join(distRoot, "mujoco.wasm");
const jsURL = path.join(distRoot, "mujoco.js");

console.log(`xml-missing-ref: using dist/${versionLabel}`);

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

try {
  Module.FS.mkdir("/models");
} catch (err) {
  if (err?.errno !== 17) throw err;
}

const xml = `<?xml version="1.0"?>
<mujoco model="outer">
  <asset>
    <model file="missing.xml"/>
  </asset>
  <worldbody>
    <body name="root"/>
  </worldbody>
</mujoco>`;

const fsXmlPath = "/models/outer.xml";
Module.FS.writeFile(fsXmlPath, xml, { canRead: true, canWrite: true });

let handle = 0;
handle = Module.ccall("mjwf_helper_make_from_xml", "number", ["string"], [fsXmlPath]) | 0;
assert.equal(handle, -1, `expected mjwf_helper_make_from_xml to return -1; got handle=${handle}`);

const errnoLast = Module.ccall("mjwf_helper_errno_last_global", "number", [], []) | 0;
const msg = Module.ccall("mjwf_helper_errmsg_last_global", "string", [], []) ?? "";
assert.notEqual(errnoLast, 0, "expected mjwf_helper_errno_last_global to be non-zero");
assert.ok(msg && msg.trim().length > 0, "expected non-empty error message");
assert.ok(
  msg.toLowerCase().includes("missing.xml") || msg.toLowerCase().includes("missing"),
  `expected error message to mention missing.xml; got: ${msg}`,
);

console.log(`xml-missing-ref(${versionLabel}) ok`);
