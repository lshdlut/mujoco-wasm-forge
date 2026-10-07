import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
import {distDir, distVersion} from '../dist_paths.mjs';
import {ensureNodeEnv} from './node_env.mjs';

ensureNodeEnv();
const root = distDir();
const abi = path.join(process.env.MJWF_DIST_VARIANT ? path.dirname(root) : root, 'abi');
const capabilities = JSON.parse(fs.readFileSync(path.join(abi, 'js_signature_capabilities.json')));
const factory = (await import(pathToFileURL(path.join(root, 'mujoco.js')))).default;
const m = await factory({locateFile: p => path.join(root, p.endsWith('.wasm') ? 'mujoco.wasm' : p)});
const allocated = [];
function alloc(size) {const p = m._malloc(size); assert.ok(p); allocated.push(p); m.HEAPU8.fill(0,p,p+size); return p;}
function offset(type, field) {return m['_mjwf_offsetof_'+type+'_'+field]();}
function size(type) {return m['_mjwf_sizeof_'+type]();}
function bytes(text) {const p=alloc(m.lengthBytesUTF8(text)+1); m.stringToUTF8(text,p,m.lengthBytesUTF8(text)+1); return p;}
const view = new DataView(m.HEAPU8.buffer);
let handle = 0;
try {
  const camera = capabilities.functions.mjv_averageCamera;
  assert.equal(camera.rawCall, 'needs-aggregate-adapter');
  assert.equal(camera.adapter.symbol, 'mjwf_mjv_averageCamera_out');
  assert.equal(typeof m._mjwf_mjv_averageCamera, 'function', 'raw signature must remain exported');
  const c1=alloc(size('mjvGLCamera')), c2=alloc(size('mjvGLCamera')), out=alloc(size('mjvGLCamera'));
  assert.equal(c1 % m._mjwf_alignof_mjvGLCamera(), 0);
  for (const [p,pos] of [[c1,[1,2,3]],[c2,[3,4,5]]]) {
    new Float32Array(m.HEAPU8.buffer,p+offset('mjvGLCamera','pos'),3).set(pos);
    new Float32Array(m.HEAPU8.buffer,p+offset('mjvGLCamera','forward'),3).set([0,0,-1]);
    new Float32Array(m.HEAPU8.buffer,p+offset('mjvGLCamera','up'),3).set([0,1,0]);
  }
  m._mjwf_mjv_averageCamera_out(out,c1,c2);
  assert.deepEqual([...new Float32Array(m.HEAPU8.buffer,out+offset('mjvGLCamera','pos'),3)],[2,3,4]);
  assert.deepEqual([...new Float32Array(m.HEAPU8.buffer,out+offset('mjvGLCamera','forward'),3)],[0,0,-1]);
  let logConfig = 'NOT_APPLICABLE';
  if (capabilities.functions.mju_getLogConfig) {
    const type='mjLogConfig', p=alloc(size(type)), q=alloc(size(type));
    m._mjwf_mju_getLogConfig_out(p);
    const saved=m.HEAPU8.slice(p,p+size(type));
    try {
      m.HEAPU8[p+offset(type,'logto_console')]=0;
      m.HEAPU8[p+offset(type,'logto_file')]=0;
      m._mjwf_mju_setLogConfig_ptr(p);
      m._mjwf_mju_getLogConfig_out(q);
      assert.equal(m.HEAPU8[q+offset(type,'logto_console')],0);
      assert.equal(m.HEAPU8[q+offset(type,'logto_file')],0);
      assert.equal(m.UTF8ToString(q+offset(type,'logfile')),m.UTF8ToString(p+offset(type,'logfile')));
      logConfig='PASS';
    } finally {m.HEAPU8.set(saved,p); m._mjwf_mju_setLogConfig_ptr(p);}
    assert.equal(capabilities.functions.mju_setLogHandler.rawCall,'needs-callback-bridge');
  }
  let u32Result='NOT_APPLICABLE';
  if (capabilities.functions.mj_getCacheCapacity) {
    for (const name of ['mj_getCacheCapacity','mj_getCacheSize','mj_setCacheCapacity'])
      assert.equal(capabilities.functions[name].returnType.resultNormalization,'rawResult >>> 0');
    const cache=m._mjwf_mj_getCache(), saved=m._mjwf_mj_getCacheCapacity(cache)>>>0;
    const capacity=0x80000017, used=m._mjwf_mj_getCacheSize(cache)>>>0;
    assert.ok(cache && used<=capacity);
    // SetCapacity changes a limit and trims existing entries; it does not reserve capacity bytes.
    try {
      const raw=m._mjwf_mj_setCacheCapacity(cache,capacity);
      assert.ok(raw<0); assert.equal(raw>>>0,capacity);
      assert.equal(m._mjwf_mj_getCacheCapacity(cache)>>>0,capacity);
      assert.equal(m._mjwf_mj_getCacheSize(cache)>>>0,used);
      u32Result='PASS';
    } finally {
      assert.equal(m._mjwf_mj_setCacheCapacity(cache,saved)>>>0,saved);
      assert.equal(m._mjwf_mj_getCacheCapacity(cache)>>>0,saved);
    }
  }
  let i64Parameter='NOT_APPLICABLE';
  if (capabilities.functions.mju_writeResource) {
    assert.equal(capabilities.functions.mju_writeResource.parameters.find(p=>p.name==='nbytes').kind,'i64');
    const content=alloc(3), error=alloc(256), name=bytes('/aggregate-resource.bin');
    m.HEAPU8.set([1,2,3],content);
    assert.equal(m._mjwf_mju_writeResource(name,content,3n,0,error,256),3n,m.UTF8ToString(error));
    assert.deepEqual([...m.FS.readFile('/aggregate-resource.bin')],[1,2,3]);
    i64Parameter='PASS';
  }
  const xml='<mujoco><worldbody><geom type="plane" size="1 1 .1"/><body pos="-.3 0 .08"><freejoint/><geom type="sphere" size=".1"/></body><body pos=".3 0 .08"><freejoint/><geom type="sphere" size=".1"/></body></worldbody></mujoco>';
  m.FS.writeFile('/two-contacts.xml',xml);
  handle=m.cwrap('mjwf_helper_make_from_xml','number',['string'])('/two-contacts.xml');
  assert.ok(handle>0);
  m._mjwf_mj_forward(m._mjwf_helper_model_ptr(handle),m._mjwf_helper_data_ptr(handle));
  assert.equal(m._mjwf_data_ncon(handle),2);
  const stride=m._mjwf_data_contact_stride();
  const position=m._mjwf_data_contact_pos_ptr(handle), distance=m._mjwf_data_contact_dist_ptr(handle);
  assert.ok(stride>3*8);
  const xs=[0,1].map(i=>view.getFloat64(position+i*stride,true)).sort((a,b)=>a-b);
  assert.ok(Math.abs(xs[0]+.3)<1e-12 && Math.abs(xs[1]-.3)<1e-12);
  for(let i=0;i<2;i++)assert.ok(Math.abs(view.getFloat64(distance+i*stride,true)+.02)<1e-12);
  console.log(JSON.stringify({aggregateABI:distVersion(),camera:'PASS',logConfig,i64Parameter,u32Result,contactAoS:'PASS',contactRecordStride:stride}));
} finally {
  if(handle)m._mjwf_helper_free(handle);
  for(const p of allocated)m._free(p);
}
if(process.env.MJWF_DIST_VARIANT==='pthreads')process.exit(0);
