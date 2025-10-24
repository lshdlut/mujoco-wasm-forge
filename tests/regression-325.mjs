// Regression test: compare WASM vs native outputs (qpos0/qvel0 series)
// - Builds and runs native harness to produce golden vectors
// - Runs WASM module and collects same signals
// - Compares within tolerance

import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import fs from 'node:fs';
import assert from 'node:assert/strict';
import { execFile } from 'node:child_process';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const rootDir = path.resolve(__dirname, '..');
const distDir = path.resolve(rootDir, 'dist');

const wasmURL = path.join(distDir, 'mujoco-3.2.5.wasm');
const jsURL = path.join(distDir, 'mujoco-3.2.5.js');

// Path to native harness from workflow build
const nativeBin = process.env.MJ_NATIVE_BIN || path.resolve(rootDir, 'build', '325_native', 'mujoco_compare325');
const steps = Number(process.env.MJ_STEPS || 200);
const tol = Number(process.env.MJ_TOL || 1e-8);

// Small pendulum model (same as smoke)
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

// 1) Produce native golden vectors
const tmpXml = path.resolve(rootDir, 'tests', 'tmp_model.xml');
fs.writeFileSync(tmpXml, xml, 'utf8');

const nativeOut = await new Promise((resolve, reject) => {
  execFile(nativeBin, [tmpXml, String(steps)], { cwd: rootDir }, (err, stdout, stderr) => {
    if (err) {
      err.message += `\nSTDERR:\n${stderr}`;
      return reject(err);
    }
    resolve(stdout);
  });
});

let golden;
try {
  golden = JSON.parse(nativeOut);
} catch (e) {
  throw new Error(`Failed to parse native JSON: ${e.message}`);
}
assert.ok(Array.isArray(golden.qpos0) && golden.qpos0.length === steps, 'native qpos0 length mismatch');
assert.ok(Array.isArray(golden.qvel0) && golden.qvel0.length === steps, 'native qvel0 length mismatch');

// 2) Run WASM and collect series
assert.ok(fs.existsSync(jsURL), 'dist JS missing');
assert.ok(fs.existsSync(wasmURL), 'dist WASM missing');

const modFactory = (await import(pathToFileURL(jsURL).href)).default;
const Module = await modFactory({ locateFile: (p) => (p.endsWith('.wasm') ? wasmURL : p) });

Module.FS.writeFile('/model.xml', new TextEncoder().encode(xml));
const init = Module.cwrap('mjw_init','number',['string']);
const step_demo = Module.cwrap('mjw_step_demo', null, ['number']);
const qpos0 = Module.cwrap('mjw_qpos0','number',[]);
const qvel0 = Module.cwrap('mjw_qvel0','number',[]);

if (init('/model.xml') !== 1) throw new Error('WASM init failed');

const js_qpos0 = [];
for (let i = 0; i < steps; i++) {
  step_demo(1);
  js_qpos0.push(qpos0());
}

// re-init for qvel series alignment
if (init('/model.xml') !== 1) throw new Error('WASM re-init failed');
const js_qvel0 = [];
for (let i = 0; i < steps; i++) {
  step_demo(1);
  js_qvel0.push(qvel0());
}

// 3) Compare with tolerance
const maxAbsDiff = (a, b) => a.reduce((mx, v, i) => Math.max(mx, Math.abs(v - b[i])), 0);
const posDiff = maxAbsDiff(js_qpos0, golden.qpos0);
const velDiff = maxAbsDiff(js_qvel0, golden.qvel0);

console.log(`regression: posDiff=${posDiff}, velDiff=${velDiff}, tol=${tol}`);
if (!(posDiff <= tol && velDiff <= tol)) {
  throw new Error(`Regression failed: diffs exceed tolerance (pos=${posDiff}, vel=${velDiff}, tol=${tol})`);
}

console.log('regression OK');

