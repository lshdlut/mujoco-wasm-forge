#!/usr/bin/env node

// Generate wrapper export whitelist / d.ts from wrapper headers.
// Example:
//   node scripts/mujoco_abi/gen_exports_from_abi.mjs dist/3.3.7/abi \
//        --header wrappers/auto/mjwf_auto_exports.h \
//        --header wrappers/official_app_337/include/mjwf_exports.h \
//        --version 3.3.7 --out build

import { readFileSync, writeFileSync, mkdirSync, existsSync } from 'node:fs';
import { join as pathJoin, resolve as pathResolve, dirname } from 'node:path';

function ensureDir(p) { mkdirSync(p, { recursive: true }); }

function parseArgs(argv) {
  if (argv.length < 3) {
    console.error('Usage: node scripts/mujoco_abi/gen_exports_from_abi.mjs <abiDir> [--header <path> ...] [--version 3.3.7] [--out build]');
    process.exit(2);
  }
  const opts = {
    abiDir: pathResolve(argv[2]),
    headers: [],
    version: null,
    outDir: 'build',
    noAbiCopy: false,
  };
  for (let i = 3; i < argv.length; ++i) {
    const arg = argv[i];
    if (arg === '--header') opts.headers.push(pathResolve(argv[++i]));
    else if (arg === '--version') opts.version = argv[++i];
    else if (arg === '--out') opts.outDir = argv[++i];
    else if (arg === '--no-abi-copy') opts.noAbiCopy = true;
    else {
      console.error(`Unknown argument: ${arg}`);
      process.exit(2);
    }
  }
  return opts;
}

function stripComments(text) {
  const withoutBlock = text.replace(/\/\*[\s\S]*?\*\//g, ' ');
  return withoutBlock
    .split('\n')
    .map((line) => {
      const idx = line.indexOf('//');
      return idx >= 0 ? line.slice(0, idx) : line;
    })
    .join('\n');
}

function splitStatements(text) {
  const statements = [];
  let buf = '';
  for (let i = 0; i < text.length; ++i) {
    const ch = text[i];
    buf += ch;
    if (ch === ';') {
      const trimmed = buf.trim();
      if (trimmed.length) statements.push(trimmed);
      buf = '';
    }
  }
  return statements;
}

function splitParams(paramStr) {
  const params = [];
  let buf = '';
  let depth = 0;
  for (let i = 0; i < paramStr.length; ++i) {
    const ch = paramStr[i];
    if (ch === ',' && depth === 0) {
      params.push(buf.trim());
      buf = '';
      continue;
    }
    if (ch === '(' || ch === '[') depth++;
    else if (ch === ')' || ch === ']') depth = Math.max(depth - 1, 0);
    buf += ch;
  }
  if (buf.trim()) params.push(buf.trim());
  return params;
}

function extractParamPieces(param, index) {
  if (!param || param === 'void') return null;
  const variadic = param.includes('...');
  if (variadic) {
    return { type: '...', name: `p${index}` };
  }
  const aliasMatch = param.match(/\(\s*\*([A-Za-z_][A-Za-z0-9_]*)\s*\)/);
  if (aliasMatch) {
    return { type: param, name: aliasMatch[1] };
  }
  const nameMatch = param.match(/([A-Za-z_][A-Za-z0-9_]*)(\s*(\[[^\]]*\])*)\s*$/);
  let name = nameMatch ? nameMatch[1] : `p${index}`;
  let type = param;
  const pos = param.lastIndexOf(name);
  if (pos >= 0) type = param.slice(0, pos).trim();
  if (!type) type = 'void';
  return { type: type.trim(), name: name.trim() };
}

function parseHeaderFile(headerPath) {
  const text = stripComments(readFileSync(headerPath, 'utf8'));
  const statements = splitStatements(text);
  const out = [];
  for (const stmt of statements) {
    if (!stmt.includes('mjwf_')) continue;
    if (!stmt.includes('(')) continue;
    let cleaned = stmt.replace(/\s+/g, ' ').trim();
    if (cleaned.endsWith(';')) cleaned = cleaned.slice(0, -1).trim();
    cleaned = cleaned.replace(/\bMJWF_API\b/g, '')
      .replace(/\bEMSCRIPTEN_KEEPALIVE\b/g, '')
      .replace(/\bextern\b/g, '')
      .replace(/\b__attribute__\([\s\S]*?\)/g, '')
      .trim();
    const match = cleaned.match(/^(.*?)\s+(mjwf_[A-Za-z0-9_]+)\s*\((.*)\)$/);
    if (!match) continue;
    let returnType = match[1].trim();
    const name = match[2].trim();
    let paramStr = match[3].trim();
    if (paramStr.endsWith(';')) paramStr = paramStr.slice(0, -1).trim();
    if (!returnType) returnType = 'void';
    const params = paramStr ? splitParams(paramStr) : [];
    const parsedParams = [];
    if (!(params.length === 1 && params[0] === 'void') && params.length > 0) {
      params.forEach((p, idx) => {
        const piece = extractParamPieces(p, idx);
        if (piece) parsedParams.push(piece);
      });
    }
    out.push({ name, returnType, params: parsedParams });
  }
  return out;
}

function ctypeToTs(ty) {
  const t = ty.replace(/\s+const\b|\bconst\s+/g, '').trim();
  if (t === '...') return 'any';
  if (/\*/.test(t)) return 'number /* ptr */';
  if (/^(int|unsigned|size_t|mjtByte|uintptr_t|uint\d+_t|int\d+_t|mj|long)/.test(t)) return 'number';
  if (/^(float|double|mjtNum)$/.test(t)) return 'number';
  if (/char\s*\*/.test(t)) return 'number /* cstring ptr */';
  return 'number';
}

function emitDts(functions) {
  const lines = [];
  lines.push('// AUTO-GENERATED from wrapper headers.');
  lines.push('export interface Exports {');
  for (const f of functions) {
    const params = (f.params || []).map((p, idx) => `${p.name || 'p' + idx}: ${ctypeToTs(p.type || 'int')}`).join(', ');
    lines.push(`  ${f.name}(${params}): ${ctypeToTs(f.returnType || 'int')};`);
  }
  lines.push('}');
  return lines.join('\n');
}

const opts = parseArgs(process.argv);
const manifest = new Map();
for (const headerPath of opts.headers) {
  if (!existsSync(headerPath)) {
    console.error(`Header not found: ${headerPath}`);
    process.exit(1);
  }
  for (const entry of parseHeaderFile(headerPath)) {
    if (!manifest.has(entry.name)) {
      manifest.set(entry.name, entry);
    }
  }
}

if (!manifest.size) {
  const fallback = pathJoin(opts.abiDir, 'functions.json');
  if (existsSync(fallback)) {
    const abiFunctions = JSON.parse(readFileSync(fallback, 'utf8'));
    for (const f of abiFunctions.functions || []) {
      if (!f.name?.startsWith('mjwf_')) continue;
      if (!manifest.has(f.name)) {
        manifest.set(f.name, {
          name: f.name,
          returnType: f.return || 'int',
          params: (f.params || []).map((p, idx) => ({ type: Array.isArray(p) ? p[0] : 'int', name: `p${idx}` })),
        });
      }
    }
  }
}

const functions = Array.from(manifest.values()).sort((a, b) => a.name.localeCompare(b.name));
const version = opts.version || (existsSync(pathJoin(opts.abiDir, 'functions.json'))
  ? (JSON.parse(readFileSync(pathJoin(opts.abiDir, 'functions.json'), 'utf8')).meta?.ref || 'unknown')
  : 'unknown');

const exportsJson = {
  version,
  count: functions.length,
  exports: functions.map((f) => f.name),
  required: functions.map((f) => f.name),
  optional: [],
  runtime_keep: [
    '_malloc',
    '_free',
    '_memcpy',
    '_memmove',
    '_memset',
    '_emscripten_stack_init',
    '_emscripten_stack_get_current',
    '_emscripten_stack_get_end',
  ],
};

const outDir = pathResolve(opts.outDir);
ensureDir(outDir);
writeFileSync(pathJoin(outDir, `exports_${version}.json`), JSON.stringify(exportsJson, null, 2));
writeFileSync(pathJoin(outDir, `exports_${version}.lst`), JSON.stringify(exportsJson.exports.map(name => `_${name}`)));
writeFileSync(pathJoin(outDir, `types_${version}.d.ts`), emitDts(functions));

if (!opts.noAbiCopy) {
  ensureDir(opts.abiDir);
  writeFileSync(pathJoin(opts.abiDir, 'wrapper_exports.json'), JSON.stringify(exportsJson, null, 2));
}

console.log(`[gen-exports] wrote ${pathJoin(outDir, `exports_${version}.json`)} and types_${version}.d.ts (count=${functions.length})`);
