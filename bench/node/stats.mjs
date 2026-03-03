export function median(values) {
  const xs = values.filter((v) => Number.isFinite(v)).slice().sort((a, b) => a - b);
  if (!xs.length) return null;
  const mid = Math.floor(xs.length / 2);
  if (xs.length % 2) return xs[mid];
  return (xs[mid - 1] + xs[mid]) / 2;
}

export function percentile(values, p) {
  const xs = values.filter((v) => Number.isFinite(v)).slice().sort((a, b) => a - b);
  if (!xs.length) return null;
  const pp = Number(p);
  if (!Number.isFinite(pp) || pp < 0 || pp > 100) throw new Error(`Invalid percentile: ${p}`);
  const idx = (pp / 100) * (xs.length - 1);
  const lo = Math.floor(idx);
  const hi = Math.ceil(idx);
  if (lo === hi) return xs[lo];
  const t = idx - lo;
  return xs[lo] * (1 - t) + xs[hi] * t;
}

export function formatBytes(n) {
  const v = Number(n);
  if (!Number.isFinite(v) || v < 0) return 'n/a';
  const units = ['B', 'KB', 'MB', 'GB'];
  let x = v;
  let i = 0;
  while (x >= 1024 && i < units.length - 1) {
    x /= 1024;
    i += 1;
  }
  return i === 0 ? `${Math.round(x)} ${units[i]}` : `${x.toFixed(2)} ${units[i]}`;
}

export function formatMs(n, { digits = 2 } = {}) {
  const v = Number(n);
  if (!Number.isFinite(v)) return 'n/a';
  return `${v.toFixed(digits)} ms`;
}

export function formatNum(n, { digits = 2 } = {}) {
  const v = Number(n);
  if (!Number.isFinite(v)) return 'n/a';
  return v.toFixed(digits);
}

