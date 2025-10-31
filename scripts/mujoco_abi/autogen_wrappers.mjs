#!/usr/bin/env node

/**
 * Auto-generate wrapper functions for MuJoCo public C API.
 *
 * Scans MuJoCo headers under --include (defaults to external/mujoco/include),
 * selects functions that match the allow-list prefixes (mj_*, mju_*, mjs_),
 * excluding visualization/render/ui families (mjv_*, mjr_*, mjui_*), and writes
 * wrapper declarations/definitions that forward to the original symbols while
 * exposing new mjwf_* names suitable for Emscripten exports.
 *
 * Outputs:
 *  - Header file (default wrappers/auto/mjwf_auto_exports.h)
 *  - Source file (default wrappers/auto/mjwf_auto_exports.c)
 *  - JSON manifest listing generated wrapper names (optional --json)
 *
 * Usage:
 *   node scripts/mujoco_abi/autogen_wrappers.mjs \
 *     --include external/mujoco/include \
 *     --header wrappers/auto/mjwf_auto_exports.h \
 *     --source wrappers/auto/mjwf_auto_exports.c \
 *     --json wrappers/auto/mjwf_auto_exports.json
 */

import { readFileSync, writeFileSync, readdirSync, mkdirSync } from 'node:fs';
import { join as pathJoin, resolve as pathResolve, dirname, extname } from 'node:path';

const ALLOW_PREFIX = [
  /^mj(?![vru]_)/, // mj_* but not mjv_, mjr_, mjui_
  /^mju_/,
  /^mjs_/,
];
const DENY_PREFIX = [
  /^mjv_/,
  /^mjr_/,
  /^mjui_/,
];

function parseArgs(argv) {
  const opts = {
    include: 'external/mujoco/include',
    header: 'wrappers/auto/mjwf_auto_exports.h',
    source: 'wrappers/auto/mjwf_auto_exports.c',
    json: null,
  };
  for (let i = 2; i < argv.length; ++i) {
    const arg = argv[i];
    if (arg === '--include') opts.include = argv[++i];
    else if (arg === '--header') opts.header = argv[++i];
    else if (arg === '--source') opts.source = argv[++i];
    else if (arg === '--json') opts.json = argv[++i];
    else {
      console.error(`Unknown argument: ${arg}`);
      process.exit(2);
    }
  }
  return {
    include: pathResolve(opts.include),
    header: pathResolve(opts.header),
    source: pathResolve(opts.source),
    json: opts.json ? pathResolve(opts.json) : null,
  };
}

function readHeaderFiles(dir) {
  const files = [];
  const entries = readdirSync(dir, { withFileTypes: true });
  for (const entry of entries) {
    if (entry.name.startsWith('.')) continue;
    const full = pathJoin(dir, entry.name);
    if (entry.isDirectory()) {
      files.push(...readHeaderFiles(full));
    } else if (entry.isFile() && extname(entry.name) === '.h') {
      files.push(full);
    }
  }
  return files;
}

function stripBlockComments(text) {
  return text.replace(/\/\*[\s\S]*?\*\//g, ' ');
}

function normalizeWhitespace(txt) {
  return txt.replace(/\r\n/g, '\n').replace(/\s+/g, ' ').trim();
}

function shouldInclude(name) {
  if (!name) return false;
  if (DENY_PREFIX.some((re) => re.test(name))) return false;
  return ALLOW_PREFIX.some((re) => re.test(name));
}

function splitStatements(text) {
  const statements = [];
  let buf = '';
  for (let i = 0; i < text.length; ++i) {
    const ch = text[i];
    buf += ch;
    if (ch === ';') {
      const trimmed = buf.trim();
      if (trimmed) statements.push(trimmed);
      buf = '';
    }
  }
  return statements;
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

function extractParamName(param) {
  if (!param) return '';
  const variadic = param.trim() === '...' || param.includes('...');
  if (variadic) return '/*__variadic__*/';
  const fnPtrMatch = param.match(/\(\s*\*([A-Za-z_][A-Za-z0-9_]*)\s*\)/);
  if (fnPtrMatch) return fnPtrMatch[1];
  const arrayMatch = param.match(/([A-Za-z_][A-Za-z0-9_]*)(\s*(\[[^\]]*\]\s*)+)$/);
  if (arrayMatch) return arrayMatch[1];
  const simpleMatch = param.match(/([A-Za-z_][A-Za-z0-9_]*)$/);
  if (simpleMatch) return simpleMatch[1];
  return '';
}

function parseFunction(statement) {
  if (!statement.includes('MJAPI')) return null;
  if (!statement.includes('(')) return null;
  if (statement.includes('=') && statement.includes('{')) return null; // skip definitions
  const normalized = normalizeWhitespace(statement);
  const idx = normalized.indexOf('MJAPI');
  if (idx < 0) return null;
  let body = normalized.slice(idx + 5).trim();
  if (!body.includes('(')) return null;
  const parenIndex = body.indexOf('(');
  const prefix = body.slice(0, parenIndex).trim();
  if (!prefix) return null;
  let depth = 1;
  let i = parenIndex + 1;
  for (; i < body.length; ++i) {
    const ch = body[i];
    if (ch === '(') depth++;
    else if (ch === ')') depth--;
    if (depth === 0) break;
  }
  if (depth !== 0) return null;
  const paramsRaw = body.slice(parenIndex + 1, i).trim();
  const suffix = body.slice(i + 1).trim();
  const nameMatch = prefix.match(/([A-Za-z_][A-Za-z0-9_]*)$/);
  if (!nameMatch) return null;
  const name = nameMatch[1];
  if (!shouldInclude(name)) return null;
  const returnType = prefix.slice(0, prefix.length - name.length).trim() || 'void';
  const params = paramsRaw === '' ? [] : extractParameters(paramsRaw);
  const paramNames = params.map((p) => extractParamName(p));
  const isVariadic = params.some((p) => p.includes('...'));
  return {
    name,
    returnType,
    params,
    paramNames,
    isVariadic,
    raw: normalized,
    suffix,
  };
}

function collectFunctions(includeDir) {
  const files = readHeaderFiles(includeDir);
  const map = new Map();
  for (const file of files) {
    const text = stripBlockComments(readFileSync(file, 'utf8'));
    const statements = splitStatements(text);
    let hit = 0;
    for (const stmt of statements) {
      const fn = parseFunction(stmt);
      if (!fn) continue;
      if (!map.has(fn.name)) {
        map.set(fn.name, { ...fn, source: file });
        hit++;
      }
    }
    if (hit > 0) {
      console.log(`[autogen] ${file} -> ${hit} functions`);
    }
  }
  return map;
}

function ensureDirFor(filePath) {
  const dir = dirname(filePath);
  mkdirSync(dir, { recursive: true });
}

function renderHeader(functions) {
  const lines = [];
  lines.push('// AUTO-GENERATED by autogen_wrappers.mjs. DO NOT EDIT.');
  lines.push('#pragma once');
  lines.push('');
  lines.push('#include <mujoco/mujoco.h>');
  lines.push('');
  lines.push('#ifdef __cplusplus');
  lines.push('extern "C" {');
  lines.push('#endif');
  lines.push('');
  lines.push('#if defined(__EMSCRIPTEN__)');
  lines.push('#  include <emscripten/emscripten.h>');
  lines.push('#  define MJWF_API EMSCRIPTEN_KEEPALIVE __attribute__((used, visibility("default")) )');
  lines.push('#else');
  lines.push('#  define MJWF_API __attribute__((used, visibility("default")) )');
  lines.push('#endif');
  lines.push('');
  for (const fn of functions) {
    lines.push(`MJWF_API ${fn.returnType} mjwf_${fn.name}(${fn.params.join(', ') || 'void'});`);
  }
  lines.push('');
  lines.push('#undef MJWF_API');
  lines.push('');
  lines.push('#ifdef __cplusplus');
  lines.push('}  // extern "C"');
  lines.push('#endif');
  lines.push('');
  return lines.join('\n');
}

function renderSource(functions) {
  const lines = [];
  lines.push('// AUTO-GENERATED by autogen_wrappers.mjs. DO NOT EDIT.');
  lines.push('#include "mjwf_auto_exports.h"');
  lines.push('#include <stdarg.h>');
  lines.push('');
  lines.push('#if defined(__EMSCRIPTEN__)');
  lines.push('#  include <emscripten/emscripten.h>');
  lines.push('#  define MJWF_API_IMPL EMSCRIPTEN_KEEPALIVE __attribute__((used, visibility("default")))');
  lines.push('#else');
  lines.push('#  define MJWF_API_IMPL __attribute__((used, visibility("default")))');
  lines.push('#endif');
  lines.push('');
  lines.push('#define MJWF_ALIAS(ret, name, params, target) \\\n  MJWF_API_IMPL ret name params __attribute__((alias(target)));');
  lines.push('');

  const seen = new Set();
  for (const fn of functions) {
    if (seen.has(fn.name)) continue;
    seen.add(fn.name);
    const paramsList = fn.params.join(', ') || 'void';
    if (fn.isVariadic) {
      lines.push(`MJWF_ALIAS(${fn.returnType}, mjwf_${fn.name}, (${paramsList}), "${fn.name}")`);
      continue;
    }
    const argNames = fn.paramNames.filter((n) => n && !n.startsWith('/*')).join(', ');
    const hasArgs = fn.params.length > 0 && argNames.length > 0;
    const callArgs = hasArgs ? argNames : '';
    const returnKeyword = fn.returnType === 'void' ? '' : 'return ';
    lines.push(`MJWF_API_IMPL ${fn.returnType} mjwf_${fn.name}(${paramsList}) {`);
    if (fn.params.length > 0 && (!argNames || argNames.split(',').length !== fn.params.length)) {
      // fallback: pass through using positional macros; mark unused
      fn.params.forEach((param, idx) => {
        const pname = fn.paramNames[idx];
        if (pname) {
          lines.push(`  (void)${pname};`);
        }
      });
      lines.push(`  ${returnKeyword}${fn.name}(${callArgs});`);
    } else {
      lines.push(`  ${returnKeyword}${fn.name}(${callArgs});`);
    }
    lines.push('}');
    lines.push('');
  }

  lines.push('#undef MJWF_API_IMPL');
  lines.push('#undef MJWF_ALIAS');
  lines.push('');
  return lines.join('\n');
}

function main() {
  const opts = parseArgs(process.argv);
  const functionsMap = collectFunctions(opts.include);
  const functions = Array.from(functionsMap.values()).sort((a, b) => a.name.localeCompare(b.name));
  if (!functions.length) {
    console.error('No functions discovered; aborting.');
    process.exit(1);
  }

  ensureDirFor(opts.header);
  ensureDirFor(opts.source);
  writeFileSync(opts.header, renderHeader(functions));
  writeFileSync(opts.source, renderSource(functions));
  console.log(`[autogen] wrote ${opts.header}`);
  console.log(`[autogen] wrote ${opts.source}`);

  if (opts.json) {
    ensureDirFor(opts.json);
    const payload = {
      generatedAt: new Date().toISOString(),
      include: opts.include,
      count: functions.length,
      functions: functions.map((f) => ({
        name: f.name,
        returnType: f.returnType,
        params: f.params,
        source: f.source,
        variadic: f.isVariadic,
      })),
    };
    writeFileSync(opts.json, JSON.stringify(payload, null, 2));
    console.log(`[autogen] wrote ${opts.json}`);
  }
}

main();
