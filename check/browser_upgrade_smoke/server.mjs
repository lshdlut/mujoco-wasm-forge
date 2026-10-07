import fs from 'node:fs/promises';
import http from 'node:http';
import path from 'node:path';

const root = path.resolve(process.argv[2]);
const port = Number(process.argv[3] || 4173);
const mime = new Map([
  ['.html', 'text/html; charset=utf-8'],
  ['.js', 'text/javascript; charset=utf-8'],
  ['.mjs', 'text/javascript; charset=utf-8'],
  ['.wasm', 'application/wasm'],
  ['.json', 'application/json; charset=utf-8'],
]);

function headersFor(urlPath, referer = '') {
  const pthreads = urlPath === '/pthreads' || urlPath.startsWith('/artifact/pthreads/') || referer.includes('/pthreads');
  const headers = {
    'Cache-Control': 'no-store',
    'Cross-Origin-Resource-Policy': 'same-origin',
  };
  if (pthreads) {
    headers['Cross-Origin-Opener-Policy'] = 'same-origin';
    headers['Cross-Origin-Embedder-Policy'] = 'require-corp';
  }
  return headers;
}

const server = http.createServer(async (request, response) => {
  try {
    const requestUrl = new URL(request.url, 'http://127.0.0.1');
    if (requestUrl.pathname === '/favicon.ico') {
      response.writeHead(204, { 'Cache-Control': 'no-store' });
      response.end();
      return;
    }
    let relative = decodeURIComponent(requestUrl.pathname);
    if (relative === '/' || relative === '/single' || relative === '/pthreads') relative = '/index.html';
    const filePath = path.resolve(root, `.${relative}`);
    if (filePath !== root && !filePath.startsWith(`${root}${path.sep}`)) {
      response.writeHead(400);
      response.end('bad path');
      return;
    }
    const body = await fs.readFile(filePath);
    const type = mime.get(path.extname(filePath)) || 'application/octet-stream';
    response.writeHead(200, { ...headersFor(requestUrl.pathname, request.headers.referer || ''), 'Content-Type': type });
    response.end(body);
  } catch (error) {
    process.stderr.write(`[browser-smoke-server] ${error?.stack || error}\n`);
    const status = error?.code === 'ENOENT' ? 404 : 500;
    response.writeHead(status, { 'Cache-Control': 'no-store' });
    response.end(`${status} ${error?.message || error}`);
  }
});

server.listen(port, '127.0.0.1', () => {
  process.stdout.write(`browser smoke server ${process.pid} root=${root} port=${port}\n`);
});

process.on('SIGTERM', () => server.close(() => process.exit(0)));
process.on('SIGINT', () => server.close(() => process.exit(0)));
