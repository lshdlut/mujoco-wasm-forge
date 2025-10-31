# mujoco-wasm-forge

English | Chinese

可复现的 MuJoCo → WebAssembly 构建流水线。本仓库聚焦：从 MuJoCo 标签产出版本化的 WASM 工件，并配套最小的 smoke/regression 验证与机器可读元数据。

- 输入：MuJoCo 标签（如 3.2.5、3.3.7）
- 输出：`dist/<version>/{mujoco.js,mujoco.wasm[,mujoco.wasm.map],version.json,sbom.spdx.json}`
- 工具链：固定 Emscripten；最小导出；不包含渲染 UI

状态：可用。CI 构建单模块 WASM + glue JS，运行 Node smoke + 原生对比回归，并上传工件。

仓库地址：https://github.com/lshdlut/mujoco-wasm-forge

## 工件（Artifacts）

CI（以及启用元数据的规范本地构建）会产出：

- `dist/<mjVer>/mujoco.wasm` - WebAssembly 二进制
- `dist/<mjVer>/mujoco.js` - ES 模块工厂（`createMuJoCo`）
- `dist/<mjVer>/mujoco.wasm.map` - 可选 source map
- `dist/<mjVer>/version.json` - 元数据（MuJoCo 版本、emscripten 版本、大小、sha256、git sha）
- `dist/<mjVer>/sbom.spdx.json` - SPDX SBOM（轻量）

## 快速开始（Node ESM）

```
import createMuJoCo from './dist/3.2.5/mujoco.js';

const Module = await createMuJoCo({
  locateFile: (p) => (p.endsWith('.wasm') ? './dist/3.2.5/mujoco.wasm' : p),
});

// 迷你摆模型 XML
const xml = `<?xml version="1.0"?>\n<mujoco model="pendulum">\n  <option timestep="0.002" gravity="0 0 -9.81"/>\n  <worldbody>\n    <body name="link" pos="0 0 0.1">\n      <joint name="hinge" type="hinge" axis="0 1 0" damping="0.01"/>\n      <geom type="capsule" fromto="0 0 0 0 0 0.2" size="0.02" density="1000"/>\n    </body>\n  </worldbody>\n</mujoco>`;

Module.FS.writeFile('/model.xml', new TextEncoder().encode(xml));
const init = Module.cwrap('mjwf_init','number',['string']);
const step = Module.cwrap('mjwf_step_demo', null, ['number']);
const qpos0 = Module.cwrap('mjwf_qpos0','number',[]);

if (init('/model.xml') !== 1) throw new Error('init failed');
const before = qpos0();
step(200);
const after = qpos0();
console.log({ before, after });
```

## CI 与可复现性

- 统一入口：`.github/workflows/forge.yml`（矩阵覆盖 3.2.5/3.3.7）
- 固定工具链：emsdk 3.1.55、Node 20
- 3.3.7 采用“两阶段配置”强制 qhull 静态化（SHARED->STATIC，BUILD_SHARED_LIBS=OFF）；3.2.5 不需要该补丁
- 质量闸：`[GATE:SYM]` 符号校验、`[GATE:DTS]` d.ts 漂移、`[GATE:RUN]` 运行时 smoke/regression/mesh-smoke
- 工件：上传 `dist/<mjVer>/`（含 `version.json`、`sbom.spdx.json`）

## 本地构建（规范）

推荐：WSL Ubuntu 22.04（或 Docker）完整复刻 CI 配方。

- Windows 侧（仓库根执行）：
  - 首次（镜像到 WSL 并构建）：
    - `pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File local_tools/wsl/run.ps1 -Sync -Clean -Meta -PinNode20 -UseTemp -Jobs 6`
  - 后续（已镜像）：
    - `pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File local_tools/wsl/run.ps1 -Clean -Meta -PinNode20 -Jobs 6`
  - 说明：`-WslWorkDir` 可指定 WSL 侧工作目录（默认 `~/dev/mujoco-wasm-forge`）

- WSL 侧（等效）：
  - `CLEAN=1 META=1 PIN_NODE20=1 TARGETS=325,337 MJVER_337=3.3.7 MJVER_325=3.2.5 bash ./local_tools/wsl/build.sh`

注意：
- 所有构建与测试都应在 WSL 的 ext4（例如 `~/dev/mujoco-wasm-forge`），或使用 `-UseTemp` 在 `/tmp` 下进行，避免 `/mnt/c/...` 和 OneDrive 带来的 I/O 与同步开销。不会在 Windows 侧创建 `~` 或写入 OneDrive。
- 默认并行度为 6，可用 `-Jobs` 覆盖。

## 在其它仓库使用工件（如 mujoco-wasm-play）

- 在本仓库 WSL 内构建完成后，工件位于 `dist/<mjVer>/`。
- 在 WSL 内仅复制需要的工件到 play 仓库：
  - `cp -r ./dist/3.3.7 /path/to/mujoco-wasm-play/dist/3.3.7`
- 在 play 仓库的加载器中从 `dist/<mjVer>/mujoco.{js,wasm}` 加载；不要复制 `build/` 或 `external/`。
- 3.3.7 需要两阶段配置以静态化 qhull；启用 `-Meta`/`META=1` 时会在 `dist/<mjVer>/` 生成与 CI 相同的元数据文件。

## 版本与标签

- 稳定发布：`forge-<mujocoVersion>-r<rev>`，如 `forge-3.2.5-r3`、`forge-3.3.7-r2`
- 预发布：`forge-<mujocoVersion>-rc.<n>`，并标记为 pre-release
- 工件不可变；修订以 `-rN` 形式发布

## 回归基线

- 基线：`native-3.2.5` ↔ `wasm-3.2.5`，`native-3.3.7` ↔ `wasm-3.3.7`
- 确定性：固定步长、无随机、关闭 warmstart

## 说明

- 前端演示（进行中）：https://github.com/lshdlut/mujoco-wasm-play.git
