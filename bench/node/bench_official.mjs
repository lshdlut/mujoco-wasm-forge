import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { performance } from 'node:perf_hooks';
import { pathToFileURL } from 'node:url';

function parseArgs(argv) {
  const args = { dist: '', label: '', hc: null, playRoot: '' };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    if (a === '--dist') args.dist = argv[++i] ?? '';
    else if (a === '--label') args.label = argv[++i] ?? '';
    else if (a === '--hc') {
      const raw = argv[++i] ?? '';
      const n = Number(raw);
      if (!Number.isFinite(n) || n < 0) throw new Error(`Invalid --hc: ${raw}`);
      args.hc = n;
    } else if (a === '--play-root') args.playRoot = argv[++i] ?? '';
    else throw new Error(`Unknown arg: ${a}`);
  }
  if (!args.dist) throw new Error('--dist is required');
  if (!args.label) args.label = path.basename(path.resolve(args.dist));
  return args;
}

function ensureNavigator({ hcOverride = null } = {}) {
  const hc = hcOverride ?? os.cpus().length;
  const orig = globalThis.navigator;
  const hasNumeric = typeof orig?.hardwareConcurrency === 'number';
  if (!orig || typeof orig !== 'object') {
    Object.defineProperty(globalThis, 'navigator', {
      value: { hardwareConcurrency: hc },
      configurable: true,
      enumerable: true,
      writable: true,
    });
    return;
  }
  if (hcOverride === null && hasNumeric) return;
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

function benchSteps(stepFn, model, data, count) {
  for (let i = 0; i < Math.min(10, count); i += 1) stepFn(model, data);
  const t0 = performance.now();
  for (let i = 0; i < count; i += 1) stepFn(model, data);
  const dt = performance.now() - t0;
  return { stepCount: count, stepMs: dt, stepsPerSec: (count / dt) * 1000.0 };
}

async function loadModule(distRoot, { hcOverride = null } = {}) {
  ensureNavigator({ hcOverride });
  const jsPath = path.join(distRoot, 'mujoco_wasm.js');
  const modFactory = (await import(pathToFileURL(jsPath).href)).default;
  const t0 = performance.now();
  const Module = await modFactory({
    locateFile: (p) => path.join(distRoot, p),
  });
  if (Module.ready) await Module.ready;
  const initMs = performance.now() - t0;
  return { Module, initMs };
}

function loadModelApi(Module) {
  const ModelType = Module.MjModel ?? null;
  const DataType = Module.MjData ?? null;
  if (!ModelType || !DataType) {
    throw new Error('Official module missing embind MjModel/MjData types');
  }

  const loadXml = (fsPath) => {
    if (typeof ModelType.loadFromXML === 'function') return ModelType.loadFromXML(fsPath);
    if (typeof ModelType.mj_loadXML === 'function') return ModelType.mj_loadXML(fsPath);
    throw new Error('Official MjModel has no loadFromXML/mj_loadXML');
  };

  const mkData = (model) => new DataType(model);
  const step = (model, data) => Module.mj_step(model, data);
  return { loadXml, mkData, step };
}

function discoverPlayRoot(explicit) {
  if (explicit) return path.resolve(explicit);
  const sibling = path.resolve(process.cwd(), '..', 'mujoco-wasm-play');
  return fs.existsSync(path.join(sibling, 'model')) ? sibling : '';
}

async function main() {
  const { dist, label, hc, playRoot: playRootRaw } = parseArgs(process.argv.slice(2));
  const distRoot = path.resolve(dist);
  const distSizes = collectDistSizes(distRoot);

  const rssBefore = rssBytes();
  const { Module, initMs } = await loadModule(distRoot, { hcOverride: hc });
  const rssAfterInit = rssBytes();

  const api = loadModelApi(Module);

  const results = {
    schemaVersion: 1,
    kind: 'node',
    target: 'official-embind',
    label,
    distRoot,
    distSizes,
    env: {
      timestamp: new Date().toISOString(),
      node: process.version,
      platform: process.platform,
      arch: process.arch,
      cpus: os.cpus().length,
      hardwareConcurrency: globalThis.navigator?.hardwareConcurrency ?? null,
      nodefsAvailable: null,
    },
    metrics: {
      initMs,
      wasmMemBytes: null,
      rssBefore,
      rssAfterInit,
      rssAfterBench: null,
    },
    models: {},
  };

  results.env.nodefsAvailable = Boolean(Module.FS?.filesystems?.NODEFS);

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

  try {
    const t0 = performance.now();
    const model = api.loadXml(pendulumPath);
    const loadMs = performance.now() - t0;
    const t1 = performance.now();
    const data = api.mkData(model);
    const dataMs = performance.now() - t1;
    results.models.pendulum = { status: 'ok', fsCopyMs: pendulumFsMs, loadMs, dataMs, ...benchSteps(api.step, model, data, 20_000) };
    data.delete?.();
    model.delete?.();
  } catch (err) {
    results.models.pendulum = { status: 'error', fsCopyMs: pendulumFsMs, error: err?.stack || String(err) };
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

  try {
    const t0 = performance.now();
    const model = api.loadXml(touchPath);
    const loadMs = performance.now() - t0;
    model.delete?.();
    results.models.touch_grid = { status: 'ok', fsCopyMs: touchFsMs, loadMs };
  } catch (err) {
    results.models.touch_grid = { status: 'error', fsCopyMs: touchFsMs, error: err?.stack || String(err) };
  }

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

    try {
      const t0 = performance.now();
      const model = api.loadXml(bunnyPath);
      const loadMs = performance.now() - t0;
      const t1 = performance.now();
      const data = api.mkData(model);
      const dataMs = performance.now() - t1;
      results.models.flex_bunny = {
        status: 'ok',
        fsMountMs: mount.mounted ? mount.mountMs : null,
        fsCopyMs: bunnyFsCopyMs,
        loadMs,
        dataMs,
        ...benchSteps(api.step, model, data, 2_000),
      };
      data.delete?.();
      model.delete?.();
    } catch (err) {
      results.models.flex_bunny = {
        status: 'error',
        fsMountMs: mount.mounted ? mount.mountMs : null,
        fsCopyMs: bunnyFsCopyMs,
        error: err?.stack || String(err),
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
      try {
        const t0 = performance.now();
        const model = api.loadXml(m.fsPath);
        const loadMs = performance.now() - t0;
        const t1 = performance.now();
        const data = api.mkData(model);
        const dataMs = performance.now() - t1;
        results.models[m.id] = {
          status: 'ok',
          fsMountMs: mount.mounted ? mount.mountMs : null,
          fsCopyMs: m.fsCopyMs ?? null,
          loadMs,
          dataMs,
          ...benchSteps(api.step, model, data, m.stepCount),
        };
        data.delete?.();
        model.delete?.();
      } catch (err) {
        results.models[m.id] = {
          status: 'error',
          fsMountMs: mount.mounted ? mount.mountMs : null,
          fsCopyMs: m.fsCopyMs ?? null,
          error: err?.stack || String(err),
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
