import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import { pathToFileURL } from 'node:url';
import { distDir, distVersion } from '../dist_paths.mjs';
import { ensureNodeEnv } from './node_env.mjs';

ensureNodeEnv();
const root = distDir();
const factory = (await import(pathToFileURL(path.join(root, 'mujoco.js')).href)).default;
const printed = [];
const module = await factory({ locateFile: p => path.join(root, p.endsWith('.wasm') ? 'mujoco.wasm' : p),
  print: line => { printed.push(String(line)); console.log(line); } });
const make = module.cwrap('mjwf_helper_make_from_xml', 'number', ['string']);
module.FS.writeFile('/scalar-widths.xml', '<mujoco><worldbody><body><joint/><geom size=".1"/></body></worldbody></mujoco>');
const handle = make('/scalar-widths.xml');
assert.ok(handle > 0);
try {
  assert.equal(typeof module._mjwf_model_nq(handle), 'number');
  assert.equal(module._mjwf_model_nq(handle), 1);
  const size = module._mjwf_model_nbuffer_i64(handle);
  assert.equal(typeof size, 'bigint');
  assert.ok(size > 0n);
  // An invalid handle still has the declared i64 JS boundary.
  assert.equal(module._mjwf_model_nbuffer_i64(0), 0n);
  assert.equal(BigInt(module._mjwf_model_nbuffer(handle)), size);
  assert.equal(module._mjwf_model_nq_i64(handle), 1n);
  const arenaPtr = module._mjwf_data_maxuse_arena_ptr(handle);
  assert.ok(arenaPtr > 0);
  const arena = new BigInt64Array(module.HEAPU8.buffer, arenaPtr, 1);
  const saved = arena[0];
  const wide = (1n << 40n) + 123n;
  arena[0] = wide;
  assert.equal(arena[0], wide);
  assert.equal(new Uint32Array(module.HEAPU8.buffer, arenaPtr + 4, 1)[0], 256);
  arena[0] = saved;
  const abiRoot = process.env.MJWF_DIST_VARIANT ? path.dirname(root) : root;
  const abi = JSON.parse(fs.readFileSync(path.join(abiRoot, 'abi', 'structs_introspect_like.json')));
  const boolField = abi.structs.mjData.fields.find(f => f.name === 'flg_energypos');
  const flagPtr = module._mjwf_data_flg_energypos_ptr(handle);
  const neighborPtr = module._mjwf_data_flg_energyvel_ptr(handle);
  assert.ok(flagPtr > 0 && neighborPtr > flagPtr);
  const neighbor = module.HEAPU8[neighborPtr];
  module.HEAPU8[flagPtr] = 0;
  const modelPtr = module._mjwf_helper_model_ptr(handle);
  const dataPtr = module._mjwf_helper_data_ptr(handle);
  const warning = module.cwrap('mjwf_mju_warning', null, ['string']);
  const message = 'literal %s %d %n 100%';
  warning(message);
  assert.equal(module.cwrap('mjwf_helper_errmsg_last_global', 'string', [])(), message);
  if (typeof module._mjwf_mju_info === 'function') {
    module.cwrap('mjwf_mju_info', null, ['number', 'string'])(0, message);
    assert.ok(printed.some(line => line.includes(message)), 'mju_info must preserve literal percent text');
  }
  module._mjwf_mj_energyPos(modelPtr, dataPtr);
  assert.equal(module.HEAPU8[flagPtr], 1);
  assert.equal(module.HEAPU8[neighborPtr], neighbor);
  assert.equal(module._mjwf_scene_update_and_pack(handle, 7), module._mjwf_scene_ngeom(handle));
  const flags = module._mjwf_scene_geoms_transparent_ptr(handle);
  assert.ok(flags > 0);
  assert.equal(module.HEAPU8[flags], 0);
  const shared = module.HEAPU8.buffer instanceof SharedArrayBuffer;
  assert.equal(shared, process.env.MJWF_DIST_VARIANT === 'pthreads');
  let engineThreads = 0;
  if (shared && typeof module._mjwf_mju_threadpool === 'function') {
    module._mjwf_mju_threadpool(dataPtr, 2);
    const poolPtr = module._mjwf_data_threadpool_ptr(handle);
    assert.ok(module.HEAPU32[poolPtr >>> 2] > 0);
    for (let i = 0; i < 16; ++i) module._mjwf_mj_step(modelPtr, dataPtr);
    assert.ok(Number.isFinite(module.HEAPF64[module._mjwf_data_qpos_ptr(handle) >>> 3]));
    module._mjwf_mju_threadpool(dataPtr, 0);
    assert.equal(module.HEAPU32[poolPtr >>> 2], 0);
    engineThreads = 2;
  }
  console.log(JSON.stringify({ version: distVersion(), size: String(size), integer: 'Number', sizeType: 'BigInt', boolean: boolField?.type?.name, sharedMemory: shared, engineThreads }));
} finally {
  module._mjwf_helper_free(handle);
}
// Pthread pools are live Node workers; explicit exit is necessary after success.
if (process.env.MJWF_DIST_VARIANT === 'pthreads') process.exit(0);
