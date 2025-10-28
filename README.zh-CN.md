# mujoco-wasm-forge

English | 简体中文

本仓库提供可复现的 MuJoCo → WebAssembly 构建产线。目标是在固定工具链与接口的前提下，从上游 MuJoCo 标签（3.2.5 / 3.3.7）产出版本化的 WASM 工件，并附最小的运行验证与机器可读元数据。

- 输入：MuJoCo 标签（3.2.5、3.3.7）
- 输出：`dist/mujoco-<version>.{js,wasm[,wasm.map]}`，`dist/version.json`，`dist/sbom.spdx.json`
- 工具链：固定 Emscripten；最小导出；不包含前端渲染 UI

## 快速开始（Node ESM）

```
import createMuJoCo from './mujoco-3.2.5.js';

const Module = await createMuJoCo({
  locateFile: (p) => (p.endsWith('.wasm') ? './mujoco-3.2.5.wasm' : p),
});

// 最小摆模型 XML
const xml = `<?xml version="1.0"?>\n<mujoco model="pendulum">\n  <option timestep="0.002" gravity="0 0 -9.81"/>\n  <worldbody>\n    <body name="link" pos="0 0 0.1">\n      <joint name="hinge" type="hinge" axis="0 1 0" damping="0.01"/>\n      <geom type="capsule" fromto="0 0 0 0 0 0.2" size="0.02" density="1000"/>\n    </body>\n  </worldbody>\n</mujoco>`;

Module.FS.writeFile('/model.xml', new TextEncoder().encode(xml));
// 句柄式接口统一前缀（mjwf_）
const init = Module.cwrap('mjwf_init','number',['string']);
const step = Module.cwrap('mjwf_step_demo', null, ['number']);
const qpos0 = Module.cwrap('mjwf_qpos0','number',[]);

if (init('/model.xml') !== 1) throw new Error('init failed');
const before = qpos0();
step(200);
const after = qpos0();
console.log({ before, after });
```

## 工作流与复现性

- 单一入口：`.github/workflows/forge.yml` 使用矩阵覆盖 3.2.5 与 3.3.7
- 固定工具链：emsdk 3.1.55、Node 20
- 两阶段配置（仅 3.3.7）：为在 Emscripten 下静态链接 qhull，先配置拉依赖，再对 qhull CMake 强制 STATIC/关闭 BUILD_SHARED_LIBS，最后二次配置
- 三道闸门（语义）：
  - [GATE:SYM] 从 JSON 校验导出符号完整性
  - [GATE:DTS] 由规格生成 d.ts，并与仓库版本对比，无漂移
  - [GATE:RUN] 运行时 SMOKE/回归/扩展 SMOKE（mesh）
- 说明：若某闸门尚未在脚本中实现，工作流以 `skipped` 日志标注，不改变默认行为
- 产物：上传 `dist/`（含 `version.json`、`sbom.spdx.json`）

## 本地构建

前置：Emscripten SDK (3.1.55)、CMake、Node 20。

```
git clone https://github.com/google-deepmind/mujoco external/mujoco -b v3.2.5 --depth=1
emcmake cmake -S wrappers/official_app_325 -B build/325 -DCMAKE_BUILD_TYPE=Release \
  -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF
cmake --build build/325 -j
```

工作流默认把工件放在 `dist/`；本地跑时可从 `build/<ver>/_wasm/` 拷贝。

## 元数据 schema（对齐方向）

- 文件：`dist/version.json`
- 字段（最小）：
  - `mujocoVersion`、`emscripten`、`buildTime`、`gitSha`
  - 当前工作流包含的 `features/size/hash` 区段
  - 后续对齐方向：补充 `emsdk_root`、`emsdk_node`、`emsdk_python` 与 `flags`（不改变当前行为）

