# mujoco-wasm-forge（中文）

可复现的 MuJoCo → WebAssembly 构建流水线。本仓库专注于从指定的 MuJoCo 版本（tag）产出版本化的 WASM 工件，并配套最小化的冒烟/回归校验与可机器读取的元数据。

本仓库延续并致敬社区关于 MuJoCo→WASM 的探索工作，我们在此基础上整理为标准化流程：

- stillonearth: https://github.com/stillonearth/MuJoCo-WASM
- zalo: https://github.com/zalo/mujoco_wasm
- hashb: https://github.com/hashb/mujoco_web

仓库地址：https://github.com/lshdlut/mujoco-wasm-forge

## 工件（Artifacts）

一次成功构建（本地或 CI）后，将得到：

- `dist/mujoco-<mjVer>.wasm` — WebAssembly 二进制
- `dist/mujoco-<mjVer>.js` — ES 模块工厂（默认导出 `createMuJoCo`）
- `dist/mujoco-<mjVer>.wasm.map` — Source Map（可选）
- `dist/version.json` — 构建元数据（MuJoCo 版本、Emscripten 版本、体积、SHA256、上游 git sha 等）
- `dist/sbom.spdx.json` — 轻量 SPDX SBOM

示例 `version.json`：

```
{
  "mujocoVersion": "3.2.5",
  "emscripten": "3.1.55",
  "buildTime": "2025-..Z",
  "gitSha": "<upstream mujoco sha>",
  "size": {"wasmBytes": 0, "jsBytes": 0},
  "hash": {"wasmSha256": "...", "jsSha256": "..."}
}
```

## 快速开始（Node ESM）

```
import createMuJoCo from './mujoco-3.2.5.js';

const Module = await createMuJoCo({
  locateFile: (p) => (p.endsWith('.wasm') ? './mujoco-3.2.5.wasm' : p),
});

// 最小摆模型（MJCF/XML）
const xml = `<?xml version=\"1.0\"?>\n<mujoco model=\"pendulum\">\n  <option timestep=\"0.002\" gravity=\"0 0 -9.81\"/>\n  <worldbody>\n    <body name=\"link\" pos=\"0 0 0.1\">\n      <joint name=\"hinge\" type=\"hinge\" axis=\"0 1 0\" damping=\"0.01\"/>\n      <geom type=\"capsule\" fromto=\"0 0 0 0 0 0.2\" size=\"0.02\" density=\"1000\"/>\n    </body>\n  </worldbody>\n</mujoco>`;

// 首版为简化体量，仍启用 Emscripten FS 写入模型
Module.FS.writeFile('/model.xml', new TextEncoder().encode(xml));
const init = Module.cwrap('mjw_init','number',['string']);
const step = Module.cwrap('mjw_step_demo', null, ['number']);
const qpos0 = Module.cwrap('mjw_qpos0','number',[]);

if (init('/model.xml') !== 1) throw new Error('init failed');
const before = qpos0();
step(200);
const after = qpos0();
console.log({ before, after });
```

## CI 与可复现性

工作流 `forge-325`（GitHub Actions）：

- 固定 Emscripten（3.1.55）与 Node（20）
- 拉取上游 MuJoCo 指定 tag（3.2.5）
- 构建 WASM 模块与一个原生（native）对照可执行
- 运行冒烟测试（Node ESM）
- 运行回归测试：同一 XML，WASM 与原生对照 N 步
- 上传 `dist/` 目录，生成 `version.json` 与 `sbom.spdx.json`

说明：体积/初始化时间的质量门目前以“告警”方式运行（不阻塞首版）。

## 版本与 Tag 规范

- Release Tag 形如：`forge-<mujocoVersion>-r<rev>`，如 `forge-3.2.5-r1`
- 预发布使用：`forge-<mujocoVersion>-rc.<n>`，并标记为 `prerelease`
- 工件不可变；修复以新的修订号（如 `-r2`）发布

## 回归基线

- 基线：`native-3.2.5` ↔ `wasm-3.2.5`（最小摆模型）
- 确定性：固定步长、禁用随机、禁用 warmstart
- 阈值：尺寸/元数据严格一致；数值差在小的绝对误差阈值内（详见测试日志）

## 本地构建

前置：Emscripten SDK（3.1.55）、CMake、Node 20。

```
git clone https://github.com/google-deepmind/mujoco external/mujoco -b v3.2.5 --depth=1
emcmake cmake -S wrappers/official_app_325 -B build/325 -DCMAKE_BUILD_TYPE=Release \
  -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF
cmake --build build/325 -j
```

CI 会将工件整理到 `dist/`；本地构建可从 `build/325/_wasm/` 拷贝对应文件。

## 备注（Notes）

- 本仓库不包含前端演示页面；如需 Web UI，请参考维护者分支：https://github.com/lshdlut/mujoco_webfork
- 本地工具、缓存与实验脚本通过 `.gitignore` 统一忽略

## 溯源（Provenance）

本仓库部分配置、CI 工作流与文档由生成式 AI 协助撰写或改写，并由维护者进行人工审阅与验证。

