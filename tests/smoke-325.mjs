import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import assert from 'node:assert/strict';
import fs from 'node:fs';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const distDir = path.resolve(__dirname, '../dist');
const wasmURL = path.join(distDir, 'mujoco-3.2.5.wasm');
const jsURL = path.join(distDir, 'mujoco-3.2.5.js');

assert.ok(fs.existsSync(jsURL), 'dist/mujoco-3.2.5.js missing');
assert.ok(fs.existsSync(wasmURL), 'dist/mujoco-3.2.5.wasm missing');

const modFactory = (await import(pathToFileURL(jsURL).href)).default;
const Module = await modFactory({
  locateFile: (p) => (p.endsWith('.wasm') ? wasmURL : p),
});

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

Module.FS.writeFile('/model.xml', new TextEncoder().encode(xml));
const init = Module.cwrap('mjw_init','number',['string']);
const step_demo = Module.cwrap('mjw_step_demo', null, ['number']);
const nq = Module.cwrap('mjw_nq','number',[]);
const qpos0 = Module.cwrap('mjw_qpos0','number',[]);

if (init('/model.xml') !== 1) { throw new Error('init failed'); }
if (nq() < 0) { throw new Error('nq invalid'); }
const before = qpos0();
step_demo(200);
const after = qpos0();
if (!Number.isFinite(after)) { throw new Error('qpos0 not finite'); }
console.log('qpos0 before=%d, after=%d', before, after);
