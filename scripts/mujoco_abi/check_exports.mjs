#!/usr/bin/env node

// Validate WASM export surface against expected wrapper whitelist.
// Usage: node scripts/mujoco_abi/check_exports.mjs <abiDir> <wasmPath> <expectedJson>

import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { join as pathJoin, resolve as pathResolve, dirname } from 'node:path';

function ensureDir(p) { mkdirSync(p, { recursive: true }); }

function loadJson(p) { return JSON.parse(readFileSync(p, 'utf8')); }

function normalizeExport(name) {
  if (name.startsWith('_')) return name.slice(1);
  return name;
}

function main() {
  const abiDir = pathResolve(process.argv[2] || '');
  const wasmPath = pathResolve(process.argv[3] || '');
  const expectedPath = pathResolve(process.argv[4] || '');
  if (!abiDir || !wasmPath || !expectedPath) {
    console.error('Usage: node scripts/mujoco_abi/check_exports.mjs <abiDir> <wasmPath> <expectedJson>');
    process.exit(2);
  }

  const expected = loadJson(expectedPath);
  const required = new Set(expected.required || expected.exports || []);
  const optional = new Set(expected.optional || []);
  const runtimeKeep = new Set((expected.runtime_keep || []).map((n) => n.replace(/^_/, '')));

  const wasmBytes = readFileSync(wasmPath);
  const module = new WebAssembly.Module(wasmBytes);
  const exports = WebAssembly.Module.exports(module).map((e) => e.name);

  const actualWrappers = new Set();
  const unexpected = [];

  const allowedRuntime = new Set([
    '__wasm_call_ctors',
    '__wasm_apply_data_relocations',
    '__wasm_init_memory_flag',
    '__stack_pointer',
    '__heap_base',
    '__data_end',
    '__global_base',
    '__memory_base',
    '__table_base',
    '__indirect_function_table',
    'memory',
    'stackSave',
    'stackRestore',
    'stackAlloc',
    'setThrew',
    'setTempRet0',
    'emscripten_stack_get_current',
    'emscripten_stack_get_end',
    'emscripten_stack_get_base',
    'emscripten_stack_init',
    'emscripten_stack_set_limits',
  ]);

  for (const name of exports) {
    const plain = normalizeExport(name);
    if (plain.startsWith('mjwf_')) {
      actualWrappers.add(plain);
      continue;
    }
    if (runtimeKeep.has(plain) || runtimeKeep.has(name)) continue;
    if (allowedRuntime.has(name) || allowedRuntime.has(plain)) continue;
    if (/^_?mj(v|r|ui)_/.test(name) || /^mj(v|r|ui)_/.test(plain)) {
      unexpected.push(name);
      continue;
    }
    if (plain.startsWith('mj') || plain.startsWith('mju_') || plain.startsWith('mjs_')) {
      unexpected.push(name);
    }
  }

  const missingRequired = Array.from(required).filter((sym) => !actualWrappers.has(sym)).sort();
  const missingOptional = Array.from(optional).filter((sym) => !actualWrappers.has(sym)).sort();

  const report = {
    wasm: wasmPath,
    expected: {
      required: required.size,
      optional: optional.size,
      runtime_keep: runtimeKeep.size,
    },
    actual: {
      exportCount: exports.length,
      wrappers: actualWrappers.size,
    },
    missingRequired,
    missingOptional,
    unexpected,
    ok: missingRequired.length === 0 && unexpected.length === 0,
  };

  const outPath = pathJoin(abiDir, 'exports_check.json');
  ensureDir(dirname(outPath));
  writeFileSync(outPath, JSON.stringify(report, null, 2));
  console.log(`[exports-check] wrote ${outPath}`);
  console.log(`[exports-check] status ok=${report.ok} missingRequired=${missingRequired.length} unexpected=${unexpected.length}`);
  if (missingOptional.length) {
    console.warn(`[exports-check] optional exports missing (${missingOptional.length}):`, missingOptional.slice(0, 20));
    if (missingOptional.length > 20) console.warn('...');
  }
  if (unexpected.length) {
    console.error('[exports-check] unexpected exports:', unexpected.slice(0, 20));
    if (unexpected.length > 20) console.error('...');
  }
  if (!report.ok) process.exit(1);
}

main();
