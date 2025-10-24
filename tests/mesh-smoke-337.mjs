import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import assert from 'node:assert/strict';
import fs from 'node:fs';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const distDir = path.resolve(__dirname, '../dist');
const wasmURL = path.join(distDir, 'mujoco-3.3.7.wasm');
const jsURL = path.join(distDir, 'mujoco-3.3.7.js');

assert.ok(fs.existsSync(jsURL), 'dist/mujoco-3.3.7.js missing');
assert.ok(fs.existsSync(wasmURL), 'dist/mujoco-3.3.7.wasm missing');

const modFactory = (await import(pathToFileURL(jsURL).href)).default;
const Module = await modFactory({ locateFile: (p) => (p.endsWith('.wasm') ? wasmURL : p) });

// Inline a small tetrahedron mesh to exercise qhull-dependent code paths at load time.
// No contacts: keep it far from the ground to focus on load/compile-time processing.
const xml = `<?xml version="1.0"?>
<mujoco model="mesh_smoke">
  <option timestep="0.002" gravity="0 0 -9.81"/>
  <asset>
    <mesh name="tetra" vertex="0 0 0  0.1 0 0  0 0.1 0  0 0 0.1" face="0 1 2  0 1 3  0 2 3  1 2 3"/>
  </asset>
  <worldbody>
    <body name="b" pos="0 0 2">
      <geom type="mesh" mesh="tetra" density="1000"/>
    </body>
  </worldbody>
  <visual><scale forcewidth="0.002"/></visual>
</mujoco>`;

Module.FS.writeFile('/mesh_model.xml', new TextEncoder().encode(xml));
const init = Module.cwrap('mjw_init','number',['string']);
const step_demo = Module.cwrap('mjw_step_demo', null, ['number']);
const qpos0 = Module.cwrap('mjw_qpos0','number',[]);

assert.equal(init('/mesh_model.xml'), 1, 'mesh model init failed');
const before = qpos0();
step_demo(50);
const after = qpos0();
assert.ok(Number.isFinite(after), 'qpos0 after step not finite');
console.log('mesh smoke OK: qpos0 before=%d after=%d', before, after);

