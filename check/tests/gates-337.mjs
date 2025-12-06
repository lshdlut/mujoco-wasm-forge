// Quality gates: size thresholds and initialization time measurement
// Fails only when QUALITY_ENFORCE=1, otherwise logs warnings.

import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import fs from 'node:fs';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const rootDir = path.resolve(__dirname, '..');
const distDir = path.resolve(rootDir, 'dist', '3.3.7');

const wasmPath = path.join(distDir, 'mujoco.wasm');
const jsPath = path.join(distDir, 'mujoco.js');

const enforce = process.env.QUALITY_ENFORCE === '1';
const maxWasm = Number(process.env.WASM_MAX_BYTES || 16_000_000);
const maxJs = Number(process.env.JS_MAX_BYTES || 2_000_000);
const maxInitMs = Number(process.env.MAX_INIT_MS || 8000);

const statOrZero = (p) => { try { return fs.statSync(p).size; } catch { return 0; } };
const wasmSize = statOrZero(wasmPath);
const jsSize = statOrZero(jsPath);

if (!wasmSize || !jsSize) {
  throw new Error('dist artifacts missing for quality gates');
}

const modFactory = (await import(pathToFileURL(jsPath).href)).default;
const t0 = Date.now();
await modFactory({ locateFile: (p) => (p.endsWith('.wasm') ? wasmPath : p) });
const initMs = Date.now() - t0;

const breaches = [];
if (wasmSize > maxWasm) breaches.push(`wasm size ${wasmSize} > ${maxWasm}`);
if (jsSize > maxJs) breaches.push(`js size ${jsSize} > ${maxJs}`);
if (initMs > maxInitMs) breaches.push(`init ${initMs}ms > ${maxInitMs}ms`);

if (breaches.length) {
  const msg = `Quality gates breached: ${breaches.join(', ')}`;
  if (enforce) throw new Error(msg);
  console.warn(msg);
} else {
  console.log(`Quality gates OK: wasm=${wasmSize}B, js=${jsSize}B, init=${initMs}ms`);
}
