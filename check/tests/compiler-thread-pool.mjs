// Exercise enough independent mesh tasks to exceed a four-worker pool on
// high-core-count hosts. This must complete rather than blocking worker startup.
import assert from 'node:assert/strict';
import path from 'node:path';
import { pathToFileURL } from 'node:url';
import { distDir, distVersion } from '../dist_paths.mjs';
import { ensureNodeEnv } from './node_env.mjs';

ensureNodeEnv();
const root = distDir();
const factory = (await import(pathToFileURL(path.join(root, 'mujoco.js')).href)).default;
const module = await factory({ locateFile: p => path.join(root, p.endsWith('.wasm') ? 'mujoco.wasm' : p) });
const tetra = 'v 0 0 0\nv 1 0 0\nv 0 1 0\nv 0 0 1\nf 1 3 2\nf 1 2 4\nf 1 4 3\nf 2 3 4\n';
let assets = '', bodies = '';
for (let i = 0; i < 8; ++i) {
  module.FS.writeFile(`/mesh-${i}.obj`, tetra);
  assets += `<mesh name="m${i}" file="/mesh-${i}.obj"/>`;
  bodies += `<body pos="${i * 3} 0 0"><freejoint/><geom type="mesh" mesh="m${i}"/></body>`;
}
module.FS.writeFile('/compiler-pool.xml', `<mujoco><asset>${assets}</asset><worldbody>${bodies}</worldbody></mujoco>`);
const handle = module.cwrap('mjwf_helper_make_from_xml', 'number', ['string'])('/compiler-pool.xml');
assert.ok(handle > 0, module.cwrap('mjwf_helper_errmsg_last_global', 'string', [])());
try {
  assert.equal(module._mjwf_model_nmesh(handle), 8);
  module._mjwf_mj_step(module._mjwf_helper_model_ptr(handle), module._mjwf_helper_data_ptr(handle));
} finally {
  module._mjwf_helper_free(handle);
}
console.log(`compiler-thread-pool(${distVersion()}): PASS (8 independent mesh tasks, hardwareConcurrency=${navigator.hardwareConcurrency})`);
