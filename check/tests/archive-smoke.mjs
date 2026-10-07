// ZIP fixture contains model.xml and its tetrahedral OBJ dependency.
import assert from 'node:assert/strict';
import path from 'node:path';
import { pathToFileURL } from 'node:url';
import { distDir, distVersion } from '../dist_paths.mjs';
import { ensureNodeEnv } from './node_env.mjs';

const version = distVersion();
if (Number(version.split('.')[1]) < 14) {
  console.log(`archive-smoke(${version}): NOT_APPLICABLE (archive provider introduced in 3.14)`);
} else {
  ensureNodeEnv();
  const root = distDir();
  const factory = (await import(pathToFileURL(path.join(root, 'mujoco.js')).href)).default;
  const module = await factory({ locateFile: p => path.join(root, p.endsWith('.wasm') ? 'mujoco.wasm' : p) });
  const zip = Buffer.from('UEsDBBQAAAAAAHd7R111lzDzlAAAAJQAAAAJAAAAbW9kZWwueG1sPG11am9jbz48YXNzZXQ+PG1lc2ggbmFtZT0ibSIgZmlsZT0ibWVzaGVzL3RldHJhLm9iaiIvPjwvYXNzZXQ+PHdvcmxkYm9keT48Ym9keT48ZnJlZWpvaW50Lz48Z2VvbSB0eXBlPSJtZXNoIiBtZXNoPSJtIi8+PC9ib2R5Pjwvd29ybGRib2R5PjwvbXVqb2NvPlBLAwQUAAAAAAB3e0ddGfGG0UAAAABAAAAAEAAAAG1lc2hlcy90ZXRyYS5vYmp2IDAgMCAwCnYgMSAwIDAKdiAwIDEgMAp2IDAgMCAxCmYgMSAzIDIKZiAxIDIgNApmIDEgNCAzCmYgMiAzIDQKUEsBAhQAFAAAAAAAd3tHXXWXMPOUAAAAlAAAAAkAAAAAAAAAAAAAAIABAAAAAG1vZGVsLnhtbFBLAQIUABQAAAAAAHd7R10Z8YbRQAAAAEAAAAAQAAAAAAAAAAAAAACAAbsAAABtZXNoZXMvdGV0cmEub2JqUEsFBgAAAAACAAIAdQAAACkBAAAAAA==', 'base64');
  module.FS.writeFile('/archive-smoke.mjz', zip);
  const parse = module.cwrap('mjwf_mj_parse', 'number', ['string', 'number', 'number', 'number', 'number']);
  const error = module._malloc(1024);
  const spec = parse('/archive-smoke.mjz', 0, 0, error, 1024);
  assert.ok(spec > 0, module.UTF8ToString(error));
  const model = module._mjwf_mj_compile(spec, 0);
  assert.ok(model > 0, 'nested OBJ archive dependency failed to compile');
  const data = module._mjwf_mj_makeData(model);
  assert.ok(data > 0);
  try {
    module._mjwf_mj_step(model, data);
  } finally {
    module._mjwf_mj_deleteData(data);
    module._mjwf_mj_deleteModel(model);
    module._mjwf_mj_deleteSpec(spec);
    module._free(error);
  }
  console.log(`archive-smoke(${version}): PASS (MJZ model and nested OBJ resource)`);
}
