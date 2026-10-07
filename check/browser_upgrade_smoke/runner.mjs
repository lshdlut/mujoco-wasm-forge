import crypto from 'node:crypto';
import fs from 'node:fs';
import * as fsp from 'node:fs/promises';
import net from 'node:net';
import os from 'node:os';
import path from 'node:path';
import { createRequire } from 'node:module';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { spawn } from 'node:child_process';

const SOURCE_ROOT = path.dirname(fileURLToPath(import.meta.url));
const MIN_BROWSER_VERSION = [3, 15, 0];
const VARIANTS = ['single', 'pthreads'];

function parseArgs(argv) {
  const defaultRuntime = process.env.MJWF_BROWSER_RUNTIME
    || path.join(os.tmpdir(), 'mujoco-wasm-forge-closeout-browser');
  const args = {
    distRoot: '',
    expectedVersion: '',
    runtimeRoot: defaultRuntime,
    output: '',
    port: 0,
    timeoutMs: 15_000,
    browserChannel: process.env.PLAYWRIGHT_CHANNEL || '',
    headed: false,
  };
  for (let i = 0; i < argv.length; i += 1) {
    const arg = argv[i];
    if (arg === '--dist-root') args.distRoot = argv[++i] ?? '';
    else if (arg === '--version') args.expectedVersion = argv[++i] ?? '';
    else if (arg === '--runtime-root') args.runtimeRoot = argv[++i] ?? '';
    else if (arg === '--output') args.output = argv[++i] ?? '';
    else if (arg === '--port') args.port = Number(argv[++i] ?? '0');
    else if (arg === '--timeout-ms') args.timeoutMs = Number(argv[++i] ?? '');
    else if (arg === '--browser-channel') args.browserChannel = argv[++i] ?? '';
    else if (arg === '--headed') args.headed = true;
    else throw new Error(`Unknown argument: ${arg}`);
  }
  if (!args.distRoot) throw new Error('--dist-root is required');
  if (!args.expectedVersion) throw new Error('--version is required');
  if (!Number.isInteger(args.port) || args.port < 0 || args.port > 65535) throw new Error(`Invalid --port: ${args.port}`);
  if (!Number.isFinite(args.timeoutMs) || args.timeoutMs < 1000) throw new Error(`Invalid --timeout-ms: ${args.timeoutMs}`);
  if (!args.output) args.output = path.join(args.runtimeRoot, 'browser-upgrade-result.json');
  return args;
}

function parseVersion(value) {
  const match = /^(\d+)\.(\d+)\.(\d+)(?:[-+].*)?$/.exec(value);
  if (!match) throw new Error(`Expected a semantic MuJoCo version, got: ${value}`);
  return [Number(match[1]), Number(match[2]), Number(match[3])];
}

function compareVersion(a, b) {
  for (let i = 0; i < 3; i += 1) {
    if (a[i] !== b[i]) return a[i] - b[i];
  }
  return 0;
}

function sha256File(filePath) {
  return crypto.createHash('sha256').update(fs.readFileSync(filePath)).digest('hex');
}

function artifactFiles(distRoot, variant) {
  const root = variant === 'pthreads' ? path.join(distRoot, 'pthreads') : distRoot;
  const js = path.join(root, 'mujoco.js');
  const wasm = path.join(root, 'mujoco.wasm');
  if (!fs.existsSync(js) || !fs.existsSync(wasm)) {
    throw new Error(`Missing ${variant} artifact pair under ${root}`);
  }
  return {
    js,
    wasm,
    aliases: variant === 'pthreads'
      ? ['mjwasm_forge.js', 'mjwasm_forge.wasm']
        .map(name => path.join(root, name))
        .filter(filePath => fs.existsSync(filePath))
      : [],
    jsSha256: sha256File(js),
    wasmSha256: sha256File(wasm),
  };
}

function reservePort() {
  return new Promise((resolve, reject) => {
    const server = net.createServer();
    server.once('error', reject);
    server.listen(0, '127.0.0.1', () => {
      const address = server.address();
      if (!address || typeof address === 'string') {
        server.close();
        reject(new Error('Could not determine an ephemeral loopback port'));
        return;
      }
      server.close(error => error ? reject(error) : resolve(address.port));
    });
  });
}

async function stageSite(distRoot, runtimeRoot) {
  await fsp.mkdir(runtimeRoot, { recursive: true });
  const siteRoot = await fsp.mkdtemp(path.join(path.resolve(runtimeRoot), 'site-'));
  await Promise.all(['index.html', 'main.mjs', 'worker.mjs'].map(name => (
    fsp.copyFile(path.join(SOURCE_ROOT, name), path.join(siteRoot, name))
  )));
  const artifacts = {};
  for (const variant of VARIANTS) {
    const info = artifactFiles(distRoot, variant);
    const targetDir = path.join(siteRoot, 'artifact', variant);
    await fsp.mkdir(targetDir, { recursive: true });
    await fsp.copyFile(info.js, path.join(targetDir, 'mujoco.js'));
    await fsp.copyFile(info.wasm, path.join(targetDir, 'mujoco.wasm'));
    await Promise.all(info.aliases.map(alias => fsp.copyFile(alias, path.join(targetDir, path.basename(alias)))));
    artifacts[variant] = { ...info, stagedRoot: targetDir };
  }
  return { siteRoot, artifacts };
}

async function waitForServer(child, url, timeoutMs) {
  const deadline = Date.now() + timeoutMs;
  let lastError = 'not yet probed';
  while (Date.now() < deadline) {
    if (child.exitCode !== null) {
      throw new Error(`owned server exited before readiness with code=${child.exitCode}`);
    }
    const probeTimeoutMs = Math.min(250, Math.max(1, deadline - Date.now()));
    const probeController = new AbortController();
    const probeTimer = setTimeout(() => probeController.abort(), probeTimeoutMs);
    try {
      const response = await fetch(`${url}/single`, { signal: probeController.signal });
      if (response.ok) return;
      lastError = `HTTP ${response.status}`;
    } catch (error) {
      lastError = String(error?.stack || error);
    } finally {
      clearTimeout(probeTimer);
    }
    await new Promise(resolve => setTimeout(resolve, 100));
  }
  throw new Error(`server readiness timeout after ${timeoutMs} ms: ${lastError}`);
}

function waitForChildExit(child, timeoutMs) {
  if (!child || child.exitCode !== null) return Promise.resolve();
  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => {
      reject(new Error(`owned server did not exit within ${timeoutMs} ms (pid=${child.pid})`));
    }, timeoutMs);
    child.once('exit', () => {
      clearTimeout(timer);
      resolve();
    });
  });
}

async function stopOwnedChild(child) {
  if (!child || child.exitCode !== null) return;
  child.kill('SIGTERM');
  try {
    await waitForChildExit(child, 5_000);
    return;
  } catch (gracefulError) {
    if (child.exitCode !== null) return;
    try {
      child.kill('SIGKILL');
      await waitForChildExit(child, 5_000);
    } catch (forcedError) {
      throw new AggregateError([gracefulError, forcedError], `failed to stop owned server pid=${child.pid}`);
    }
  }
}

async function startServer(siteRoot, requestedPort, timeoutMs) {
  const port = requestedPort || await reservePort();
  const serverScript = path.join(SOURCE_ROOT, 'server.mjs');
  const child = spawn(process.execPath, [serverScript, siteRoot, String(port)], {
    cwd: SOURCE_ROOT,
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  let stdout = '';
  let stderr = '';
  child.stdout.setEncoding('utf8');
  child.stderr.setEncoding('utf8');
  child.stdout.on('data', chunk => { stdout += chunk; });
  child.stderr.on('data', chunk => { stderr += chunk; });
  child.once('error', error => { stderr += `${error?.stack || error}\n`; });
  const baseUrl = `http://127.0.0.1:${port}`;
  const server = { child, baseUrl, port, getLogs: () => ({ stdout, stderr }) };
  try {
    await waitForServer(child, baseUrl, timeoutMs);
    return server;
  } catch (error) {
    try {
      await stopOwnedChild(child);
    } catch (cleanupError) {
      const details = `${errorRecord(error).stack || errorRecord(error).message}; `
        + `owned-child cleanup failed: ${cleanupError.stack || cleanupError.message || cleanupError}`;
      process.stderr.write(`[browser-upgrade-smoke] ${details}\n`);
      throw new Error(details, { cause: error });
    }
    const childStderr = stderr.trim();
    process.stderr.write(
      `[browser-upgrade-smoke] startup failed; owned server pid=${child.pid} cleaned`
      + (childStderr ? `; server stderr: ${childStderr}` : '')
      + '\n',
    );
    throw error;
  }
}

async function stopServer(server) {
  await stopOwnedChild(server?.child);
}

function loadPlaywright() {
  const moduleRoot = process.env.PLAYWRIGHT_MODULE_ROOT;
  const requireFrom = moduleRoot
    ? createRequire(path.join(path.resolve(moduleRoot), 'package.json'))
    : createRequire(import.meta.url);
  return requireFrom('playwright');
}

function errorRecord(error) {
  return { message: String(error?.message || error), stack: error?.stack || null };
}

function validateHarness(value, variant, expectedVersion) {
  const failures = [];
  const pthreads = variant === 'pthreads';
  if (value?.status !== 'PASS') failures.push(`harness status=${value?.status || 'missing'}`);
  if (value?.version !== expectedVersion) failures.push(`version=${value?.version} expected=${expectedVersion}`);
  if (value?.sharedMemory !== pthreads) failures.push(`sharedMemory=${value?.sharedMemory} expected=${pthreads}`);
  if (value?.crossOriginIsolated !== pthreads) failures.push(`crossOriginIsolated=${value?.crossOriginIsolated} expected=${pthreads}`);
  const dims = value?.dims;
  if (!dims || !Number.isInteger(dims.nq) || dims.nq <= 0) failures.push('invalid dims.nq');
  if (!dims || !Number.isInteger(dims.nv) || dims.nv <= 0) failures.push('invalid dims.nv');
  if (!dims || !Number.isInteger(dims.nu) || dims.nu < 0) failures.push('invalid dims.nu');
  if (!dims || !Number.isInteger(dims.nbody) || dims.nbody <= 0) failures.push('invalid dims.nbody');
  if (!dims || !Number.isInteger(dims.ngeom) || dims.ngeom <= 0) failures.push('invalid dims.ngeom');
  if (String(dims?.nqI64) !== String(dims?.nq)) failures.push('nq Number/BigInt mismatch');
  if (!Number.isInteger(dims?.nbuffer) || dims.nbuffer <= 0) failures.push('invalid dims.nbuffer');
  if (String(dims?.nbufferI64) !== String(dims?.nbuffer)) failures.push('nbuffer Number/BigInt mismatch');
  if (!Number.isFinite(value?.qpos)) failures.push(`qpos=${value?.qpos}`);
  if (pthreads && value?.meshCount !== 8) failures.push(`meshCount=${value?.meshCount} expected=8`);
  if (pthreads && value?.engineThreads !== 2) failures.push(`engineThreads=${value?.engineThreads} expected=2`);
  if (pthreads && value?.threadpoolTeardown !== true) failures.push('threadpool teardown was not asserted');
  if (!pthreads && value?.engineThreads !== 0) failures.push(`single engineThreads=${value?.engineThreads}`);
  return failures;
}

async function runVariant(browser, baseUrl, variant, expectedVersion, timeoutMs) {
  const pageErrors = [];
  const workerErrors = [];
  const consoleErrors = [];
  const networkErrors = [];
  const pageResponseHeaders = {};
  let page = null;
  try {
    page = await browser.newPage();
    page.on('pageerror', error => pageErrors.push(errorRecord(error)));
    page.on('console', message => {
      if (message.type() === 'error') consoleErrors.push({ type: message.type(), text: message.text() });
    });
    page.on('requestfailed', request => networkErrors.push({
      url: request.url(),
      method: request.method(),
      failure: request.failure(),
    }));
    page.on('response', response => {
      if (response.status() >= 400) networkErrors.push({
        url: response.url(),
        status: response.status(),
        statusText: response.statusText(),
      });
    });
    page.on('worker', worker => {
      worker.on('console', message => {
        if (message.type() === 'error') workerErrors.push({ type: message.type(), text: message.text() });
      });
    });

    const url = `${baseUrl}/${variant}?variant=${variant}&version=${encodeURIComponent(expectedVersion)}`;
    const response = await page.goto(url, { waitUntil: 'load', timeout: timeoutMs });
    Object.assign(pageResponseHeaders, response?.headers() || {});
    await page.waitForFunction(() => {
      const text = document.querySelector('#result')?.textContent || '';
      return text.length > 0 && text !== 'starting';
    }, undefined, { timeout: timeoutMs + 2_000 });
    const raw = await page.locator('#result').textContent();
    const harness = JSON.parse(raw || '{}');
    if (Array.isArray(harness.pageErrors)) pageErrors.push(...harness.pageErrors);
    if (Array.isArray(harness.workerErrors)) workerErrors.push(...harness.workerErrors);
    const failures = validateHarness(harness, variant, expectedVersion);
    const pthreads = variant === 'pthreads';
    const headerFailures = pthreads
      ? [
        pageResponseHeaders['cross-origin-opener-policy'] !== 'same-origin' ? 'missing COOP same-origin' : null,
        pageResponseHeaders['cross-origin-embedder-policy'] !== 'require-corp' ? 'missing COEP require-corp' : null,
      ].filter(Boolean)
      : [
        pageResponseHeaders['cross-origin-opener-policy'] ? 'ST unexpectedly has COOP' : null,
        pageResponseHeaders['cross-origin-embedder-policy'] ? 'ST unexpectedly has COEP' : null,
      ].filter(Boolean);
    const allFailures = [...failures, ...headerFailures];
    return {
      status: allFailures.length || pageErrors.length || workerErrors.length || consoleErrors.length || networkErrors.length ? 'FAIL' : 'PASS',
      variant,
      url,
      harness,
      checks: { failures, headerFailures },
      errors: { pageErrors, workerErrors, consoleErrors, networkErrors },
      pageResponseHeaders,
    };
  } catch (error) {
    const failure = errorRecord(error);
    process.stderr.write(`[browser-upgrade-smoke] ${variant} failed: ${failure.stack || failure.message}\n`);
    return {
      status: 'FAIL',
      variant,
      checks: { failures: [failure.message], headerFailures: [] },
      errors: { pageErrors, workerErrors, consoleErrors, networkErrors },
      error: failure,
      pageResponseHeaders,
    };
  } finally {
    if (page) await page.close();
  }
}

async function main() {
  const args = parseArgs(process.argv.slice(2));
  const parsedVersion = parseVersion(args.expectedVersion);
  if (compareVersion(parsedVersion, MIN_BROWSER_VERSION) < 0) {
    throw new Error(`browser upgrade smoke requires MuJoCo >= 3.15.0; use the Node checks for ${args.expectedVersion}`);
  }
  const distRoot = path.resolve(args.distRoot);
  const runtimeRoot = path.resolve(args.runtimeRoot);
  const outputPath = path.resolve(args.output);
  const staged = await stageSite(distRoot, runtimeRoot);
  let server = null;
  let browser = null;
  let outcome = null;
  const cleanupErrors = [];
  try {
    server = await startServer(staged.siteRoot, args.port, args.timeoutMs);
    const { chromium } = loadPlaywright();
    browser = await chromium.launch({
      headless: !args.headed,
      ...(args.browserChannel ? { channel: args.browserChannel } : {}),
    });
    const variants = {};
    for (const variant of VARIANTS) {
      variants[variant] = await runVariant(browser, server.baseUrl, variant, args.expectedVersion, args.timeoutMs);
    }
    const passed = VARIANTS.every(variant => variants[variant].status === 'PASS');
    outcome = {
      status: passed ? 'PASS' : 'FAIL',
      expectedVersion: args.expectedVersion,
      featureGate: {
        minimumVersion: '3.15.0',
        required: ['Number/BigInt scalar-width accessors', 'mjData boolean accessor stability', 'PT mju_threadpool(data, 2) and teardown'],
        olderVersions: 'browser gate skipped; Node checks remain authoritative',
      },
      distRoot,
      artifacts: staged.artifacts,
      server: { pid: server.child.pid, port: server.port, baseUrl: server.baseUrl },
      browser: { channel: args.browserChannel || 'default', version: browser.version() },
      variants,
      fixedErrorFields: ['pageErrors', 'workerErrors', 'consoleErrors', 'networkErrors'],
    };
  } finally {
    if (browser) {
      try { await browser.close(); } catch (error) {
        const record = errorRecord(error);
        cleanupErrors.push({ phase: 'browser.close', ...record });
        process.stderr.write(`[browser-upgrade-smoke] cleanup browser.close failed: ${record.stack || record.message}\n`);
      }
    }
    if (server) {
      try { await stopServer(server); } catch (error) {
        const record = errorRecord(error);
        cleanupErrors.push({ phase: 'server.stop', ...record });
        process.stderr.write(`[browser-upgrade-smoke] cleanup server.stop failed: ${record.stack || record.message}\n`);
      }
      const logs = server.getLogs();
      await fsp.writeFile(path.join(runtimeRoot, `server-${server.child.pid}.stdout.log`), logs.stdout, 'utf8');
      await fsp.writeFile(path.join(runtimeRoot, `server-${server.child.pid}.stderr.log`), logs.stderr, 'utf8');
    }
    const ownedRoot = `${runtimeRoot}${path.sep}`;
    if (staged.siteRoot.startsWith(ownedRoot)) {
      await fsp.rm(staged.siteRoot, { recursive: true, force: true });
    } else {
      cleanupErrors.push({ message: `refused to remove non-owned site root: ${staged.siteRoot}`, stack: null });
    }
  }
  if (!outcome) outcome = { status: 'FAIL', expectedVersion: args.expectedVersion, error: { message: 'runner did not produce an outcome', stack: null } };
  if (cleanupErrors.length) {
    outcome.status = 'FAIL';
    outcome.cleanupErrors = cleanupErrors;
  }
  await fsp.mkdir(path.dirname(outputPath), { recursive: true });
  await fsp.writeFile(outputPath, `${JSON.stringify(outcome, null, 2)}\n`, 'utf8');
  process.stdout.write(`${JSON.stringify(outcome, null, 2)}\n`);
  if (outcome.status !== 'PASS') process.exitCode = 1;
}

main().catch(async error => {
  const result = { status: 'FAIL', error: errorRecord(error) };
  process.stderr.write(`[browser-upgrade-smoke] fatal: ${result.error.stack || result.error.message}\n`);
  process.stdout.write(`${JSON.stringify(result, null, 2)}\n`);
  process.exitCode = 1;
});
