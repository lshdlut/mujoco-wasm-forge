import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const repoRoot = path.resolve(__dirname, '..');
const distRoot = path.join(repoRoot, 'dist');

let cachedVersion = null;
let cachedVariant = null;

const SEMVER_RE = /^(\d+)\.(\d+)\.(\d+)(?:[-+].*)?$/;

function versionSortKey(version) {
  const match = SEMVER_RE.exec(version);
  if (!match) return [0, version];
  const major = Number(match[1]);
  const minor = Number(match[2]);
  const patch = Number(match[3]);
  const prerelease = version.includes('-') ? version.split('-', 2)[1] : '';
  const isRelease = prerelease ? 0 : 1;
  return [1, major, minor, patch, isRelease, prerelease];
}

function compareVersions(a, b) {
  const ka = versionSortKey(a);
  const kb = versionSortKey(b);
  for (let i = 0; i < Math.max(ka.length, kb.length); i++) {
    if (ka[i] < kb[i]) return -1;
    if (ka[i] > kb[i]) return 1;
  }
  return 0;
}

function listDistVersions() {
  if (!fs.existsSync(distRoot)) {
    return [];
  }
  return fs
    .readdirSync(distRoot, { withFileTypes: true })
    .filter((entry) => entry.isDirectory())
    .map((entry) => entry.name)
    .sort(compareVersions);
}

function resolveDistVersion() {
  if (cachedVersion) return cachedVersion;
  const envVersion = process.env.MJVER ?? process.env.DIST_VERSION ?? '';
  if (envVersion) {
    cachedVersion = envVersion;
    return cachedVersion;
  }

  const versions = listDistVersions();
  if (!versions.length) {
    throw new Error(
      'No dist/<ver> directory was found; run `python forge_cli.py build --version <ver>` (or `npm run build:forge -- --version <ver>`) first.'
    );
  }
  cachedVersion = versions[versions.length - 1];
  return cachedVersion;
}

function resolveDistVariant() {
  if (cachedVariant !== null) return cachedVariant;
  const raw = process.env.MJWF_DIST_VARIANT ?? process.env.DIST_VARIANT ?? '';
  cachedVariant = raw.trim();
  return cachedVariant;
}

function distDir(version, variant) {
  const ver = version ?? resolveDistVersion();
  const base = path.join(distRoot, ver);
  if (!fs.existsSync(base) || !fs.statSync(base).isDirectory()) {
    throw new Error(`Dist directory not found for version "${ver}" (expected at ${base}).`);
  }

  const v = (variant ?? resolveDistVariant()).trim();
  const dir = v ? path.join(base, v) : base;
  if (!fs.existsSync(dir) || !fs.statSync(dir).isDirectory()) {
    const suffix = v ? `/${v}` : '';
    throw new Error(`Dist directory not found for version "${ver}${suffix}" (expected at ${dir}).`);
  }
  return dir;
}

function resolveDistPath(...segments) {
  return path.join(distDir(), ...segments);
}

export { distRoot, resolveDistVersion as distVersion, distDir, resolveDistPath };
