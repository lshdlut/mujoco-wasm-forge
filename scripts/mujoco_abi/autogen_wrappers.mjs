#!/usr/bin/env node

/**
 * Scan MuJoCo public headers and enumerate every MJAPI function name.
 *
 * Input: include directory that contains mujoco/mujoco.h (and optionally mujoco/mjspec.h).
 * Output: JSON payload listing discovered function names (A-set for export pipeline).
 *
 * Usage:
 *   node scripts/mujoco_abi/autogen_wrappers.mjs \
 *        --include external/mujoco/include \
 *        --out build/mjapi_functions.json
 */

import { readFileSync, writeFileSync, existsSync, mkdirSync } from 'node:fs';
import { resolve as pathResolve, join as pathJoin, dirname } from 'node:path';

const TARGET_HEADERS = ['mujoco/mujoco.h', 'mujoco/mjspec.h'];

function parseArgs(argv) {
  const opts = {
    includeDir: 'external/mujoco/include',
    outPath: null,
  };
  for (let i = 2; i < argv.length; ++i) {
    const arg = argv[i];
    if (arg === '--include') {
      opts.includeDir = argv[++i];
    } else if (arg === '--out') {
      opts.outPath = argv[++i];
    } else {
      console.error(`Unknown argument: ${arg}`);
      process.exit(2);
    }
  }
  if (!opts.outPath) {
    console.error('Missing --out <path> for output JSON.');
    process.exit(2);
  }
  return {
    includeDir: pathResolve(opts.includeDir),
    outPath: pathResolve(opts.outPath),
  };
}

function ensureDirFor(filePath) {
  mkdirSync(dirname(filePath), { recursive: true });
}

function stripComments(source) {
  const withoutBlock = source.replace(/\/\*[\s\S]*?\*\//g, ' ');
  return withoutBlock
    .split('\n')
    .map((line) => {
      const idx = line.indexOf('//');
      return idx >= 0 ? line.slice(0, idx) : line;
    })
    .join('\n');
}

function splitStatements(text) {
  const stmts = [];
  let buf = '';
  let depth = 0;
  for (let i = 0; i < text.length; ++i) {
    const ch = text[i];
    buf += ch;
    if (ch === '(') depth++;
    else if (ch === ')') depth = Math.max(depth - 1, 0);
    if (ch === ';' && depth === 0) {
      const trimmed = buf.trim();
      if (trimmed) stmts.push(trimmed);
      buf = '';
    }
  }
  return stmts;
}

function normalizeWhitespace(value) {
  return value.replace(/\s+/g, ' ').trim();
}

function extractParameters(paramStr) {
  const params = [];
  let buf = '';
  let depth = 0;
  for (let i = 0; i < paramStr.length; ++i) {
    const ch = paramStr[i];
    if (ch === ',' && depth === 0) {
      if (buf.trim()) params.push(buf.trim());
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

function extractParamName(param, index) {
  if (!param) return `p${index}`;
  const variadic = param.includes('...');
  if (variadic) return `p${index}`;
  const aliasMatch = param.match(/\(\s*\*([A-Za-z_][A-Za-z0-9_]*)\s*\)/);
  if (aliasMatch) return aliasMatch[1];
  const nameMatch = param.match(/([A-Za-z_][A-Za-z0-9_]*)(\s*(\[[^\]]*\])*)\s*$/);
  if (nameMatch) return nameMatch[1];
  return `p${index}`;
}

function ensureParamDecl(param, name) {
  if (!param || param === 'void') return 'void';
  if (param.includes('...')) return param;
  const nameRegex = new RegExp(`\\b${name}\\b`);
  if (nameRegex.test(param)) return param;
  return `${param.replace(/\s+$/, '')} ${name}`;
}

function parseFunction(statement) {
  if (!statement.includes('MJAPI')) return null;
  if (!statement.includes('(')) return null;
  const normalized = normalizeWhitespace(statement);
  const idx = normalized.indexOf('MJAPI');
  if (idx < 0) return null;
  const body = normalized.slice(idx + 'MJAPI'.length).trim();
  const parenIdx = body.indexOf('(');
  if (parenIdx < 0) return null;
  const prefix = body.slice(0, parenIdx).trim();
  if (!prefix) return null;
  const nameMatch = prefix.match(/([A-Za-z_][A-Za-z0-9_]*)$/);
  if (!nameMatch) return null;
  const name = nameMatch[1];
  const returnType = prefix.slice(0, prefix.length - name.length).trim() || 'void';
  let depth = 1;
  let i = parenIdx + 1;
  for (; i < body.length; ++i) {
    const ch = body[i];
    if (ch === '(') depth++;
    else if (ch === ')') depth--;
    if (depth === 0) break;
  }
  if (depth !== 0) return null;
  const paramsRaw = body.slice(parenIdx + 1, i).trim();
  const rawParams = paramsRaw === '' ? [] : extractParameters(paramsRaw);
  const hasVoid = rawParams.length === 1 && rawParams[0].trim() === 'void';
  const params = hasVoid ? [] : rawParams;
  const paramNames = params.map((p, idx) => extractParamName(p, idx));
  const paramDecls = params.map((p, idx) => ensureParamDecl(p, paramNames[idx]));
  const variadicIndex = params.findIndex((p) => p.includes('...'));
  const isVariadic = variadicIndex !== -1;
  const baseParamNames = paramNames.filter((_, idx) => idx !== variadicIndex);
  return {
    name,
    returnType,
    params,
    paramDecls,
    paramNames,
    isVariadic,
    variadicIndex,
    baseParamNames,
  };
}

function collectFunctions(includeDir) {
  const functions = new Map();
  const scanned = [];
  for (const rel of TARGET_HEADERS) {
    const headerPath = pathJoin(includeDir, rel);
    if (!existsSync(headerPath)) continue;
    scanned.push(headerPath);
    const content = stripComments(readFileSync(headerPath, 'utf8'));
    const statements = splitStatements(content);
    for (const stmt of statements) {
      const fn = parseFunction(stmt);
      if (!fn) continue;
      if (!functions.has(fn.name)) {
        functions.set(fn.name, fn);
      }
    }
  }
  if (!scanned.length) {
    console.error(`No target headers found under ${includeDir}`);
    process.exit(1);
  }
  return { functions, scanned };
}

function main() {
  const opts = parseArgs(process.argv);
  const { functions, scanned } = collectFunctions(opts.includeDir);
  const sortedNames = Array.from(functions.keys()).sort();
  const nameSet = new Set(sortedNames);
  const functionList = sortedNames.map((name) => {
    const fn = functions.get(name);
    fn.has_v_alternative = fn.isVariadic ? nameSet.has(`${fn.name}_v`) : false;
    return fn;
  });
  const payload = {
    generatedAt: new Date().toISOString(),
    includeDir: opts.includeDir,
    headers: scanned,
    count: sortedNames.length,
    names: sortedNames,
    functions: functionList,
  };
  ensureDirFor(opts.outPath);
  writeFileSync(opts.outPath, JSON.stringify(payload, null, 2));
  console.log(`[scan-mjapi] headers=${scanned.length} names=${sortedNames.length} -> ${opts.outPath}`);
}

main();
