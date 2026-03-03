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

async function loadModule(distRoot) {
  ensureNavigator();
  const jsPath = path.join(distRoot, 'mujoco.js');
  const wasmPath = path.join(distRoot, 'mujoco.wasm');
  const modFactory = (await import(pathToFileURL(jsPath).href)).default;
  const t0 = performance.now();
  const Module = await modFactory({
    locateFile: (p) => {
      if (p.endsWith('.wasm')) return wasmPath;
      const cand = path.join(distRoot, p);
      return fs.existsSync(cand) ? cand : p;
    },
  });
  if (Module.ready) await Module.ready;
  const initMs = performance.now() - t0;
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

async function main() {
  const { dist, label, playRoot: playRootRaw } = parseArgs(process.argv.slice(2));
  const distRoot = path.resolve(dist);
  const distSizes = collectDistSizes(distRoot);

  const rssBefore = rssBytes();
  const { Module, initMs } = await loadModule(distRoot);
  const rssAfterInit = rssBytes();

  const wasmMemBytes = Module.HEAPU8?.buffer?.byteLength ?? 0;
  const fns = makeFromXmlFns(Module);

  const results = {
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
      rssAfterBench: null,
    },
    models: {},
  };

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
  const tPendFs0 = performance.now();
  writeTextFileToFS(Module, pendulumPath, pendulumXml);
  const pendulumFsMs = performance.now() - tPendFs0;

  const tPend0 = performance.now();
  const pendH = fns.makeFromXml(pendulumPath) | 0;
  const pendCompileMs = performance.now() - tPend0;
  const pendErrno = fns.errnoLast() | 0;
  const pendErrmsg = fns.errmsgLast() ?? '';
  if (pendH > 0) {
    const m = fns.modelPtrOf(pendH);
    const d = fns.dataPtrOf(pendH);
    results.models.pendulum = {
      status: 'ok',
      fsCopyMs: pendulumFsMs,
      compileMs: pendCompileMs,
      errno: pendErrno,
      errmsg: pendErrmsg,
      ...benchSteps(fns.step, m, d, 20_000),
    };
    fns.freeHandle(pendH);
  } else {
    results.models.pendulum = {
      status: 'error',
      fsCopyMs: pendulumFsMs,
      compileMs: pendCompileMs,
      errno: pendErrno,
      errmsg: pendErrmsg,
    };
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
  const tTouchFs0 = performance.now();
  writeTextFileToFS(Module, touchPath, touchXml);
  const touchFsMs = performance.now() - tTouchFs0;

  const tTouch0 = performance.now();
  const touchH = fns.makeFromXml(touchPath) | 0;
  const touchCompileMs = performance.now() - tTouch0;
  const touchErrno = fns.errnoLast() | 0;
  const touchErrmsg = fns.errmsgLast() ?? '';
  if (touchH > 0) fns.freeHandle(touchH);
  results.models.touch_grid = {
    status: touchH > 0 ? 'ok' : 'error',
    fsCopyMs: touchFsMs,
    compileMs: touchCompileMs,
    errno: touchErrno,
    errmsg: touchErrmsg,
  };

  const hostMjModelDir = path.resolve('external/mujoco/model');
  const hostBunny = path.join(hostMjModelDir, 'flex', 'bunny.xml');
  if (fs.existsSync(hostBunny)) {
    const mount = mountNodefsIfAvailable(Module, '/host_mj_model', hostMjModelDir);
    let bunnyPath = '/host_mj_model/flex/bunny.xml';
    let bunnyFsCopyMs = null;
    if (!mount.mounted) {
      const hostFlexDir = path.join(hostMjModelDir, 'flex');
      const fsFlexDir = '/bench/mj_model/flex';
      const tStage0 = performance.now();
      stageHostFileToFS(Module, path.join(hostFlexDir, 'bunny.xml'), `${fsFlexDir}/bunny.xml`);
      stageHostFileToFS(Module, path.join(hostFlexDir, 'scene.xml'), `${fsFlexDir}/scene.xml`);
      stageHostFileToFS(Module, path.join(hostFlexDir, 'asset', 'bunny.obj'), `${fsFlexDir}/asset/bunny.obj`);
      bunnyFsCopyMs = performance.now() - tStage0;
      bunnyPath = `${fsFlexDir}/bunny.xml`;
    }
    const tBunny0 = performance.now();
    const bunnyH = fns.makeFromXml(bunnyPath) | 0;
    const bunnyCompileMs = performance.now() - tBunny0;
    const bunnyErrno = fns.errnoLast() | 0;
    const bunnyErrmsg = fns.errmsgLast() ?? '';
    if (bunnyH > 0) {
      const m = fns.modelPtrOf(bunnyH);
      const d = fns.dataPtrOf(bunnyH);
      results.models.flex_bunny = {
        status: 'ok',
        fsMountMs: mount.mounted ? mount.mountMs : null,
        fsCopyMs: bunnyFsCopyMs,
        compileMs: bunnyCompileMs,
        errno: bunnyErrno,
        errmsg: bunnyErrmsg,
        ...benchSteps(fns.step, m, d, 2_000),
      };
      fns.freeHandle(bunnyH);
    } else {
      results.models.flex_bunny = {
        status: 'error',
        fsMountMs: mount.mounted ? mount.mountMs : null,
        fsCopyMs: bunnyFsCopyMs,
        compileMs: bunnyCompileMs,
        errno: bunnyErrno,
        errmsg: bunnyErrmsg,
      };
    }
  } else {
    results.models.flex_bunny = { status: 'skipped', reason: `Missing host model: ${hostBunny}` };
  }

  const playRoot = discoverPlayRoot(playRootRaw);
  const playModelDir = playRoot ? path.join(playRoot, 'model') : '';
  if (playModelDir && fs.existsSync(playModelDir)) {
    const mount = mountNodefsIfAvailable(Module, '/host_play_model', playModelDir);
    let stage = null;
    if (!mount.mounted) {
      stage = {};
      stage.rajMs = stageHostFileToFS(
        Module,
        path.join(playModelDir, 'mujoco_Rajagopal2015_simple.xml'),
        '/bench/play_model/mujoco_Rajagopal2015_simple.xml',
      );
      stage.cardsMs = stageHostDirToFS(Module, path.join(playModelDir, 'cards'), '/bench/play_model/cards');
      stage.humanoidMs = stageHostDirToFS(Module, path.join(playModelDir, 'humanoid'), '/bench/play_model/humanoid');
      stage.sensorMs = stageHostDirToFS(Module, path.join(playModelDir, 'plugin', 'sensor'), '/bench/play_model/plugin/sensor');
    }
    const playModels = [
      {
        id: 'raj',
        fsPath: mount.mounted ? '/host_play_model/mujoco_Rajagopal2015_simple.xml' : '/bench/play_model/mujoco_Rajagopal2015_simple.xml',
        stepCount: 2_000,
        fsCopyMs: stage?.rajMs ?? null,
      },
      {
        id: 'cards',
        fsPath: mount.mounted ? '/host_play_model/cards/cards.xml' : '/bench/play_model/cards/cards.xml',
        stepCount: 2_000,
        fsCopyMs: stage?.cardsMs ?? null,
      },
      {
        id: 'humanoid',
        fsPath: mount.mounted ? '/host_play_model/humanoid/humanoid.xml' : '/bench/play_model/humanoid/humanoid.xml',
        stepCount: 2_000,
        fsCopyMs: stage?.humanoidMs ?? null,
      },
      {
        id: 'sensor',
        fsPath: mount.mounted ? '/host_play_model/plugin/sensor/touch_grid.xml' : '/bench/play_model/plugin/sensor/touch_grid.xml',
        stepCount: 500,
        fsCopyMs: stage?.sensorMs ?? null,
      },
    ];
    for (const m of playModels) {
      const hostPath = path.join(playModelDir, m.fsPath.replace('/host_play_model/', ''));
      if (mount.mounted && !fs.existsSync(hostPath)) {
        results.models[m.id] = { status: 'skipped', reason: `Missing host file: ${hostPath}` };
        continue;
      }
      const t0 = performance.now();
      const h = fns.makeFromXml(m.fsPath) | 0;
      const compileMs = performance.now() - t0;
      const errno = fns.errnoLast() | 0;
      const errmsg = fns.errmsgLast() ?? '';
      if (h > 0) {
        const mp = fns.modelPtrOf(h);
        const dp = fns.dataPtrOf(h);
        results.models[m.id] = {
          status: 'ok',
          fsMountMs: mount.mounted ? mount.mountMs : null,
          fsCopyMs: m.fsCopyMs ?? null,
          compileMs,
          errno,
          errmsg,
          ...benchSteps(fns.step, mp, dp, m.stepCount),
        };
        fns.freeHandle(h);
      } else {
        results.models[m.id] = {
          status: 'error',
          fsMountMs: mount.mounted ? mount.mountMs : null,
          fsCopyMs: m.fsCopyMs ?? null,
          compileMs,
          errno,
          errmsg,
        };
      }
    }
  } else {
    results.models.play_models = { status: 'skipped', reason: 'mujoco-wasm-play not found; pass --play-root' };
  }

  results.metrics.rssAfterBench = rssBytes();
  process.stdout.write(`${JSON.stringify(results)}\n`);
  process.exit(0);
}

main().catch((err) => {
  // eslint-disable-next-line no-console
  console.error(err);
  process.exit(1);
});
