import path from "node:path";
import { pathToFileURL } from "node:url";
import { performance } from "node:perf_hooks";
import { access } from "node:fs/promises";

const ITERATIONS = Number(process.env.BENCH_ITERATIONS ?? 100_000);
const WARMUPS = Number(process.env.BENCH_WARMUPS ?? 1_000);

const distDir = path.resolve(process.cwd(), "dist/3.3.8-alpha");
const wasmPath = path.join(distDir, "mujoco.wasm");
const jsPath = path.join(distDir, "mujoco.js");

await ensureFile(jsPath);
await ensureFile(wasmPath);

const factory = (await import(pathToFileURL(jsPath).href)).default;
const Module = await factory({
  locateFile: (p) => (p.endsWith(".wasm") ? wasmPath : p),
});
if (Module.ready) await Module.ready;

const readBack = process.env.BENCH_READBACK === "1";
const preForward = process.env.BENCH_PREFORWARD === "1";
const batchMode = process.env.BENCH_BATCH === "1";

const stackTop = Module.stackSave();

const parseXMLString = Module.cwrap(
  "mjwf_mj_parseXMLString",
  "number",
  ["string", "number", "number", "number"],
);

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

const specPtr = parseXMLString(xml, 0, 0, 0);
if (!specPtr) throw new Error("mj_parseXMLString returned null");

const compile = ensureFn(bindExport("mjwf_mj_compile", "number", ["number", "number"]), "mjwf_mj_compile");
const deleteSpec = ensureFn(bindExport("mjwf_mj_deleteSpec", null, ["number"]), "mjwf_mj_deleteSpec");
const makeData = ensureFn(bindExport("mjwf_mj_makeData", "number", ["number"]), "mjwf_mj_makeData");
const deleteData = ensureFn(bindExport("mjwf_mj_deleteData", null, ["number"]), "mjwf_mj_deleteData");
const deleteModel = ensureFn(bindExport("mjwf_mj_deleteModel", null, ["number"]), "mjwf_mj_deleteModel");
const resetData = ensureFn(bindExport("mjwf_mj_resetData", null, ["number", "number"]), "mjwf_mj_resetData");
const getNq = Module.cwrap("mjwf_nq", "number", ["number"]);
const getQposPtr = Module.cwrap("mjwf_data_qpos_ptr", "number", ["number"]);

const modelPtr = compile(specPtr, 0);
if (!modelPtr) throw new Error("mj_compile returned null");

const dataPtr = makeData(modelPtr);
if (!dataPtr) throw new Error("mj_makeData returned null");

const nq = getNq(modelPtr);
const qposPtr = getQposPtr(modelPtr);
const qposView =
  qposPtr && nq > 0 ? Module.HEAPF64.subarray(qposPtr >> 3, (qposPtr >> 3) + nq) : null;
const qposCopy = readBack && qposView ? new Float64Array(qposView.length) : null;

const makeCall = (name) => ensureFn(bindExport(name, null, ["number", "number"]), name);
const makeBatch = (name) => bindExport(name, null, ["number", "number", "number"]);

const forwardFn = ensureFn(bindExport("mjwf_mj_forward", null, ["number", "number"]), "mjwf_mj_forward");

const benchEntries = [
  {
    label: "mj_step",
    fn: makeCall("mjwf_mj_step"),
    setup: () => resetData(modelPtr, dataPtr),
  },
  {
    label: "mj_forward",
    fn: makeCall("mjwf_mj_forward"),
    setup: () => resetData(modelPtr, dataPtr),
  },
  {
    label: "mj_sensorPos",
    fn: makeCall("mjwf_mj_sensorPos"),
    setup: () => resetData(modelPtr, dataPtr),
    batchFn: makeBatch("mjwf_bench_mj_sensorPos"),
  },
  {
    label: "mj_energyPos",
    fn: makeCall("mjwf_mj_energyPos"),
    setup: () => resetData(modelPtr, dataPtr),
    batchFn: makeBatch("mjwf_bench_mj_energyPos"),
  },
  {
    label: "mj_fwdConstraint",
    fn: makeCall("mjwf_mj_fwdConstraint"),
    setup: () => resetData(modelPtr, dataPtr),
    batchFn: makeBatch("mjwf_bench_mj_fwdConstraint"),
  },
];

const results = [];
for (const entry of benchEntries) {
  results.push(runBenchmark(entry));
}

for (const r of results) {
  console.log(
    `${r.label}: iterations=${r.iterations}, total=${r.total.toFixed(
      3,
    )} ms, avg=${r.avg.toFixed(6)} ms`,
  );
}
console.log(
  "summary:",
  JSON.stringify(
    results.map(({ label, iterations, warmups, total, avg }) => ({
      label,
      iterations,
      warmups,
      totalMs: total,
      avgMs: avg,
    })),
    null,
    2,
  ),
);

cleanup();

function runBenchmark({ label, fn, setup, batchFn }) {
  const needsPreForward = preForward && label !== "mj_step";
  const runSetup = () => {
    if (setup) setup();
    if (needsPreForward) forwardFn(modelPtr, dataPtr);
  };

  if (batchMode && typeof batchFn === "function") {
    runSetup();
    if (WARMUPS > 0) batchFn(modelPtr, dataPtr, WARMUPS);
    runSetup();
    const start = performance.now();
    batchFn(modelPtr, dataPtr, ITERATIONS);
    if (readBack && qposView && qposCopy) {
      qposCopy.set(qposView);
    }
    const total = performance.now() - start;
    return { label, iterations: ITERATIONS, warmups: WARMUPS, total, avg: total / ITERATIONS };
  }

  runSetup();
  for (let i = 0; i < WARMUPS; i += 1) fn(modelPtr, dataPtr);
  runSetup();
  const start = performance.now();
  for (let i = 0; i < ITERATIONS; i += 1) {
    fn(modelPtr, dataPtr);
    if (readBack && qposView && qposCopy) {
      qposCopy.set(qposView);
    }
  }
  const total = performance.now() - start;
  return { label, iterations: ITERATIONS, warmups: WARMUPS, total, avg: total / ITERATIONS };
}

function bindExport(name, returnType, argTypes) {
  const exportName = `_${name}`;
  const direct = Module.wasmExports?.[exportName];
  if (typeof direct === "function") {
    return direct;
  }
  const legacy = Module[exportName];
  if (typeof legacy === "function") {
    return legacy;
  }
  try {
    return Module.cwrap(name, returnType, argTypes);
  } catch {
    return null;
  }
}

function ensureFn(fn, name) {
  if (typeof fn === "function") {
    return fn;
  }
  throw new Error(`Required export missing: ${name}`);
}

async function ensureFile(p) {
  try {
    await access(p);
  } catch {
    throw new Error(`Required file missing: ${p}`);
  }
}

function cleanup() {
  deleteData(dataPtr);
  deleteModel(modelPtr);
  deleteSpec(specPtr);
  Module.stackRestore(stackTop);
}
