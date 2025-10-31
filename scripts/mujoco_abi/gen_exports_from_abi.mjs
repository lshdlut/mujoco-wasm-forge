// Minimal generator: read ABI JSON and produce exports_<ver>.json and types_<ver>.d.ts
// Usage: node scripts/mujoco_abi/gen_exports_from_abi.mjs dist/<ver>/abi

import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { join as pathJoin, dirname, resolve as pathResolve } from 'node:path';

function load(p) { return JSON.parse(readFileSync(p, 'utf8')); }
function ensureDir(p) { mkdirSync(p, { recursive: true }); }

function ctypeToTs(ty) {
  const t = ty.replace(/\s+const\b|\bconst\s+/g, '').trim();
  if (/\*/.test(t)) return 'number /* ptr */';
  if (/^(int|unsigned|size_t|mjtByte|uintptr_t|uint\d+_t|int\d+_t)$/.test(t)) return 'number';
  if (/^(float|double|mjtNum)$/.test(t)) return 'number';
  if (/char\s*\*/.test(t)) return 'number /* cstring ptr */';
  return 'number';
}

function emitDts(functions) {
  const lines = [];
  lines.push('// AUTO-GENERATED from ABI. Minimal d.ts surface.');
  lines.push('export interface Exports {');
  for (const f of functions) {
    const params = (f.params||[]).map(p => `${p.name||'p'}: ${ctypeToTs(p.type||'int')}`).join(', ');
    lines.push(`  ${f.name}(${params}): ${ctypeToTs(f.return||'int')};`);
  }
  lines.push('}');
  return lines.join('\n');
}

const abiDir = pathResolve(process.argv[2] || '.');
const fn = load(pathJoin(abiDir, 'functions.json'));
const meta = fn.meta || {}; const ver = meta.ref || 'unknown';
const functions = fn.functions || [];

// Select exports: gate in required|optional and not exposure==='excluded'
const selected = functions.filter(f => (f.gate==='required' || f.gate==='optional') && f.exposure !== 'excluded');

const outDir = pathResolve('build');
ensureDir(outDir);
writeFileSync(pathJoin(outDir, `exports_${ver}.json`), JSON.stringify({ version: ver, count: selected.length, exports: selected.map(f=>f.name) }, null, 2));
writeFileSync(pathJoin(outDir, `types_${ver}.d.ts`), emitDts(selected));

console.log(`[gen-exports] wrote build/exports_${ver}.json and build/types_${ver}.d.ts (count=${selected.length})`);
