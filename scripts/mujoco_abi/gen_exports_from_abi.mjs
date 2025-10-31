// Generate wrapper export whitelist / d.ts from ABI + wrapper headers
// Usage: node scripts/mujoco_abi/gen_exports_from_abi.mjs <abiDir> [--header path] [--version 3.3.7]

import { readFileSync, writeFileSync, mkdirSync, existsSync } from 'node:fs';
import { join as pathJoin, dirname, resolve as pathResolve } from 'node:path';

function load(p) { return JSON.parse(readFileSync(p, 'utf8')); }
function ensureDir(p) { mkdirSync(p, { recursive: true }); }

function parseArgs(argv) {
  const opts = { abiDir: null, header: null, version: null, outDir: 'build' };
  if (argv.length < 3) throw new Error('Usage: node scripts/mujoco_abi/gen_exports_from_abi.mjs <abiDir> [--header hdr.h] [--version 3.3.7]');
  opts.abiDir = pathResolve(argv[2]);
  for (let i = 3; i < argv.length; i++) {
    const a = argv[i];
    if (a === '--header') opts.header = pathResolve(argv[++i]);
    else if (a === '--version') opts.version = argv[++i];
    else if (a === '--out') opts.outDir = pathResolve(argv[++i]);
    else if (a === '--no-abi-copy') opts.noAbiCopy = true;
    else throw new Error(`Unknown arg: ${a}`);
  }
  return opts;
}

function ctypeToTs(ty) {
  const t = ty.replace(/\s+const\b|\bconst\s+/g, '').trim();
  if (/\*/.test(t)) return 'number /* ptr */';
  if (/^(int|unsigned|size_t|mjtByte|uintptr_t|uint\d+_t|int\d+_t|mj|long)/.test(t)) return 'number';
  if (/^(float|double|mjtNum)$/.test(t)) return 'number';
  if (/char\s*\*/.test(t)) return 'number /* cstring ptr */';
  return 'number';
}

function parseHeaderExports(text) {
  const out = [];
  const re = /EMSCRIPTEN_KEEPALIVE\s+([\s\S]+?)\s+(mjwf_[a-zA-Z0-9_]+)\s*\(([\s\S]*?)\)\s*;/g;
  let m;
  while ((m = re.exec(text)) !== null) {
    const ret = m[1].replace(/\s+/g, ' ').trim();
    const name = m[2].trim();
    const paramsRaw = m[3].trim();
    const params = [];
    if (paramsRaw && paramsRaw !== 'void') {
      for (const part of paramsRaw.split(',')) {
        const seg = part.trim();
        if (!seg) continue;
        const mm = seg.match(/(.+?)\s+([A-Za-z0-9_\*\[\]]+)$/);
        if (mm) params.push({ type: mm[1].trim(), name: mm[2].trim() });
        else params.push({ type: seg, name: `p${params.length}` });
      }
    }
    out.push({ name, return: ret, params });
  }
  return out;
}

function emitDts(functions) {
  const lines = [];
  lines.push('// AUTO-GENERATED from wrapper header.');
  lines.push('export interface Exports {');
  for (const f of functions) {
    const params = (f.params || []).map(p => `${p.name || 'p'}: ${ctypeToTs(p.type || 'int')}`).join(', ');
    lines.push(`  ${f.name}(${params}): ${ctypeToTs(f.return || 'int')};`);
  }
  lines.push('}');
  return lines.join('\n');
}

const opts = parseArgs(process.argv);
const abiDir = opts.abiDir;
const abiFunctions = existsSync(pathJoin(abiDir, 'functions.json')) ? load(pathJoin(abiDir, 'functions.json')) : { meta: {}, functions: [] };
const meta = abiFunctions.meta || {};
const inferredVersion = opts.version || meta.ref || 'unknown';

let headerFunctions = [];
if (opts.header) {
  const headerText = readFileSync(opts.header, 'utf8');
  headerFunctions = parseHeaderExports(headerText);
}

if (!headerFunctions.length) {
  // fallback to ABI functions filtered by gate/exposure
  headerFunctions = (abiFunctions.functions || []).filter(f => (f.gate === 'required' || f.gate === 'optional') && f.exposure !== 'excluded')
    .map(f => ({ name: f.name, return: f.return || 'int', params: f.params || [] }));
}

const unique = new Map();
for (const f of headerFunctions) {
  if (!unique.has(f.name)) unique.set(f.name, f);
}
const sorted = Array.from(unique.values()).sort((a, b) => a.name.localeCompare(b.name));
const required = sorted.map(f => f.name);

const outDir = pathResolve(opts.outDir);
ensureDir(outDir);
const exportsJson = { version: inferredVersion, required, optional: [], runtime_keep: ['_malloc','_free','_memcpy','_memmove','_memset','_emscripten_stack_init','_emscripten_stack_get_current','_emscripten_stack_get_end'] };
writeFileSync(pathJoin(outDir, `exports_${inferredVersion}.json`), JSON.stringify(exportsJson, null, 2));
writeFileSync(pathJoin(outDir, `types_${inferredVersion}.d.ts`), emitDts(sorted));

if (!opts.noAbiCopy) {
  ensureDir(abiDir);
  writeFileSync(pathJoin(abiDir, 'wrapper_exports.json'), JSON.stringify(exportsJson, null, 2));
}

console.log(`[gen-exports] wrote build/exports_${inferredVersion}.json, types_${inferredVersion}.d.ts (count=${required.length})`);
