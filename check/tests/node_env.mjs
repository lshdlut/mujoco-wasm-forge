import os from "node:os";

export function ensureNodeEnv() {
  if (!globalThis.navigator) {
    globalThis.navigator = { hardwareConcurrency: os.cpus().length };
    return;
  }
  if (typeof globalThis.navigator.hardwareConcurrency !== "number") {
    globalThis.navigator.hardwareConcurrency = os.cpus().length;
  }
}

