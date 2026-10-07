function assert(condition, message) {
  if (!condition) throw new Error(message);
}

function timeout(ms) {
  return new Promise((_, reject) => {
    setTimeout(() => reject(new Error(`worker case timeout after ${ms} ms`)), ms);
  });
}

function compilerPoolXml() {
  const tetra = [
    'v 0 0 0',
    'v 1 0 0',
    'v 0 1 0',
    'v 0 0 1',
    'f 1 3 2',
    'f 1 2 4',
    'f 1 4 3',
    'f 2 3 4',
    '',
  ].join('\n');
  let assets = '';
  let bodies = '';
  for (let i = 0; i < 8; ++i) {
    const meshPath = `/mesh-${i}.obj`;
    assets += `<mesh name="m${i}" file="${meshPath}"/>`;
    bodies += `<body pos="${i * 3} 0 0"><freejoint/><geom type="mesh" mesh="m${i}"/></body>`;
  }
  return {
    tetra,
    xml: `<mujoco><asset>${assets}</asset><worldbody>${bodies}</worldbody></mujoco>`,
  };
}

async function runCase({ variant, artifactUrl, expectedVersion }) {
  assert(expectedVersion, 'missing expected MuJoCo version query parameter');
  const artifactDir = new URL('./', artifactUrl).href;
  const { default: factory } = await import(artifactUrl);
  const module = await factory({
    locateFile(name) {
      return name.endsWith('.wasm') ? new URL('mujoco.wasm', artifactDir).href : new URL(name, artifactDir).href;
    },
  });

  const version = module.cwrap('mjwf_mj_versionString', 'string', [])();
  assert(version === expectedVersion, `engine version ${version} != ${expectedVersion}`);
  const hasSharedArrayBuffer = typeof SharedArrayBuffer !== 'undefined';
  const sharedMemory = hasSharedArrayBuffer && module.HEAPU8.buffer instanceof SharedArrayBuffer;
  assert(sharedMemory === (variant === 'pthreads'), `SharedArrayBuffer=${sharedMemory} for ${variant}`);

  const xmlPath = variant === 'pthreads' ? '/compiler-pool.xml' : '/browser-smoke.xml';
  if (variant === 'pthreads') {
    const { tetra, xml } = compilerPoolXml();
    for (let i = 0; i < 8; ++i) module.FS.writeFile(`/mesh-${i}.obj`, tetra);
    module.FS.writeFile(xmlPath, xml);
  } else {
    module.FS.writeFile(xmlPath, '<mujoco><worldbody><body><joint/><geom size=".1"/></body></worldbody></mujoco>');
  }

  const makeFromXml = module.cwrap('mjwf_helper_make_from_xml', 'number', ['string']);
  const handle = makeFromXml(xmlPath);
  const lastError = module.cwrap('mjwf_helper_errmsg_last_global', 'string', [])();
  assert(handle > 0, `mjwf_helper_make_from_xml handle=${handle}: ${lastError}`);

  let threadpoolStarted = false;
  let threadpoolPtr = 0;
  let dataPtr = 0;
  try {
    const nq = module._mjwf_model_nq(handle);
    const nv = module._mjwf_model_nv(handle);
    const nu = module._mjwf_model_nu(handle);
    const nbody = module._mjwf_model_nbody(handle);
    const ngeom = module._mjwf_model_ngeom(handle);
    const nqI64 = module._mjwf_model_nq_i64(handle);
    const nbuffer = module._mjwf_model_nbuffer(handle);
    const nbufferI64 = module._mjwf_model_nbuffer_i64(handle);
    assert(typeof nq === 'number' && nq > 0, `invalid Number nq=${nq}`);
    assert(typeof nv === 'number' && nv > 0, `invalid Number nv=${nv}`);
    assert(typeof nu === 'number' && nu >= 0, `invalid Number nu=${nu}`);
    assert(typeof nbody === 'number' && nbody > 0, `invalid Number nbody=${nbody}`);
    assert(typeof ngeom === 'number' && ngeom > 0, `invalid Number ngeom=${ngeom}`);
    assert(typeof nqI64 === 'bigint' && nqI64 === BigInt(nq), `nq Number/BigInt mismatch ${nq}/${nqI64}`);
    assert(typeof nbuffer === 'number' && nbuffer > 0, `invalid Number nbuffer=${nbuffer}`);
    assert(typeof nbufferI64 === 'bigint' && nbufferI64 === BigInt(nbuffer), `nbuffer Number/BigInt mismatch`);

    const flagPtr = module._mjwf_data_flg_energypos_ptr(handle);
    const neighborPtr = module._mjwf_data_flg_energyvel_ptr(handle);
    assert(flagPtr > 0 && neighborPtr > flagPtr, `bool field pointers ${flagPtr}/${neighborPtr}`);
    const neighbor = module.HEAPU8[neighborPtr];
    module.HEAPU8[flagPtr] = 0;

    const modelPtr = module._mjwf_helper_model_ptr(handle);
    dataPtr = module._mjwf_helper_data_ptr(handle);
    module._mjwf_mj_energyPos(modelPtr, dataPtr);
    assert(module.HEAPU8[flagPtr] === 1, 'mjData.flg_energypos accessor did not update');
    assert(module.HEAPU8[neighborPtr] === neighbor, 'adjacent mjData bool field changed');
    const energyPosValue = module.HEAPU8[flagPtr];

    let meshCount = null;
    if (variant === 'pthreads') {
      meshCount = module._mjwf_model_nmesh(handle);
      assert(meshCount === 8, `compiler-pool nmesh=${meshCount}`);
      module._mjwf_mju_threadpool(dataPtr, 2);
      threadpoolStarted = true;
      threadpoolPtr = module._mjwf_data_threadpool_ptr(handle);
      assert(threadpoolPtr > 0 && module.HEAPU32[threadpoolPtr >>> 2] > 0, 'mju_threadpool did not start');
    }

    module._mjwf_mj_step(modelPtr, dataPtr);
    const qposPtr = module._mjwf_data_qpos_ptr(handle);
    const qpos = module.HEAPF64[qposPtr >>> 3];
    assert(Number.isFinite(qpos), `non-finite qpos=${qpos}`);

    if (threadpoolStarted) {
      module._mjwf_mju_threadpool(dataPtr, 0);
      assert(module.HEAPU32[threadpoolPtr >>> 2] === 0, 'mju_threadpool teardown did not clear pointer');
      threadpoolStarted = false;
    }

    return {
      status: 'PASS',
      variant,
      version,
      sharedMemory,
      crossOriginIsolated: self.crossOriginIsolated === true,
      hardwareConcurrency: typeof navigator === 'object' ? navigator.hardwareConcurrency : null,
      dims: { nq, nv, nu, nbody, ngeom, nqI64: String(nqI64), nbuffer, nbufferI64: String(nbufferI64) },
      boolField: { flagPtr, neighborPtr, energyPosValue, neighbor },
      meshCount,
      engineThreads: variant === 'pthreads' ? 2 : 0,
      threadpoolTeardown: variant === 'pthreads',
      qpos,
    };
  } finally {
    if (threadpoolStarted) {
      module._mjwf_mju_threadpool(dataPtr, 0);
    }
    module._mjwf_helper_free(handle);
  }
}

self.addEventListener('message', async event => {
  const input = event.data;
  try {
    const result = await Promise.race([
      runCase(input),
      timeout(input.timeoutMs),
    ]);
    self.postMessage(result);
  } catch (error) {
    self.postMessage({
      status: 'FAIL',
      variant: input.variant,
      error: String(error?.stack || error),
      workerErrors: [{ message: String(error?.message || error), stack: error?.stack || null }],
    });
  }
});
