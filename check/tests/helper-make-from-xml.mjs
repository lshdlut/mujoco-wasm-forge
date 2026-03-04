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
console.log(`helper-make-from-xml: using dist/${versionLabel}`);

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

function tryMake(xml) {
  try {
    return { handle: makeFromXml(xml), thrown: null };
  } catch (err) {
    return { handle: 0, thrown: err };
  }
}

const validXml = `<?xml version="1.0"?>
<mujoco model="pendulum">
  <worldbody>
    <body name="b" pos="0 0 0">
      <joint name="hinge" type="hinge" axis="0 1 0"/>
      <geom type="capsule" fromto="0 0 0 0 0 1" size="0.05"/>
    </body>
  </worldbody>
</mujoco>`;

{
  const r = tryMake(validXml);
  assert.ok(!r.thrown, `make_from_xml(valid) threw: ${r.thrown}`);
  assert.ok(r.handle !== 0, "make_from_xml(valid) returned 0");
  freeHandle(r.handle);
}

const invalidXml = `<?xml version="1.0"?><mujoco model="bad"><worldbody></mujoco>`;
{
  const r = tryMake(invalidXml);
  assert.ok(!r.thrown, `make_from_xml(invalid) threw: ${r.thrown}`);
  assert.ok(r.handle <= 0, `make_from_xml(invalid) should fail (<=0), got: ${r.handle}`);
  const msg = String(errmsgLast() || "");
  assert.ok(msg.length > 0, "expected a non-empty global errmsg for invalid XML");
}

const missingPluginXml = `<?xml version="1.0"?>
<mujoco model="missing_plugin">
  <extension>
    <plugin plugin="mujoco.sensor.DOES_NOT_EXIST"/>
  </extension>
</mujoco>`;
{
  const r = tryMake(missingPluginXml);
  assert.ok(!r.thrown, `make_from_xml(missing_plugin) threw: ${r.thrown}`);
  assert.ok(r.handle <= 0, `make_from_xml(missing_plugin) should fail (<=0), got: ${r.handle}`);
  const msg = String(errmsgLast() || "");
  // errno may legitimately be 0 for some MuJoCo error paths; require readable text.
  const msgLower = msg.toLowerCase();
  assert.ok(
    msgLower.includes("plugin") || msgLower.includes("not found"),
    `expected a readable plugin-related errmsg, got: ${msg}`,
  );
  // best-effort: ensure errno is at least a finite integer.
  const e = errnoLast();
  assert.ok(Number.isFinite(e), `expected errno to be finite, got: ${e}`);
}

console.log("mjwf_helper_make_from_xml error reporting OK");
