const params = new URL(location.href).searchParams;
const variant = params.get('variant') === 'pthreads' ? 'pthreads' : 'single';
const expectedVersion = params.get('version') || '';
const timeoutMs = 10_000;
const resultNode = document.querySelector('#result');
const pageErrors = [];
const workerErrors = [];

window.addEventListener('error', event => {
  pageErrors.push(`error: ${event.message || event.type}`);
});
window.addEventListener('unhandledrejection', event => {
  pageErrors.push(`unhandledrejection: ${String(event.reason)}`);
});

const artifactUrl = new URL(`/artifact/${variant}/mujoco.js`, location.href).href;
const worker = new Worker(new URL('./worker.mjs', import.meta.url), { type: 'module' });
let settled = false;
let timeoutId = null;

function render(value) {
  clearTimeout(timeoutId);
  resultNode.textContent = JSON.stringify(value, null, 2);
  document.title = `${value.status || 'UNKNOWN'} ${variant}`;
}

function finish(value) {
  if (settled) return;
  settled = true;
  worker.terminate();
  render({ ...value, pageErrors, workerErrors: [...workerErrors, ...(value.workerErrors || [])] });
}

timeoutId = setTimeout(() => {
  finish({
    status: 'FAIL',
    variant,
    error: `browser worker timeout after ${timeoutMs} ms`,
  });
}, timeoutMs + 2_000);

worker.addEventListener('error', event => {
  workerErrors.push({
    message: event.message || 'unknown worker error',
    filename: event.filename || null,
    lineno: event.lineno || null,
  });
  finish({
    status: 'FAIL',
    variant,
    error: `worker error: ${event.message || 'unknown worker error'}`,
  });
});

worker.addEventListener('message', event => {
  finish(event.data);
});

worker.postMessage({ variant, artifactUrl, timeoutMs, expectedVersion });
