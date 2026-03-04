import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { performance } from 'node:perf_hooks';
import { pathToFileURL } from 'node:url';

function parseArgs(argv) {
  const args = { dist: '', label: '', playRoot: '' };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    if (a === '--dist') args.dist = argv[++i] ?? '';
    else if (a === '--label') args.label = argv[++i] ?? '';
    else if (a === '--play-root') args.playRoot = argv[++i] ?? '';
    else throw new Error(`Unknown arg: ${a}`);
  }
  if (!args.dist) throw new Error('--dist is required');
  if (!args.label) args.label = path.basename(path.resolve(args.dist));
  return args;
}

function ensureNavigator() {
  const hc = os.cpus().length;
  const orig = globalThis.navigator;
  if (!orig || typeof orig !== 'object') {
    Object.defineProperty(globalThis, 'navigator', {
      value: { hardwareConcurrency: hc },
      configurable: true,
      enumerable: true,
      writable: true,
    });
    return;
  }
  if (typeof orig.hardwareConcurrency === 'number') return;
  const nav = Object.create(orig);
  Object.defineProperty(nav, 'hardwareConcurrency', {
    value: hc,
    configurable: true,
    enumerable: true,
    writable: true,
  });
  Object.defineProperty(globalThis, 'navigator', {
    value: nav,
    configurable: true,
    enumerable: true,
    writable: true,
  });
}

function statIfExists(p) {
  try {
    return fs.statSync(p);
  } catch {
    return null;
  }
}

function collectDistSizes(distRoot) {
  const sizes = {};
  for (const name of fs.readdirSync(distRoot)) {
    if (!name.match(/\.(js|mjs|wasm|map|d\.ts)$/)) continue;
    const st = statIfExists(path.join(distRoot, name));
    if (st?.isFile()) sizes[name] = st.size;
  }
  return sizes;
}

function rssBytes() {
  return process.memoryUsage().rss;
}

function startRssSampler({ intervalMs = 20 } = {}) {
  let peak = rssBytes();
  const timer = setInterval(() => {
    const r = rssBytes();
    if (r > peak) peak = r;
  }, intervalMs);
  return {
    stop: () => {
      clearInterval(timer);
      const r = rssBytes();
      if (r > peak) peak = r;
      return { rssPeakBytes: peak };
    },
  };
}

function benchFnCalls(fn, calls) {
  let acc = 0;
  const t0 = performance.now();
  for (let i = 0; i < calls; i += 1) {
    // eslint-disable-next-line no-bitwise
    acc = (acc + (fn() | 0)) | 0;
  }
  const dt = performance.now() - t0;
  const nsPerCall = (dt / calls) * 1e6;
  return { calls, totalMs: dt, nsPerCall, acc };
}

function ensureFsDir(Module, fsDir) {
  if (Module.FS.analyzePath(fsDir).exists) return;
  if (typeof Module.FS.mkdirTree === 'function') {
    Module.FS.mkdirTree(fsDir);
    return;
  }
  const parts = fsDir.split('/').filter(Boolean);
  let cur = '';
  for (const part of parts) {
    cur += `/${part}`;
    if (!Module.FS.analyzePath(cur).exists) Module.FS.mkdir(cur);
  }
}

function mountNodefsIfAvailable(Module, mountPoint, hostDir) {
  const fsApi = Module.FS;
  const nodefs = fsApi?.filesystems?.NODEFS;
  if (!fsApi || !nodefs) return { mounted: false, reason: 'NODEFS unavailable' };

  const t0 = performance.now();
  ensureFsDir(Module, mountPoint);
  if (fsApi.analyzePath(mountPoint).object?.mounted) {
    return { mounted: true, mountMs: performance.now() - t0 };
  }
  fsApi.mount(nodefs, { root: hostDir }, mountPoint);
  return { mounted: true, mountMs: performance.now() - t0 };
}

function writeTextFileToFS(Module, fsPath, text) {
  const dir = path.posix.dirname(fsPath);
  ensureFsDir(Module, dir);
  Module.FS.writeFile(fsPath, text, { canRead: true, canWrite: true });
}

function writeBinaryFileToFS(Module, fsPath, bytes) {
  const dir = path.posix.dirname(fsPath);
  ensureFsDir(Module, dir);
  Module.FS.writeFile(fsPath, bytes, { canRead: true, canWrite: true });
}

function stageHostFileToFS(Module, hostPath, fsPath) {
  const t0 = performance.now();
  const bytes = fs.readFileSync(hostPath);
  writeBinaryFileToFS(Module, fsPath, bytes);
  return performance.now() - t0;
}

function stageHostDirToFS(Module, hostDir, fsDir) {
  const t0 = performance.now();
  const stack = [hostDir];
  while (stack.length) {
    const cur = stack.pop();
    const rel = path.relative(hostDir, cur);
    const fsCur = rel ? path.posix.join(fsDir, rel.replaceAll('\\', '/')) : fsDir;
    ensureFsDir(Module, fsCur);
    const entries = fs.readdirSync(cur, { withFileTypes: true });
    for (const e of entries) {
      const hp = path.join(cur, e.name);
      if (e.isDirectory()) stack.push(hp);
      else if (e.isFile()) {
        const fp = path.posix.join(fsCur, e.name);
        const bytes = fs.readFileSync(hp);
        writeBinaryFileToFS(Module, fp, bytes);
      }
    }
  }
  return performance.now() - t0;
}

function benchSteps(stepFn, modelPtr, dataPtr, count) {
  for (let i = 0; i < Math.min(10, count); i += 1) stepFn(modelPtr, dataPtr);
  const t0 = performance.now();
  for (let i = 0; i < count; i += 1) stepFn(modelPtr, dataPtr);
  const dt = performance.now() - t0;
  return { stepCount: count, stepMs: dt, stepsPerSec: (count / dt) * 1000.0 };
}

function nowMs() {
  return performance.now();
}

async function loadModule(distRoot) {
  ensureNavigator();
  const jsPath = path.join(distRoot, 'mujoco.js');
  const wasmPath = path.join(distRoot, 'mujoco.wasm');
  const modFactory = (await import(pathToFileURL(jsPath).href)).default;
  const t0 = nowMs();
  const Module = await modFactory({
    locateFile: (p) => {
      if (p.endsWith('.wasm')) return wasmPath;
      const cand = path.join(distRoot, p);
      return fs.existsSync(cand) ? cand : p;
    },
  });
  if (Module.ready) await Module.ready;
  const initMs = nowMs() - t0;
  return { Module, initMs };
}

function makeFromXmlFns(Module) {
  const makeFromXml = Module.cwrap('mjwf_helper_make_from_xml', 'number', ['string']);
  const freeHandle = Module.cwrap('mjwf_helper_free', null, ['number']);
  const errnoLast = Module.cwrap('mjwf_helper_errno_last_global', 'number', []);
  const errmsgLast = Module.cwrap('mjwf_helper_errmsg_last_global', 'string', []);
  const modelPtrOf = Module.cwrap('mjwf_helper_model_ptr', 'number', ['number']);
  const dataPtrOf = Module.cwrap('mjwf_helper_data_ptr', 'number', ['number']);
  const step = Module.cwrap('mjwf_mj_step', null, ['number', 'number']);
  return { makeFromXml, freeHandle, errnoLast, errmsgLast, modelPtrOf, dataPtrOf, step };
}

function discoverPlayRoot(explicit) {
  if (explicit) return path.resolve(explicit);
  const sibling = path.resolve(process.cwd(), '..', 'mujoco-wasm-play');
  return fs.existsSync(path.join(sibling, 'model')) ? sibling : '';
}

function makeFsStager(Module) {
  const staged = new Set();
  return {
    stageFileOnce: (id, hostPath, fsPath) => {
      if (staged.has(id)) return 0;
      const ms = stageHostFileToFS(Module, hostPath, fsPath);
      staged.add(id);
      return ms;
    },
    stageDirOnce: (id, hostDir, fsDir) => {
      if (staged.has(id)) return 0;
      const ms = stageHostDirToFS(Module, hostDir, fsDir);
      staged.add(id);
      return ms;
    },
  };
}

function benchOneHandleModel(fns, { fsMountMs = null, fsCopyMs = null, pathInFs, stepCount }) {
  const t0 = nowMs();
  const h = fns.makeFromXml(pathInFs) | 0;
  const compileMs = nowMs() - t0;
  const errno = fns.errnoLast() | 0;
  const errmsg = fns.errmsgLast() ?? '';
  if (h <= 0) {
    return { status: 'error', fsMountMs, fsCopyMs, compileMs, errno, errmsg };
  }
  const mp = fns.modelPtrOf(h);
  const dp = fns.dataPtrOf(h);
  const tFirst0 = nowMs();
  fns.step(mp, dp);
  const firstStepMs = nowMs() - tFirst0;
  const stepped = benchSteps(fns.step, mp, dp, stepCount);
  fns.freeHandle(h);
  return {
    status: 'ok',
    fsMountMs,
    fsCopyMs,
    compileMs,
    firstStepMs,
    ttfsMs: compileMs + firstStepMs + (fsCopyMs ?? 0),
    errno,
    errmsg,
    ...stepped,
  };
}

async function main() {
  const { dist, label, playRoot: playRootRaw } = parseArgs(process.argv.slice(2));
  const distRoot = path.resolve(dist);
  const distSizes = collectDistSizes(distRoot);

  const rssSampler = startRssSampler();
  let results = null;
  try {
    const rssBefore = rssBytes();
    const { Module, initMs } = await loadModule(distRoot);
    const rssAfterInit = rssBytes();

    const wasmMemBytes = Module.HEAPU8?.buffer?.byteLength ?? 0;
    const fns = makeFromXmlFns(Module);
    const stager = makeFsStager(Module);

    results = {
      schemaVersion: 1,
      kind: 'node',
      target: 'forge',
      label,
      distRoot,
      distSizes,
      env: {
        timestamp: new Date().toISOString(),
        node: process.version,
        platform: process.platform,
        arch: process.arch,
        cpus: os.cpus().length,
      },
      metrics: {
        initMs,
        wasmMemBytes,
        rssBefore,
        rssAfterInit,
        rssPeakBytes: null,
        rssAfterBench: null,
        ffi: {},
        reload: {},
      },
      models: {},
    };

    const mjVersion = Module.cwrap('mjwf_mj_version', 'number', []);
    results.metrics.ffi.mj_version = benchFnCalls(mjVersion, 1_000_000);

    const pendulumXml = `<?xml version="1.0"?>
<mujoco model="pendulum">
  <option timestep="0.002" gravity="0 0 -9.81"/>
  <worldbody>
    <body name="link" pos="0 0 0.1">
      <joint name="hinge" type="hinge" axis="0 1 0" damping="0.01"/>
      <geom type="capsule" fromto="0 0 0 0 0 0.2" size="0.02" density="1000"/>
    </body>
  </worldbody>
</mujoco>`;
    const pendulumPath = '/bench/pendulum.xml';
    const tPendFs0 = nowMs();
    writeTextFileToFS(Module, pendulumPath, pendulumXml);
    const pendulumFsMs = nowMs() - tPendFs0;
    results.models.pendulum = benchOneHandleModel(fns, {
      fsCopyMs: pendulumFsMs,
      pathInFs: pendulumPath,
      stepCount: 20_000,
    });

    {
      const helperValid = Module.cwrap('mjwf_helper_valid', 'number', ['number']);
      const modelNq = Module.cwrap('mjwf_model_nq', 'number', ['number']);
      const h = fns.makeFromXml(pendulumPath) | 0;
      if (h <= 0) {
        throw new Error(`ffi(pendulum): make_from_xml returned ${h}; errno=${fns.errnoLast()} msg=${String(fns.errmsgLast() || '')}`);
      }
      results.metrics.ffi.helper_valid = benchFnCalls(() => helperValid(h), 1_000_000);
      results.metrics.ffi.helper_model_ptr = benchFnCalls(() => fns.modelPtrOf(h), 1_000_000);
      results.metrics.ffi.model_nq = benchFnCalls(() => modelNq(h), 1_000_000);
      fns.freeHandle(h);
    }

    const touchXml = `<?xml version="1.0"?>
<mujoco model="touch_grid_smoke">
  <compiler autolimits="true"/>
  <extension>
    <plugin plugin="mujoco.sensor.touch_grid"/>
  </extension>
  <worldbody>
    <body name="b" pos="0 0 0">
      <geom type="sphere" size="0.1"/>
      <site name="touch" pos="0 0 0"/>
    </body>
  </worldbody>
  <sensor>
    <plugin name="touch" plugin="mujoco.sensor.touch_grid" objtype="site" objname="touch">
      <config key="size" value="2 2"/>
      <config key="fov" value="45 45"/>
      <config key="gamma" value="0"/>
      <config key="nchannel" value="1"/>
    </plugin>
  </sensor>
</mujoco>`;
    const touchPath = '/bench/touch_grid.xml';
    const tTouchFs0 = nowMs();
    writeTextFileToFS(Module, touchPath, touchXml);
    const touchFsMs = nowMs() - tTouchFs0;
    results.models.touch_grid = benchOneHandleModel(fns, {
      fsCopyMs: touchFsMs,
      pathInFs: touchPath,
      stepCount: 1,
    });

    const hostMjModelDir = path.resolve('external/mujoco/model');
    const hostBunny = path.join(hostMjModelDir, 'flex', 'bunny.xml');
    if (fs.existsSync(hostBunny)) {
      const mount = mountNodefsIfAvailable(Module, '/host_mj_model', hostMjModelDir);
      let bunnyPath = '/host_mj_model/flex/bunny.xml';
      let bunnyFsCopyMs = null;
      if (!mount.mounted) {
        const hostFlexDir = path.join(hostMjModelDir, 'flex');
        const fsFlexDir = '/bench/mj_model/flex';
        const tStage0 = nowMs();
        stageHostFileToFS(Module, path.join(hostFlexDir, 'bunny.xml'), `${fsFlexDir}/bunny.xml`);
        stageHostFileToFS(Module, path.join(hostFlexDir, 'scene.xml'), `${fsFlexDir}/scene.xml`);
        stageHostFileToFS(Module, path.join(hostFlexDir, 'asset', 'bunny.obj'), `${fsFlexDir}/asset/bunny.obj`);
        bunnyFsCopyMs = nowMs() - tStage0;
        bunnyPath = `${fsFlexDir}/bunny.xml`;
      }
      results.models.flex_bunny = benchOneHandleModel(fns, {
        fsMountMs: mount.mounted ? mount.mountMs : null,
        fsCopyMs: bunnyFsCopyMs,
        pathInFs: bunnyPath,
        stepCount: 2_000,
      });
    } else {
      results.models.flex_bunny = { status: 'skipped', reason: `Missing host model: ${hostBunny}` };
    }

    const playRoot = discoverPlayRoot(playRootRaw);
    const playModelDir = playRoot ? path.join(playRoot, 'model') : '';
    if (playModelDir && fs.existsSync(playModelDir)) {
      const mount = mountNodefsIfAvailable(Module, '/host_play_model', playModelDir);
      const baseFsDir = '/bench/play_model';
      const models = [
        {
          id: 'raj',
          host: path.join(playModelDir, 'mujoco_Rajagopal2015_simple.xml'),
          fs: mount.mounted ? '/host_play_model/mujoco_Rajagopal2015_simple.xml' : `${baseFsDir}/mujoco_Rajagopal2015_simple.xml`,
          stage: () => (mount.mounted ? 0 : stager.stageFileOnce('play_raj', path.join(playModelDir, 'mujoco_Rajagopal2015_simple.xml'), `${baseFsDir}/mujoco_Rajagopal2015_simple.xml`)),
          stepCount: 2_000,
        },
        {
          id: 'cards',
          host: path.join(playModelDir, 'cards', 'cards.xml'),
          fs: mount.mounted ? '/host_play_model/cards/cards.xml' : `${baseFsDir}/cards/cards.xml`,
          stage: () => (mount.mounted ? 0 : stager.stageDirOnce('play_cards', path.join(playModelDir, 'cards'), `${baseFsDir}/cards`)),
          stepCount: 2_000,
        },
        {
          id: 'humanoid',
          host: path.join(playModelDir, 'humanoid', 'humanoid.xml'),
          fs: mount.mounted ? '/host_play_model/humanoid/humanoid.xml' : `${baseFsDir}/humanoid/humanoid.xml`,
          stage: () => (mount.mounted ? 0 : stager.stageDirOnce('play_humanoid', path.join(playModelDir, 'humanoid'), `${baseFsDir}/humanoid`)),
          stepCount: 2_000,
        },
        {
          id: 'sensor',
          host: path.join(playModelDir, 'plugin', 'sensor', 'touch_grid.xml'),
          fs: mount.mounted ? '/host_play_model/plugin/sensor/touch_grid.xml' : `${baseFsDir}/plugin/sensor/touch_grid.xml`,
          stage: () => (mount.mounted ? 0 : stager.stageDirOnce('play_sensor', path.join(playModelDir, 'plugin', 'sensor'), `${baseFsDir}/plugin/sensor`)),
          stepCount: 500,
        },
      ];

      for (const m of models) {
        if (!fs.existsSync(m.host)) {
          results.models[m.id] = { status: 'skipped', reason: `Missing host file: ${m.host}` };
          continue;
        }
        const fsCopyMs = m.stage();
        results.models[m.id] = benchOneHandleModel(fns, {
          fsMountMs: mount.mounted ? mount.mountMs : null,
          fsCopyMs,
          pathInFs: m.fs,
          stepCount: m.stepCount,
        });
      }
    } else {
      results.models.play_models = { status: 'skipped', reason: 'mujoco-wasm-play not found; pass --play-root' };
    }

    {
      const iters = 50;
      const rss0 = rssBytes();
      const sampler = startRssSampler({ intervalMs: 10 });
      const t0 = nowMs();
      let ok = 0;
      for (let i = 0; i < iters; i += 1) {
        const h = fns.makeFromXml(pendulumPath) | 0;
        if (h <= 0) {
          throw new Error(`reload(pendulum): make_from_xml returned ${h}; errno=${fns.errnoLast()} msg=${String(fns.errmsgLast() || '')}`);
        }
        const mp = fns.modelPtrOf(h);
        const dp = fns.dataPtrOf(h);
        fns.step(mp, dp);
        fns.freeHandle(h);
        ok += 1;
      }
      const totalMs = nowMs() - t0;
      const { rssPeakBytes } = sampler.stop();
      const rss1 = rssBytes();
      results.metrics.reload.pendulum = {
        iterations: iters,
        ok,
        totalMs,
        msPerIter: totalMs / iters,
        rssBefore: rss0,
        rssAfter: rss1,
        rssPeakBytes,
      };
    }

    results.metrics.rssAfterBench = rssBytes();
  } finally {
    const { rssPeakBytes } = rssSampler.stop();
    if (results?.metrics) results.metrics.rssPeakBytes = rssPeakBytes;
  }

  process.stdout.write(`${JSON.stringify(results)}\n`);
  process.exit(0);
}

main().catch((err) => {
  // eslint-disable-next-line no-console
  console.error(err);
  process.exit(1);
});
