// Post-build export surface checker
// Usage: node scripts/mujoco_abi/check_exports.mjs dist/<ver>/abi dist/<ver>/mujoco.wasm
// Compares the expected export set (from ABI + exposure) with actual WASM exports.
// Fails on required-missing or excluded-leaks. Optional-missing is a warning.

import { readFileSync, writeFileSync, mkdirSync, existsSync } from 'node:fs';
import { join as pathJoin, resolve as pathResolve, dirname } from 'node:path';

function load(p) { return JSON.parse(readFileSync(p, 'utf8')); }
function ensureDir(p) { mkdirSync(p, { recursive: true }); }

function toSet(arr) { const s = new Set(); for (const a of arr) s.add(a); return s; }

function main() {
  const abiDir = pathResolve(process.argv[2] || '');
  const wasmPath = pathResolve(process.argv[3] || '');
  const expectedPath = process.argv[4] ? pathResolve(process.argv[4]) : null;
  if (!abiDir || !wasmPath) {
    console.error('Usage: node scripts/mujoco_abi/check_exports.mjs <abiDir> <wasmPath> [expected.json]');
    process.exit(2);
  }

  // expected sets
  const expRequired = new Set();
  const expOptional = new Set();
  const expExcluded = new Set();
  const expIgnore = new Set();
  if (expectedPath && existsSync(expectedPath)) {
    const cfg = load(expectedPath);
    for (const name of cfg.required || []) expRequired.add(name);
    for (const name of cfg.optional || []) expOptional.add(name);
    for (const name of cfg.excluded || []) expExcluded.add(name);
    for (const name of cfg.runtime_keep || []) expIgnore.add(name);
  } else {
    const fjson = load(pathJoin(abiDir, 'functions.json'));
    const functions = fjson.functions || [];
    for (const f of functions) {
      const name = f.name;
      if (f.exposure === 'excluded') { expExcluded.add(name); continue; }
      if (f.gate === 'required') expRequired.add(name);
      else if (f.gate === 'optional') expOptional.add(name);
    }
  }

  // WASM exports
  const wasmBytes = readFileSync(wasmPath);
  const mod = new WebAssembly.Module(wasmBytes);
  const exp = WebAssembly.Module.exports(mod).map(e => e.name);
  const expSet = toSet(exp);

  // Helper to check both with and without leading underscore
  const has = (nm) => expSet.has('_' + nm) || expSet.has(nm);

  const missingRequired = Array.from(expRequired).filter(n => !has(n)).sort();
  const missingOptional = Array.from(expOptional).filter(n => !has(n)).sort();
  const leakedExcluded = Array.from(expExcluded).filter(n => has(n) && !expIgnore.has(n) && !expIgnore.has('_' + n)).sort();

  const report = {
    wasm: wasmPath,
    expected: {
      required: Array.from(expRequired).length,
      optional: Array.from(expOptional).length,
      excluded: Array.from(expExcluded).length,
    },
    actual: { exportCount: exp.length },
    missingRequired,
    missingOptional,
    leakedExcluded,
    ok: (missingRequired.length === 0) && (leakedExcluded.length === 0),
  };

  const outPath = pathJoin(abiDir, 'exports_check.json');
  ensureDir(dirname(outPath));
  writeFileSync(outPath, JSON.stringify(report, null, 2));
  console.log(`[exports-check] wrote ${outPath}`);
  console.log(`[exports-check] status ok=${report.ok} missingRequired=${missingRequired.length} leakedExcluded=${leakedExcluded.length}`);
  if (missingOptional.length) {
    console.warn(`[exports-check] optional exports missing (${missingOptional.length}):`, missingOptional.slice(0, 20));
    if (missingOptional.length > 20) console.warn('...');
  }
  if (!report.ok) process.exit(1);
}

main();
