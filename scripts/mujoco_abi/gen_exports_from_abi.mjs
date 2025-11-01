#!/usr/bin/env node

/**
 * Generate WASM export wrappers based on A intersect B with automatic exclusions.
 *
 *   A = public C API declarations (mujoco.h [+ mjspec.h])
 *   B = implemented symbols (llvm-nm -g --defined-only libmujoco.a)
 *   C = A intersect B after applying prefix + variadic rules
 */

import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { resolve as pathResolve, dirname, join as pathJoin } from 'node:path';

const ALLOWED_PREFIXES = [/^mj_/, /^mju_/, /^mjs_/, /^mjd_/];
const RUNTIME_KEEP = ['_malloc', '_free', '_realloc', 'stackSave', 'stackRestore', 'stackAlloc'];
const REPORT_MAX_LIST = 50;

function ensureDirFor(filePath) {
  mkdirSync(dirname(filePath), { recursive: true });
}

function parseArgs(argv) {
  const opts = {
    namesJson: null,
    implJson: null,
    headerOut: null,
    sourceOut: null,
    version: 'unknown',
    outDir: 'build',
    abiDir: null,
  };
  for (let i = 2; i < argv.length; ++i) {
    const arg = argv[i];
    if (arg === '--names-json') opts.namesJson = pathResolve(argv[++i]);
    else if (arg === '--impl-json') opts.implJson = pathResolve(argv[++i]);
    else if (arg === '--header') opts.headerOut = pathResolve(argv[++i]);
    else if (arg === '--source') opts.sourceOut = pathResolve(argv[++i]);
    else if (arg === '--version') opts.version = argv[++i];
    else if (arg === '--out') opts.outDir = pathResolve(argv[++i]);
    else if (arg === '--abi') opts.abiDir = pathResolve(argv[++i]);
    else {
      console.error(`Unknown argument: ${arg}`);
      process.exit(2);
    }
  }
  const missing = [];
  if (!opts.namesJson) missing.push('--names-json');
  if (!opts.implJson) missing.push('--impl-json');
  if (!opts.headerOut) missing.push('--header');
  if (!opts.sourceOut) missing.push('--source');
  if (!opts.abiDir) missing.push('--abi');
  if (missing.length) {
    console.error(`Missing required option(s): ${missing.join(', ')}`);
    process.exit(2);
  }
  return opts;
}

function loadHeaderInfo(jsonPath) {
  const data = JSON.parse(readFileSync(jsonPath, 'utf8'));
  const map = new Map();
  for (const entry of data.functions || []) {
    if (entry && entry.name) {
      map.set(entry.name, entry);
    }
  }
  const names = Array.isArray(data.names) ? data.names : Array.from(map.keys());
  const hasMjspec = Array.isArray(data.headers)
    ? data.headers.some((h) => h.endsWith('mujoco/mjspec.h'))
    : false;
  return { map, names: new Set(names), data, hasMjspec };
}

function loadImpl(jsonPath) {
  const data = JSON.parse(readFileSync(jsonPath, 'utf8'));
  const list = Array.isArray(data.symbols) ? data.symbols : [];
  return new Set(list);
}

function isAllowedPrefix(name) {
  return ALLOWED_PREFIXES.some((re) => re.test(name));
}

function generateHeader(finalFunctions) {
  const lines = [];
  lines.push('// AUTO-GENERATED: MuJoCo WASM wrapper forward declarations.');
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
  lines.push('#  define MJWF_API EMSCRIPTEN_KEEPALIVE __attribute__((used, visibility("default")))');
  lines.push('#else');
  lines.push('#  define MJWF_API __attribute__((used, visibility("default")))');
  lines.push('#endif');
  lines.push('');
  for (const fn of finalFunctions) {
    const decls = Array.isArray(fn.paramDecls) && fn.paramDecls.length ? fn.paramDecls : [];
    const paramList = decls.length ? decls.join(', ') : 'void';
    lines.push(`MJWF_API ${fn.returnType} mjwf_${fn.name}(${paramList});`);
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

function generateSource(finalFunctions) {
  const lines = [];
  lines.push('// AUTO-GENERATED: MuJoCo WASM wrapper implementations.');
  lines.push('#include "mjwf_auto_exports.h"');
  const needsStdarg = finalFunctions.some((fn) => fn.isVariadic && fn.has_v_alternative_effective);
  if (needsStdarg) {
    lines.push('#include <stdarg.h>');
  }
  lines.push('');
  lines.push('#if defined(__EMSCRIPTEN__)');
  lines.push('#  include <emscripten/emscripten.h>');
  lines.push('#  define MJWF_API_IMPL EMSCRIPTEN_KEEPALIVE __attribute__((used, visibility("default")))');
  lines.push('#else');
  lines.push('#  define MJWF_API_IMPL __attribute__((used, visibility("default")))');
  lines.push('#endif');
  lines.push('');
  for (const fn of finalFunctions) {
    const decls = Array.isArray(fn.paramDecls) && fn.paramDecls.length ? fn.paramDecls : [];
    const paramList = decls.length ? decls.join(', ') : 'void';
    const returnType = fn.returnType || 'void';
    if (fn.isVariadic && fn.has_v_alternative_effective) {
      const baseNames = Array.isArray(fn.baseParamNames) && fn.baseParamNames.length
        ? fn.baseParamNames
        : fn.paramNames.filter((_, idx) => idx !== fn.variadicIndex);
      if (!baseNames.length) {
        continue;
      }
      const lastNamed = baseNames[baseNames.length - 1];
      const callArgs = [...baseNames, 'args'].join(', ');
      const targetName = `${fn.name}_v`;
      lines.push(`MJWF_API_IMPL ${returnType} mjwf_${fn.name}(${paramList}) {`);
      lines.push('  va_list args;');
      lines.push(`  va_start(args, ${lastNamed});`);
      if (returnType.trim() === 'void') {
        lines.push(`  ${targetName}(${callArgs});`);
        lines.push('  va_end(args);');
        lines.push('}');
      } else {
        lines.push(`  ${returnType} result = ${targetName}(${callArgs});`);
        lines.push('  va_end(args);');
        lines.push('  return result;');
        lines.push('}');
      }
      lines.push('');
      continue;
    }
    const argNames = Array.isArray(fn.paramNames) && fn.paramNames.length
      ? fn.paramNames.join(', ')
      : '';
    const callExpr = argNames.length ? `${fn.name}(${argNames})` : `${fn.name}()`;
    const returnKeyword = returnType.trim() === 'void' ? '' : 'return ';
    lines.push(`MJWF_API_IMPL ${returnType} mjwf_${fn.name}(${paramList}) {`);
    lines.push(`  ${returnKeyword}${callExpr};`);
    lines.push('}');
    lines.push('');
  }
  lines.push('#undef MJWF_API_IMPL');
  lines.push('');
  return lines.join('\n');
}

function emitDts(finalFunctions) {
  const lines = [];
  lines.push('// AUTO-GENERATED: TypeScript declarations for mjwf exports.');
  lines.push('export interface MJWFExports {');
  for (const fn of finalFunctions) {
    lines.push(`  mjwf_${fn.name}(...args: number[]): number;`);
  }
  lines.push('}');
  return lines.join('\n');
}

function emitLst(finalNames) {
  const exported = finalNames.map((n) => `_mjwf_${n}`);
  return JSON.stringify(exported);
}

function sliceWithEllipsis(list, max = REPORT_MAX_LIST) {
  const slice = list.slice(0, max);
  const lines = slice.map((item) => `  - ${item}`);
  if (list.length > slice.length) {
    const remaining = list.length - slice.length;
    lines.push(`  - ... (+${remaining} more)`);
  }
  return lines;
}

function buildReport(opts) {
  const {
    version,
    generatedAt,
    hasMjspec,
    countA,
    countB,
    finalNames,
    countMj,
    countMju,
    countMjs,
    countMjd,
    specialExclusions,
    aMinusB,
    bMinusA,
    abMinusC,
  } = opts;

  const lines = [];
  lines.push('# MuJoCo WASM Export Report (ver ' + version + ')');
  lines.push('Generated at: ' + generatedAt);
  lines.push('');
  lines.push('## Special Exclusion Rules');
  lines.push('- Export prefixes allowed: `mj_`, `mju_`, `mjs_`, `mjd_`; other prefixes (e.g. `mjv_`, `mjr_`, `mjui_`, `mjp_`, `mjc_`) are removed.');
  lines.push('- Variadic functions are exported only when a matching `*_v` variant exists; otherwise they are recorded as `variadic_no_v`.');
  lines.push('- Export set is strictly `C = A intersect B`; no handcrafted helpers.');
  lines.push('');
  lines.push('## A, B, C Overview');
  lines.push('- **A (declarations)**: public C API discovered from `mujoco.h`' + (hasMjspec ? ' and `mjspec.h`.' : '.'));
  lines.push('- **B (implementations)**: external symbols reported by `llvm-nm -g --defined-only` on `libmujoco.a`.');
  lines.push('- **C (exports)**: `A intersect B` after applying the exclusion rules, emitted as `_mjwf_*` wrappers.');
  lines.push('- **Hard gate**: `(A intersect B) - C = 0`, and exports must not include `mjv_/mjr_/mjui_/mjp_/mjc_` nor non-`_mjwf_*` symbols.');
  lines.push('');
  lines.push('## Totals');
  lines.push('- A: ' + countA);
  lines.push('- B: ' + countB);
  lines.push('- C: ' + finalNames.length);
  lines.push('- Prefix breakdown: mj: ' + countMj + ' / mju: ' + countMju + ' / mjs: ' + countMjs + ' / mjd: ' + countMjd);
  lines.push('');
  lines.push('## Excluded Items');
  if (specialExclusions.length === 0) {
    lines.push('- none');
  } else {
    for (const item of specialExclusions) {
      lines.push('- ' + item.name + ' -> ' + item.reason);
    }
  }
  lines.push('');
  lines.push('## Set Differences (for auditing)');
  lines.push('- A - B (declared but not implemented): ' + aMinusB.length);
  if (aMinusB.length) {
    lines.push(...sliceWithEllipsis(aMinusB));
  }
  lines.push('- B - A (implemented but not declared): ' + bMinusA.length);
  if (bMinusA.length) {
    lines.push(...sliceWithEllipsis(bMinusA));
  }
  lines.push('- (A intersect B) - C (should be zero): ' + abMinusC.length);
  if (abMinusC.length) {
    lines.push(...sliceWithEllipsis(abMinusC));
  }
  lines.push('');
  return lines.join('\n');
}

function main() {
  const opts = parseArgs(process.argv);
  const headerInfo = loadHeaderInfo(opts.namesJson);
  const implNames = loadImpl(opts.implJson);

  const intersection = Array.from(headerInfo.names)
    .filter((name) => implNames.has(name))
    .sort();

  const specialExclusions = [];
  const finalFunctions = [];

  for (const name of intersection) {
    const fn = headerInfo.map.get(name);
    if (!fn) continue;
    if (!isAllowedPrefix(name)) {
      specialExclusions.push({ name, reason: 'non_core_prefix' });
      continue;
    }
    const hasVariadicAlt = fn.isVariadic && fn.has_v_alternative && implNames.has(`${name}_v`);
    if (fn.isVariadic && !hasVariadicAlt) {
      specialExclusions.push({ name, reason: 'variadic_no_v' });
      continue;
    }
    const cloned = { ...fn, has_v_alternative_effective: hasVariadicAlt };
    finalFunctions.push(cloned);
  }

  const finalNames = finalFunctions.map((fn) => fn.name);
  const finalNameSet = new Set(finalNames);

  const excludedNameSet = new Set(specialExclusions.map((item) => item.name));
  const aMinusB = Array.from(headerInfo.names).filter((name) => !implNames.has(name)).sort();
  const bMinusA = Array.from(implNames).filter((name) => !headerInfo.names.has(name)).sort();
  const abMinusC = intersection
    .filter((name) => !excludedNameSet.has(name) && !finalNameSet.has(name))
    .sort();

  let countMj = 0;
  let countMju = 0;
  let countMjs = 0;
  let countMjd = 0;
  for (const name of finalNames) {
    if (name.startsWith('mju_')) {
      countMju += 1;
    } else if (name.startsWith('mjs_')) {
      countMjs += 1;
    } else if (name.startsWith('mjd_')) {
      countMjd += 1;
    } else if (name.startsWith('mj_')) {
      countMj += 1;
    }
  }

  const generatedAt = new Date().toISOString();

  ensureDirFor(opts.headerOut);
  ensureDirFor(opts.sourceOut);
  writeFileSync(opts.headerOut, generateHeader(finalFunctions));
  writeFileSync(opts.sourceOut, generateSource(finalFunctions));

  const exportsJson = {
    generatedAt,
    version: opts.version,
    count: finalNames.length,
    names: finalNames,
    required: finalNames.map((name) => `_mjwf_${name}`),
    optional: [],
    runtime_keep: [...RUNTIME_KEEP],
  };

  mkdirSync(opts.outDir, { recursive: true });
  writeFileSync(pathJoin(opts.outDir, `exports_${opts.version}.json`), JSON.stringify(exportsJson, null, 2));
  writeFileSync(pathJoin(opts.outDir, `exports_${opts.version}.lst`), emitLst(finalNames));
  writeFileSync(pathJoin(opts.outDir, `types_${opts.version}.d.ts`), emitDts(finalFunctions));

  mkdirSync(opts.abiDir, { recursive: true });
  writeFileSync(pathJoin(opts.abiDir, 'wrapper_exports.json'), JSON.stringify(exportsJson, null, 2));

  const reportMarkdown = buildReport({
    version: opts.version,
    generatedAt,
    hasMjspec: headerInfo.hasMjspec,
    countA: headerInfo.names.size,
    countB: implNames.size,
    finalNames,
    countMj,
    countMju,
    countMjs,
    countMjd,
    specialExclusions,
    aMinusB,
    bMinusA,
    abMinusC,
  });
  writeFileSync(pathJoin(opts.abiDir, 'exports_report.md'), reportMarkdown);

  if (process.env.EMIT_JSON === '1') {
    const reportJson = {
      version: opts.version,
      generatedAt,
      hasMjspec: headerInfo.hasMjspec,
      counts: {
        A: headerInfo.names.size,
        B: implNames.size,
        C: finalNames.length,
        mj: countMj,
        mju: countMju,
        mjs: countMjs,
        mjd: countMjd,
      },
      special_exclusions: specialExclusions,
      differences: {
        A_minus_B: aMinusB,
        B_minus_A: bMinusA,
        intersection_minus_C: abMinusC,
      },
      exports: finalNames,
    };
    writeFileSync(pathJoin(opts.abiDir, 'exports_report.json'), JSON.stringify(reportJson, null, 2));
  }

  console.log(`[gen-exports] version=${opts.version} names=${finalNames.length} special=${specialExclusions.length}`);
}

main();
