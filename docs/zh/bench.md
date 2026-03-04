# Bench：在“可定制/可审计”的前提下验证性能边界

本章介绍 `mujoco-wasm-forge` 随仓库提供的公开 bench（基准测试）套件。它存在的意义是回答一个工程化问题：

> 在满足 forge 的核心设计目标（**flexibility**、**flatness**、**rule-based auditable generation**）的前提下，
> 运行时性能是否保持在官方 MuJoCo embind WASM build 的同一数量级？当出现差异时，原因是否可解释、可控？

这里的“核心设计目标”包括但不限于：

- **Flexibility**：最小接口面（minimal interface）+ 多发行变体（single / pthreads 等），便于不同部署约束下取舍。
- **Flatness**：flat handles + 可预测的 ABI 组织，便于上层应用（Worker / Node / Web）与工具链构建。
- **Rule-based & auditable**：wrapper/exports 的生成与门控是规则驱动、可回归、可审计的。

这里的 **Flexibility** 不是在否定官方 embind 的价值：官方 embind 往往更“开箱即用”、绑定更完整、对下游更稳定易用。
与此同时，更丰富的绑定与更标准化的分发形态通常意味着更大的接口面与更固定的默认策略；当需求是做发行变体、控制导出 ABI、以及把升级差异纳入可审计/可回归的规则体系时，定制入口往往更少、更间接。forge 的选择是通过 **扁平化的 ABI 组织** 与 **自动化输出结构**（稳定的 dist layout + 可审计的 ABI artifacts + rules/gates），把接口迭代尽量收敛为“改规则 → 重生成 → 门控验证”，
从而在可审计前提下获得更强的可定制性。

我们希望 forge 既能覆盖 **极端部署约束**（例如线上 demo 的安全策略与资源预算），也能覆盖 **研究原型迭代**，
同时仍然足以作为日常模拟工作的基础组件。

这不是“打榜”。我们不会刻意强调 forge 一定优于 official，也不会回避两者在目标与取舍上的差异；bench 的目的是把这些取舍用可复现的数据与门控标准表达清楚。

## 我们测什么（按使用场景组织）

| 使用场景/需求 | 为什么要测 | 测什么（指标） | 备注 |
| --- | --- | --- | --- |
| 线上 demo / 文档页面要“轻” | 真实用户体验由下载/缓存决定 | 产物体积（`.js` + `.wasm`） | 从最终 bundle 读取字节数 |
| 首屏可交互要快 | 冷启动时延是硬约束 | Init（`modFactory() -> Module.ready`） | 常被线程初始化策略主导 |
| Simulate 的“第一次成功” | 用户最关心的是“第一步/第一帧” | TTFS（ready -> first step / first scene snapshot） | 浏览器 TTFS 是端到端；Node TTFS 更偏引擎侧 |
| 资源预算可控 | 线程池/编译阶段会出现内存尖峰 | RSS（after init + **peak**） | Node 侧更易稳定采样；浏览器更复杂 |
| “加载模型”要顺畅 | XML 编译/mesh 处理主导加载体验 | XML compile/load 延迟 | 对 threading default / 资产形态敏感 |
| 稳态 step 性能不退化 | 长时间运行时引擎热循环主导 | `ms/step` / steps/sec | 严谨对位需要匹配线程配置 |
| JS <-> WASM 交互成本可控 | flat API 的价值常体现在跨边界开销 | FFI 微基准（ns/call） | 微基准“小而精”，便于稳定复现 |
| reset/reload 不抖动 | demo 经常反复 load/free | reload 循环耗时 + RSS 漂移 | 更容易放大泄漏/绑定开销 |
| 线上失败可诊断 | 不能抛 raw wasm 异常/数字指针 | 错误可诊断性门控（bad XML / missing plugin / missing assets） | forge 提供显式 helper gates |
| 插件基线可用 | Simulate-style 模型可能依赖插件 | plugin baseline smoke（例如 `touch_grid`） | 属于产品契约层面的差异 |

## 对比矩阵（我们在对比什么）

| Label | 含义 | 线程策略 |
| --- | --- | --- |
| `official-3.4.0` | 官方 embind 基线 | 以上游构建产物为准 |
| `official-3.5.0-hc4` / `official-3.5.0-hc32` | 官方 embind（3.5.0）在不同“等效 pool sizing”下的表现 | pthreads 启用；用 `hardwareConcurrency` 近似 pool 行为 |
| `forge-3.4.0-single` | forge 基线 | single-thread |
| `forge-3.5.0-single` | forge 的单线程变体 | single-thread |
| `forge-3.5.0-pthreads` | forge 的 pthread 变体 | pthreads 启用；pool 默认 4（可用 `MJWF_PTHREAD_POOL_SIZE` 覆盖；3.5.0 默认 clamp=pool） |

“严谨对位”（thread-matched）场景下，我们主要对比：
`forge-3.5.0-pthreads（pool=4）` vs `official-3.5.0-hc4`。

## 结论摘要（参考运行）

bench 设计为可在不同机器上重复运行；数值会随硬件/负载变化。更重要的是回答“是否存在不可接受的退化”，以及差异是否可解释、可控。

| 维度 | 本次参考运行的观察（摘取关键数值） | 这意味着什么 | 注意事项 |
| --- | --- | --- | --- |
| 分发体积 | `forge-3.5.0-single`: wasm 3.33 MiB / JS 256.1 KiB；`official-3.5.0-hc4`: wasm 8.24 MiB / JS 296.7 KiB | minimal/flat surface 有机会降低分发成本 | 不代表未来上游变化后仍恒成立 |
| Init + 内存 | `forge-3.5.0-pthreads`: init 52.7ms / peak RSS 462.8 MiB；`official-3.5.0-hc4`: init 84.4ms / peak 659.2 MiB；`official-3.5.0-hc32`: RSS after init 507.9 MiB | pthread 默认策略与 pool sizing 往往决定成本曲线 | RSS 强依赖平台；浏览器与 Node 不一致 |
| 插件基线 | `sensor`/`touch_grid`: forge=ok，official=error | forge 可把插件可用性纳入 dist 契约 | official 的插件打包策略可能随上游演进 |
| 稳态 step（对位） | 例：`raj` 0.086 vs 0.103 ms/step；`cards` 0.325 vs 0.343（forge pthreads vs official hc4） | forge 的组织形式并不必然导致吞吐退化 | 小模型可能被测量开销/噪声主导 |
| FFI 微基准（对位） | `model_nq`: 14.71ns vs 33.56ns/call；`mj_version`: 4.23ns vs 5.24ns | flat handles 更利于降低高频小调用开销 | 微基准不等于全量应用表现 |
| TTFS（Simulate） | `humanoid` ready->snapshot：77ms（pthreads）vs 111ms（single）；`cards`：505ms（pthreads）vs 225ms（single） | pthreads 的端到端收益高度依赖模型与加载路径 | HUD 指标受环境/COI 配置影响 |
| reload/lifecycle（对位） | 50 次循环：0.840ms/iter（forge pthreads）vs 1.003ms/iter（official hc4）；RSS drift：+1.9 MiB vs +7.0 MiB | 生命周期压力下绑定/组织开销更容易被放大 | 不能替代所有真实 workload |

### 参考运行数据快照

参考运行环境（用于下面的快照表）：Windows x64 / Node `v22.16.0` / 32 logical CPUs。除特别说明外：
- Node 指标取 5 次运行的 median。
- 浏览器 HUD 指标默认 3 次运行取 median。
- 尺寸与内存以 KiB/MiB（1024 进制）表示。

#### Node：分发 / init / 内存

| Label | JS (KiB) | WASM (MiB) | Init (ms) | RSS after init (MiB) | RSS peak (MiB) |
| --- | --- | --- | --- | --- | --- |
| `forge-3.5.0-single` | 256.1 | 3.33 | 19.4 | 59.8 | 208.8 |
| `forge-3.5.0-pthreads` | 272.6 | 3.33 | 52.7 | 110.7 | 462.8 |
| `official-3.5.0-hc4` | 296.7 | 8.24 | 84.4 | 125.5 | 659.2 |
| `official-3.5.0-hc32` | 296.7 | 8.24 | 169.8 | 507.9 | 873.9 |

#### Node：模型加载与 step（严谨对位：`pool=4` vs `hc=4`）

| Model | Forge compile/load (ms) | Official compile/load (ms) | Forge steady `ms/step` | Official steady `ms/step` |
| --- | --- | --- | --- | --- |
| `cards` | 90.8 | 109.6 | 0.325 | 0.343 |
| `raj` | 303.4 | 384.3 | 0.086 | 0.103 |
| `humanoid` | 5.4 | 7.6 | 0.020 | 0.021 |
| `flex_bunny` | 59.5 | 102.5 | 0.426 | 0.465 |

#### Node：功能性基线（插件相关模型）

| Model | Forge 3.5.0 single | Official 3.5.0 (hc=4) |
| --- | --- | --- |
| `sensor` | ok | error |
| `touch_grid` | ok | error |

#### Node：FFI 微基准（apples-to-apples 子集）

> 这是 “1e6 次调用的微基准”，更适合用于对比组织形式带来的相对差异，而不应被解读为全量应用的绝对吞吐。

| Metric | Forge 3.5.0 pthreads ns/call (median) | Official 3.5.0 ns/call (median) |
| --- | --- | --- |
| `mj_version` | 4.23 | 5.24 |
| `model_nq` | 14.71 | 33.56 |

#### 浏览器：Simulate TTFS（forge 变体）

> 浏览器端通过 `mujoco-wasm-play` + Playwright 获取 HUD 指标；默认 3 次运行取 median。
> pthread 变体在部分环境下 HUD 的 `CPU (ms/step)` 可能不可用，但 TTFS/FPS 仍可采样。

| Model | Variant | TTFS ready->snapshot median (ms) | TTFS nav->snapshot median (ms) | FPS median |
| --- | --- | --- | --- | --- |
| `humanoid` | single | 111 | 322 | 120 |
| `humanoid` | pthreads | 77 | 287 | 120 |
| `cards` | single | 225 | 499 | 120 |
| `cards` | pthreads | 505 | 791 | 120 |
| `raj` | single | 338 | 570 | 121 |
| `raj` | pthreads | 270 | 510 | 45.5 |

#### Node：reload/lifecycle（严谨对位：`pool=4` vs `hc=4`）

| Label | Iterations | ms/iter | RSS drift (MiB) | RSS peak (MiB) |
| --- | --- | --- | --- | --- |
| `forge-3.5.0-pthreads` | 50 | 0.840 | 1.9 | 462.8 |
| `official-3.5.0-hc4` | 50 | 1.003 | 7.0 | 659.2 |

## 局限性（我们尽量规避，但仍可能存在）

- official embind 与 forge 的 JS API 形态不同：即使引擎版本可对齐，也难做到“逐指令同构”的对比。
- 浏览器 pthread 部署需要 COOP/COEP + `SharedArrayBuffer`：部署复杂度本身就是 trade-off 的一部分。
- 浏览器侧的内存采样比 Node 更困难：目前更可靠的内存指标集中在 Node。
- 目标不是“跑分最高”，而是“在 forge 的能力边界内，不出现不可接受的退化”。

## 如何复现

完整步骤与脚本入口见 `bench/README.md`。简版：

```powershell
powershell -ExecutionPolicy Bypass -File tools/build_official_embind.ps1 -Ref 3.4.0 -OutDir C:\dev\mjwf-bench\official\3.4.0 -Clean
powershell -ExecutionPolicy Bypass -File tools/build_official_embind.ps1 -Ref 3.5.0 -OutDir C:\dev\mjwf-bench\official\3.5.0
```

```bash
node bench/node/run_matrix.mjs --official340 C:/dev/mjwf-bench/official/3.4.0 --official350 C:/dev/mjwf-bench/official/3.5.0
node bench/node/report.mjs
```

```powershell
powershell -ExecutionPolicy Bypass -File bench/browser/run_playwright_bench.ps1
```
