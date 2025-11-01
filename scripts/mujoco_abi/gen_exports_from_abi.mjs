#!/usr/bin/env node

/**
 * Generate WASM export wrappers based on A∩B with automatic exclusions.
 *
 *   A = public C API declarations (mujoco.h [+ mjspec.h])
 *   B = implemented symbols (llvm-nm -g --defined-only libmujoco.a)
 *   C = A ∩ B after applying prefix + variadic rules
 */

import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { resolve as pathResolve, dirname, join as pathJoin } from 'node:path';

const ALLOWED_PREFIXES = [/^mj_/, /^mju_/, /^mjs_/];
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
    lines.push(`  - ... (共 ${list.length} 项)`);
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
    finalFunctions,
    countMj,
    countMju,
    countMjs,
    specialExclusions,
    aMinusB,
    bMinusA,
    abMinusC,
  } = opts;

  const lines = [];
  lines.push(`# MuJoCo WASM 导出报告 (ver ${version})`);
  lines.push(`生成时间：${generatedAt}`);
  lines.push('');
  lines.push('## 特殊排除规则');
  lines.push('- 仅导出以 `mj_`、`mju_`、`mjs_` 开头的函数；其他前缀（如 `mjv_`、`mjr_`、`mjui_`）全部排除。');
  lines.push('- 变参函数仅在存在 `*_v` 变体时导出；否则排除并记录为 `variadic_no_v`。');
  lines.push('- 导出集合恒等于 `C = A ∩ B`；不引入手写 helper。');
  lines.push('');
  lines.push('## A, B, C, 规则');
  lines.push(`- **A（声明集）**：扫描 \`mujoco.h\`${hasMjspec ? ' 与 `mjspec.h`' : ''} 的公开 C API 函数。`);
  lines.push('- **B（实现集）**：`llvm-nm -g --defined-only` 抽取 `libmujoco.a` 外部可见符号。');
  lines.push('- **C（导出集）**：`A ∩ B`，经“特殊排除”归一化后生成 `_mjwf_*` 包装。');
  lines.push('- **硬闸**：`(A ∩ B) − C = ∅`；导出不得含 `mjv_/mjr_/mjui_` 或非 `_mjwf_`。');
  lines.push('');
  lines.push('## 统计');
  lines.push(`- A: ${countA}`);
  lines.push(`- B: ${countB}`);
  lines.push(`- C: ${finalNames.length}`);
  lines.push(`- 其中 mj: ${countMj} / mju: ${countMju} / mjs: ${countMjs}`);
  lines.push('');
  lines.push('## 特殊排除规则的排除项');
  if (specialExclusions.length === 0) {
    lines.push('- 无');
  } else {
    for (const item of specialExclusions) {
      lines.push(`- ${item.name} — ${item.reason}`);
    }
  }
  lines.push('');
  lines.push('## ABC 差集（供审计）');
  lines.push(`- A − B（声明未实现）：${aMinusB.length}`);
  if (aMinusB.length) {
    lines.push(...sliceWithEllipsis(aMinusB));
  }
  lines.push(`- B − A（实现未公开）：${bMinusA.length}`);
  if (bMinusA.length) {
    lines.push(...sliceWithEllipsis(bMinusA));
  }
  lines.push(`- (A ∩ B) − C（应导未导，硬闸应为 0）：${abMinusC.length}`);
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

  const countMj = finalNames.filter((name) => name.startsWith('mj_')).length;
  const countMju = finalNames.filter((name) => name.startsWith('mju_')).length;
  const countMjs = finalNames.filter((name) => name.startsWith('mjs_')).length;

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
    finalFunctions,
    countMj,
    countMju,
    countMjs,
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
