import crypto from 'node:crypto';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { performance } from 'node:perf_hooks';
import { pathToFileURL } from 'node:url';

const STEP_COUNT = 2_000;
const WARMUP_COUNT = 10;
const PT_POOL_SIZE = 4;

function parseArgs(argv) {
  const args = { kind: '', dist: '', label: '', playRoot: '', hc: null, runIndex: null };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    if (a === '--kind') args.kind = argv[++i] ?? '';
    else if (a === '--dist') args.dist = argv[++i] ?? '';
    else if (a === '--label') args.label = argv[++i] ?? '';
    else if (a === '--play-root') args.playRoot = argv[++i] ?? '';
    else if (a === '--hc') {
      const n = Number(argv[++i] ?? '');
      if (!Number.isInteger(n) || n < 1) throw new Error(`Invalid --hc: ${n}`);
      args.hc = n;
    } else if (a === '--run-index') {
      const n = Number(argv[++i] ?? '');
      if (!Number.isInteger(n) || n < 1) throw new Error(`Invalid --run-index: ${n}`);
      args.runIndex = n;
    } else {
      throw new Error(`Unknown arg: ${a}`);
    }
  }
  if (!['forge', 'official'].includes(args.kind)) throw new Error('--kind must be forge or official');
  if (!args.dist) throw new Error('--dist is required');
  if (!args.playRoot) throw new Error('--play-root is required');
  if (!args.label) args.label = path.basename(path.resolve(args.dist));
  return args;
}

function isPthreads(label) {
  return /pthread|(^|-)mt($|-)/i.test(label);
}

function ensureNavigator(hcOverride) {
  const hc = hcOverride ?? os.cpus().length;
  const original = globalThis.navigator;
  if (!original || typeof original !== 'object') {
    Object.defineProperty(globalThis, 'navigator', {
      value: { hardwareConcurrency: hc },
      configurable: true,
      enumerable: true,
      writable: true,
    });
    return;
  }
  if (hcOverride === null && typeof original.hardwareConcurrency === 'number') return;
  const nav = Object.create(original);
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

function nowMs() {
  return performance.now();
}

function rssBytes() {
  return process.memoryUsage().rss;
}

function startRssSampler(intervalMs = 20) {
  let peak = rssBytes();
  const timer = setInterval(() => {
    peak = Math.max(peak, rssBytes());
  }, intervalMs);
  return {
    stop() {
      clearInterval(timer);
      peak = Math.max(peak, rssBytes());
      return peak;
    },
  };
}

function sha256File(filePath) {
  return crypto.createHash('sha256').update(fs.readFileSync(filePath)).digest('hex');
}

function artifactInfo(distRoot) {
  const jsPath = path.join(distRoot, 'mujoco.js');
  const wasmPath = path.join(distRoot, 'mujoco.wasm');
  const jsStat = fs.statSync(jsPath);
  const wasmStat = fs.statSync(wasmPath);
  return {
    js: { path: jsPath, bytes: jsStat.size, sha256: sha256File(jsPath) },
    wasm: { path: wasmPath, bytes: wasmStat.size, sha256: sha256File(wasmPath) },
    runtimePairBytes: jsStat.size + wasmStat.size,
  };
}

function ensureFsDir(Module, fsDir) {
  if (Module.FS.analyzePath(fsDir).exists) return;
  if (typeof Module.FS.mkdirTree === 'function') {
    Module.FS.mkdirTree(fsDir);
    return;
  }
  let cur = '';
  for (const part of fsDir.split('/').filter(Boolean)) {
    cur += `/${part}`;
    if (!Module.FS.analyzePath(cur).exists) Module.FS.mkdir(cur);
  }
}

function writeFileToFS(Module, fsPath, bytes) {
  ensureFsDir(Module, path.posix.dirname(fsPath));
  Module.FS.writeFile(fsPath, bytes, { canRead: true, canWrite: true });
}

function stageHostDirToFS(Module, hostDir, fsDir) {
  const t0 = nowMs();
  const stack = [hostDir];
  let fileCount = 0;
  let byteCount = 0;
  while (stack.length) {
    const current = stack.pop();
    const rel = path.relative(hostDir, current);
    const currentFsDir = rel ? path.posix.join(fsDir, rel.replaceAll('\\', '/')) : fsDir;
    ensureFsDir(Module, currentFsDir);
    for (const entry of fs.readdirSync(current, { withFileTypes: true })) {
      const hostPath = path.join(current, entry.name);
      if (entry.isDirectory()) {
        stack.push(hostPath);
      } else if (entry.isFile()) {
        const bytes = fs.readFileSync(hostPath);
        writeFileToFS(Module, path.posix.join(currentFsDir, entry.name), bytes);
        fileCount += 1;
        byteCount += bytes.byteLength;
      }
    }
  }
  return { stageMs: nowMs() - t0, fileCount, byteCount };
}

function finiteArray(values) {
  if (!values || typeof values.length !== 'number') return false;
  for (let i = 0; i < values.length; i += 1) {
    if (!Number.isFinite(Number(values[i]))) return false;
  }
  return true;
}

function stepLoop(stepFn, model, data) {
  for (let i = 0; i < WARMUP_COUNT; i += 1) stepFn(model, data);
  const t0 = nowMs();
  for (let i = 0; i < STEP_COUNT; i += 1) stepFn(model, data);
  const stepMs = nowMs() - t0;
  return { warmupCount: WARMUP_COUNT, stepCount: STEP_COUNT, stepMs, msPerStep: stepMs / STEP_COUNT };
}

async function loadModule(kind, distRoot, hcOverride) {
  ensureNavigator(hcOverride);
  const modulePath = path.join(distRoot, 'mujoco.js');
  const wasmPath = path.join(distRoot, 'mujoco.wasm');
  const factory = (await import(pathToFileURL(modulePath).href)).default;
  const t0 = nowMs();
  const Module = await factory({
    locateFile: (fileName) => fileName.endsWith('.wasm') ? wasmPath : path.join(distRoot, fileName),
  });
  if (Module.ready) await Module.ready;
  return { Module, initMs: nowMs() - t0, modulePath, wasmPath };
}

function forgeApi(Module) {
  const cwrap = (name, returnType, argTypes = []) => Module.cwrap(name, returnType, argTypes);
  const makeFromXml = cwrap('mjwf_helper_make_from_xml', 'number', ['string']);
  return {
    version: cwrap('mjwf_mj_versionString', 'string', []),
    makeFromXml,
    free: cwrap('mjwf_helper_free', null, ['number']),
    modelPtr: cwrap('mjwf_helper_model_ptr', 'number', ['number']),
    dataPtr: cwrap('mjwf_helper_data_ptr', 'number', ['number']),
    step: cwrap('mjwf_mj_step', null, ['number', 'number']),
    dims: {
      nq: cwrap('mjwf_model_nq', 'number', ['number']),
      nv: cwrap('mjwf_model_nv', 'number', ['number']),
      nu: cwrap('mjwf_model_nu', 'number', ['number']),
      nbody: cwrap('mjwf_model_nbody', 'number', ['number']),
      ngeom: cwrap('mjwf_model_ngeom', 'number', ['number']),
    },
    qposPtr: cwrap('mjwf_data_qpos_ptr', 'number', ['number']),
  };
}

function officialApi(Module) {
  const ModelType = Module.MjModel;
  const DataType = Module.MjData;
  if (!ModelType || !DataType) throw new Error('3.15 official module lacks MjModel/MjData');
  const loadXml = typeof ModelType.mj_loadXML === 'function'
    ? (filePath) => ModelType.mj_loadXML(filePath)
    : (filePath) => ModelType.loadFromXML(filePath);
  return {
    version: () => Module.mj_versionString(),
    loadXml,
    makeData: (model) => new DataType(model),
    step: (model, data) => Module.mj_step(model, data),
  };
}

function getDims(kind, api, Module, modelOrHandle) {
  if (kind === 'forge') {
    return Object.fromEntries(Object.entries(api.dims).map(([key, fn]) => [key, Number(fn(modelOrHandle))]));
  }
  return {
    nq: Number(modelOrHandle.nq),
    nv: Number(modelOrHandle.nv),
    nu: Number(modelOrHandle.nu),
    nbody: Number(modelOrHandle.nbody),
    ngeom: Number(modelOrHandle.ngeom),
  };
}

function checkForgeQpos(Module, api, handle, nq) {
  const ptr = Number(api.qposPtr(handle));
  if (!ptr || !Module.HEAPF64) return { finite: false, length: 0 };
  return { finite: finiteArray(Module.HEAPF64.subarray(ptr / 8, ptr / 8 + nq)), length: nq };
}

function exportedMemoryInfo(Module) {
  const descriptor = Object.getOwnPropertyDescriptor(Module, 'HEAPU8');
  if (!descriptor || descriptor.get) {
    return { memoryShared: null, wasmMemoryBytes: null };
  }
  const heap = descriptor.value;
  return {
    memoryShared: heap?.buffer instanceof SharedArrayBuffer,
    wasmMemoryBytes: heap?.buffer?.byteLength ?? null,
  };
}

function checkOfficialQpos(data) {
  const qpos = data.qpos;
  return {
    finite: finiteArray(qpos),
    length: Number(qpos?.length ?? 0),
    memoryShared: qpos?.buffer instanceof SharedArrayBuffer,
  };
}

async function runForgeModel(Module, api, spec) {
  const loadT0 = nowMs();
  const handle = Number(api.makeFromXml(spec.fsPath));
  const loadMs = nowMs() - loadT0;
  if (handle <= 0) throw new Error(`make_from_xml failed for ${spec.id}: handle=${handle}`);
  let modelPtr = 0;
  let dataPtr = 0;
  try {
    modelPtr = Number(api.modelPtr(handle));
    dataPtr = Number(api.dataPtr(handle));
    const dims = getDims('forge', api, Module, handle);
    const firstT0 = nowMs();
    api.step(modelPtr, dataPtr);
    const firstStepMs = nowMs() - firstT0;
    const stepped = stepLoop(api.step, modelPtr, dataPtr);
    const qpos = checkForgeQpos(Module, api, handle, dims.nq);
    return {
      status: 'ok',
      loadDataBoundary: 'helper_make_from_xml -> model+data handle; then first mj_step',
      handlePositive: true,
      dims,
      loadMs,
      firstStepMs,
      ...stepped,
      qpos,
    };
  } finally {
    api.free(handle);
  }
}

async function runOfficialModel(api, spec) {
  const t0 = nowMs();
  const model = api.loadXml(spec.fsPath);
  const data = api.makeData(model);
  const loadMs = nowMs() - t0;
  try {
    const dims = getDims('official', api, null, model);
    const firstT0 = nowMs();
    api.step(model, data);
    const firstStepMs = nowMs() - firstT0;
    const stepped = stepLoop(api.step, model, data);
    const qpos = checkOfficialQpos(data);
    return {
      status: 'ok',
      loadDataBoundary: 'MjModel.mj_loadXML + new MjData; then first mj_step',
      handlePositive: true,
      dims,
      loadMs,
      firstStepMs,
      ...stepped,
      qpos,
    };
  } finally {
    data.delete?.();
    model.delete?.();
  }
}

async function safeModelRun(kind, Module, api, spec) {
  try {
    return kind === 'forge'
      ? await runForgeModel(Module, api, spec)
      : await runOfficialModel(api, spec);
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    const stack = error instanceof Error ? error.stack : undefined;
    process.stderr.write(`[bench_315_adapter] ${spec.id} failed: ${message}\n`);
    return { status: 'error', error: message, stack };
  }
}

function modelSpecs(playRoot) {
  const modelRoot = path.join(path.resolve(playRoot), 'model');
  return [
    { id: 'humanoid', hostDir: path.join(modelRoot, 'humanoid'), hostXml: path.join(modelRoot, 'humanoid', 'humanoid.xml'), fsPath: '/bench/play_model/humanoid/humanoid.xml' },
    { id: 'cards', hostDir: path.join(modelRoot, 'cards'), hostXml: path.join(modelRoot, 'cards', 'cards.xml'), fsPath: '/bench/play_model/cards/cards.xml' },
  ].map((spec) => ({ ...spec, xmlSha256: sha256File(spec.hostXml) }));
}

async function main() {
  const args = parseArgs(process.argv.slice(2));
  const distRoot = path.resolve(args.dist);
  const pthreads = isPthreads(args.label);
  const hcOverride = pthreads ? (args.hc ?? PT_POOL_SIZE) : args.hc;
  const rssSampler = startRssSampler();
  const rssBefore = rssBytes();
  const artifact = artifactInfo(distRoot);
  const specs = modelSpecs(args.playRoot);
  let result = {
    schemaVersion: 2,
    benchmark: 'mujoco-wasm-3.15-deployment-bundle',
    status: 'error',
    kind: args.kind,
    label: args.label,
    runIndex: args.runIndex,
    distRoot,
    artifact,
    models: {},
    metrics: {
      rssBefore,
      rssAfterInit: null,
      rssAfterBench: null,
      rssPeakBytes: null,
      initMs: null,
      memoryShared: null,
      navigatorHardwareConcurrency: null,
      workerPoolWarmTarget: pthreads ? PT_POOL_SIZE : 0,
      engineThreadpoolRequested: 0,
      engineThreadpoolStarted: false,
      stagingExcludedFromLoadMs: true,
    },
    env: {
      timestamp: new Date().toISOString(),
      node: process.version,
      platform: process.platform,
      arch: process.arch,
      hostCpuCount: os.cpus().length,
      requestedHardwareConcurrency: hcOverride,
    },
  };

  try {
    const loaded = await loadModule(args.kind, distRoot, hcOverride);
    const { Module } = loaded;
    const api = args.kind === 'forge' ? forgeApi(Module) : officialApi(Module);
    result.status = 'ok';
    result.version = String(api.version());
    result.metrics.initMs = loaded.initMs;
    result.metrics.rssAfterInit = rssBytes();
    const memoryInfo = exportedMemoryInfo(Module);
    result.metrics.memoryShared = memoryInfo.memoryShared;
    result.metrics.navigatorHardwareConcurrency = globalThis.navigator?.hardwareConcurrency ?? null;
    result.metrics.wasmMemoryBytes = memoryInfo.wasmMemoryBytes;

    for (const spec of specs) {
      const staged = stageHostDirToFS(Module, spec.hostDir, path.posix.dirname(spec.fsPath));
      const model = await safeModelRun(args.kind, Module, api, { ...spec, stage: staged });
      result.models[spec.id] = { ...model, staging: staged, xmlSha256: spec.xmlSha256 };
      if (result.metrics.memoryShared === null && model.qpos?.memoryShared !== undefined) {
        result.metrics.memoryShared = model.qpos.memoryShared;
      }
      if (model.status !== 'ok') result.status = 'error';
    }
    result.metrics.rssAfterBench = rssBytes();
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    result.error = message;
    result.errorStack = error instanceof Error ? error.stack : undefined;
    process.stderr.write(`[bench_315_adapter] process failed: ${message}\n`);
  } finally {
    result.metrics.rssPeakBytes = rssSampler.stop();
  }

  process.stdout.write(`${JSON.stringify(result)}\n`);
  if (result.status !== 'ok') process.exitCode = 1;
}

main().catch((error) => {
  process.stderr.write(`[bench_315_adapter] fatal: ${error instanceof Error ? error.stack : String(error)}\n`);
  process.exitCode = 1;
});
