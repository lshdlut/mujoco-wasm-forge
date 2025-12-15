import { test, expect } from '@playwright/test';
import { readFileSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import load_mujoco from '../../dist/3.3.7/mujoco.js';

const wasmPath = fileURLToPath(new URL('../../dist/3.3.7/mujoco.wasm', import.meta.url));

function writeFileToFS(Module, hostPath, fsPath) {
  const data = readFileSync(hostPath);
  const dir = path.posix.dirname(fsPath);
  try {
    Module.FS.mkdir(dir);
  } catch (err) {
    if (err?.errno !== 17) throw err;
  }
  Module.FS.writeFile(fsPath, data, { canRead: true, canWrite: true });
}

function makeHandleFromXml(Module, fsXmlPath) {
  if (typeof Module.ccall === 'function') {
    return (
      Module.ccall('mjwf_helper_make_from_xml', 'number', ['string'], [fsXmlPath]) |
      0
    );
  }
  if (typeof Module._mjwf_helper_make_from_xml === 'function') {
    const bytes = Module.lengthBytesUTF8(fsXmlPath) + 1;
    const ptr = Module._malloc(bytes);
    Module.stringToUTF8(fsXmlPath, ptr, bytes);
    try {
      return Module._mjwf_helper_make_from_xml(ptr) | 0;
    } finally {
      Module._free(ptr);
    }
  }
  throw new Error('No mjwf_helper_make_from_xml entrypoint found on Module');
}

test.describe('mjwf mjvScene geoms SoA exports (Node)', () => {
  test('mjwf_scene_update_and_pack produces readable attribute buffers', async () => {
    const Module = await load_mujoco({
      locateFile: (p) => (p.endsWith('.wasm') ? wasmPath : p),
    });
    if (Module.ready) await Module.ready;

    expect(typeof Module._mjwf_scene_update_and_pack).toBe('function');
    expect(typeof Module._mjwf_scene_ngeom).toBe('function');
    expect(typeof Module._mjwf_scene_geoms_pos_ptr).toBe('function');
    expect(typeof Module._mjwf_scene_geoms_mat_ptr).toBe('function');
    expect(typeof Module._mjwf_scene_geoms_rgba_ptr).toBe('function');
    expect(typeof Module._mjwf_scene_geoms_type_ptr).toBe('function');

    const dir = path.dirname(fileURLToPath(import.meta.url));
    const hostXml = path.join(dir, 'tmp_model.xml');
    const fsXml = '/models/tmp_model.xml';
    writeFileToFS(Module, hostXml, fsXml);

    const handle = makeHandleFromXml(Module, fsXml);
    expect(handle).toBeGreaterThan(0);

    try {
      const m = Module._mjwf_helper_model_ptr(handle);
      const d = Module._mjwf_helper_data_ptr(handle);
      expect(m).not.toBe(0);
      expect(d).not.toBe(0);

      for (let i = 0; i < 3; ++i) {
        Module._mjwf_mj_step(m, d);
      }

      const catmask = 7; // mjCAT_ALL
      const nPacked = Module._mjwf_scene_update_and_pack(handle, catmask) | 0;
      expect(nPacked).toBeGreaterThan(0);

      const ngeom = Module._mjwf_scene_ngeom(handle) | 0;
      expect(ngeom).toBe(nPacked);

      const typePtr = Module._mjwf_scene_geoms_type_ptr(handle) | 0;
      const posPtr = Module._mjwf_scene_geoms_pos_ptr(handle) | 0;
      const matPtr = Module._mjwf_scene_geoms_mat_ptr(handle) | 0;
      const rgbaPtr = Module._mjwf_scene_geoms_rgba_ptr(handle) | 0;

      expect(typePtr).toBeGreaterThan(0);
      expect(posPtr).toBeGreaterThan(0);
      expect(matPtr).toBeGreaterThan(0);
      expect(rgbaPtr).toBeGreaterThan(0);

      const type = Module.HEAP32.subarray(typePtr >> 2, (typePtr >> 2) + ngeom);
      const pos = Module.HEAPF32.subarray(posPtr >> 2, (posPtr >> 2) + ngeom * 3);
      const mat = Module.HEAPF32.subarray(matPtr >> 2, (matPtr >> 2) + ngeom * 9);
      const rgba = Module.HEAPF32.subarray(
        rgbaPtr >> 2,
        (rgbaPtr >> 2) + ngeom * 4,
      );

      expect(type.length).toBe(ngeom);
      expect(pos.length).toBe(ngeom * 3);
      expect(mat.length).toBe(ngeom * 9);
      expect(rgba.length).toBe(ngeom * 4);

      // A pendulum capsule should appear in the scene (mjGEOM_CAPSULE = 3).
      expect(type.some((v) => v === 3)).toBeTruthy();

      // Basic sanity on a few values: finite numbers and normalized alpha.
      for (const v of pos.slice(0, Math.min(12, pos.length))) {
        expect(Number.isFinite(v)).toBeTruthy();
      }
      for (const v of mat.slice(0, Math.min(18, mat.length))) {
        expect(Number.isFinite(v)).toBeTruthy();
      }
      for (let i = 0; i < Math.min(4, ngeom); ++i) {
        const a = rgba[i * 4 + 3];
        expect(a).toBeGreaterThanOrEqual(0);
        expect(a).toBeLessThanOrEqual(1);
      }
    } finally {
      if (typeof Module._mjwf_helper_free === 'function') {
        Module._mjwf_helper_free(handle);
      }
    }
  });
});
