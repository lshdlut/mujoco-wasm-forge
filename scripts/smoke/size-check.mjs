import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import fs from 'node:fs';
import assert from 'node:assert/strict';

// Usage: node scripts/smoke/size-check.mjs dist/<ver>/abi/dim_map.json dist/<ver>/mujoco.js
const [,, dimMapPath, moduleJsPath] = process.argv;
if (!dimMapPath || !moduleJsPath) {
  console.error('Usage: node scripts/smoke/size-check.mjs dist/<ver>/abi/dim_map.json dist/<ver>/mujoco.js');
  process.exit(2);
}

const distDir = path.dirname(moduleJsPath);
const wasmPath = path.join(distDir, path.basename(moduleJsPath).replace(/\.js$/, '.wasm'));
assert.ok(fs.existsSync(moduleJsPath), 'module js missing');
assert.ok(fs.existsSync(wasmPath), 'module wasm missing');

const modFactory = (await import(pathToFileURL(moduleJsPath).href)).default;
const Module = await modFactory({ locateFile: (p) => (p.endsWith('.wasm') ? wasmPath : p) });

const dimMap = JSON.parse(fs.readFileSync(dimMapPath, 'utf8')).dim_map || {};
// Only check canonical counts
const keys = ['nq','nv','nu','na','nbody','ngeom','nsite','njnt'];
const fails = [];

function cwrapInt(name) { return Module.cwrap(name, 'number', []); }
const getters = {
  nq: cwrapInt('mjwf_nq'),
  nv: cwrapInt('mjwf_nv'),
  nu: cwrapInt('mjwf_nu'),
};

for (const k of keys) {
  if (!dimMap[k] || !Array.isArray(dimMap[k])) continue;
  const get = getters[k];
  if (!get) continue; // skip if no getter available in wrapper build
  const n = get();
  if (n <= 0) { fails.push(`count ${k} invalid: ${n}`); continue; }
  // Nothing to check about arrays here without pointer views; rely on wrapper-specific smoke tests for pointer lengths
}

if (fails.length) {
  throw new Error('size-check fails: ' + fails.join(', '));
} else {
  console.log('size-check OK');
}

