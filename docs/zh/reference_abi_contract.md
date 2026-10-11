# ABI 契约（exports & wrappers）

`mujoco-wasm-forge` 把导出符号视为显式契约来管理。

## 契约是什么

- 绝对权威的导出清单是 `dist/<ver>/abi/exports.lst`。
- WASM 构建会通过 Emscripten `-sEXPORTED_FUNCTIONS=@.../exports.lst` 强制使用它。

因此，“导出了什么”永远可以用文本 diff 直接审阅。

## A ∩ B = C（扁平导出）

流水线会把三类集合显式化，便于审计：

- **A**：来自 headers 的声明（introspection）。
- **B**：来自实现侧的符号清单（`nm_symbols.json`）。
- **C**：来自 wrappers 的导出集合（通常以 `mjwf_*` 为主）。

`abi_exports/` 下的生成器会把这些输入合并，产出：

- wrapper 源码（`mjwf_abi_funcs.*`、`mjwf_abi_structs.*`）；
- wrapper 导出 manifests（用于审计）；
- `exports.lst`（用于链接）。

## JS/Runtime 接口面

主要产物是 Emscripten 的 `mujoco.js`/`.wasm`。运行时入口（如 `ccall`/`cwrap`）由 `app/CMakeLists.txt`
里的链接参数导出。

## JS/Runtime 行为

维度函数继续保留 signed i32 / JavaScript Number 返回值。超出范围的值返回 `-1` 并给出诊断；
声明为 `mjtSize` 的维度另提供 `<name>_i64`，返回精确的 JavaScript BigInt。直接的 `mjtSize*`
视图使用 64 位 `BigInt64Array`，`mjtBool*` 视图使用字节型 `Uint8Array`。
`mju_warning`/`mju_error`/`mju_info` 的 wrapper 接收 literal text（包括 `%`），而不是 C varargs format string。

嵌套 struct 的 field pointer 直接指向 MuJoCo memory。对于 struct 数组（例如 `mjData.contact`），
pointer 指向第一个元素的 field，后续元素按 C struct stride 排列，并不是只包含该 field 的连续数组。
`scene_geoms_*` exports 另提供 packed SoA buffers。

## Typed calls 与 additive POD adapters

`abi/js_signature_capabilities.json` 将 raw function signature 标为 scalar、pointer、i64、aggregate、
callback、opaque 或 unknown。导出 symbol 不等于 JavaScript object marshalling；pointer call 需要有效且
对齐的 WASM memory 与 native lifetime，callback 和 opaque 生命周期 API 仍需 bridge 或 policy review。

Raw WASM i32 return 即使对应 unsigned C type 也会成为 signed JavaScript Number。生成的 type policy 对
u32（包括 wasm32 `size_t`）使用 `rawResult >>> 0`，对 u64 使用 `BigInt.asUintN(64, rawResult)`；
raw exports 本身不变，由 caller 做结果归一化。

只有完整 introspected fields 已知且由 scalar、fixed array 或 nested POD record 构成的 record 才会获得
类型驱动的 adapter。aggregate input 使用 additive `_ptr` 后缀，aggregate output 使用 `_out`（需要时为
`_ptr_out`）；已有 raw symbol 和 signature 不变。caller 分配对齐的 native record memory 并传入地址，
这不是 JavaScript SDK。生成的 `sizeof`、`alignof` 与 field-offset helper 只覆盖这些 adapter 使用的 record；
最新 camera 与 logging-record adapter 由 `check/tests/aggregate-abi.mjs` 执行 smoke。

i64/u64 argument 需要 JavaScript BigInt。raw u64 return 使用带符号的 WASM i64 bit pattern，必须用
`BigInt.asUintN(64, result)` 解读；signature metadata 会记录这个归一化，过程中不会转成 Number 或窄化。

现有 AoS-derived field pointer 另提供 additive `<pointer_name>_stride`，返回 native record 的 byte stride。
record type 与 symbol 记录在 `abi/aos_record_strides.json`；这些 helper 不会把 field view 变成 packed array。
aggregate smoke 会用该 stride 读取两个实际 contact。

## 审计证据与可复现性

每个 flavor 都需要 `abi/nm_coverage.single.json` 或 `abi/nm_coverage.pthreads.json`。其中的 schema、flavor、
成功扫描状态与对应 WASM 的 SHA256 必须匹配；共享的旧 `nm_coverage.json` alias 必须与选定 flavor report 完全相同。
缺失、失败、不匹配或过期的 evidence 会使 `verify-dist` 失败；缺少 static archive 也会在独立 post-build audit 中失败。

`abi/build_metadata.json` 记录 upstream Git SHA、实际 Emscripten compiler version、各 flavor 设置和 WASM hash。
每个 flavor 还记录 dependency 是否 dirty 以及 newline-normalized tracked patch hash；upstream SHA 不代表 source
input 一定 pristine。该 receipt 不包含 host path 或 timestamp；缺少 receipt 的历史 artifact 在 release packaging
中保留明确的 unknown provenance。

可复现 recipe 用 `-ffile-prefix-map` 映射 C/C++ source 与 dependency root。由于 engine 的 structured logging
会保留 `__FILE__` string，WASM 中使用可读的 repository-relative location，而不是 host checkout path；这是编译期
path canonicalization，不是 binary post-processing 或放宽比较。patch hash 使用 Git `--full-index`，避免仓库大小
相关的 abbreviated object ID 改变 receipt；独立的双检出 fixture 会验证 WASM 字节一致以及 source location 可读。

历史 deliverable 保留原 generator recipe；新的 baseline 应与相应 generator 和 pipeline change 一起提交，不能用
本地 runtime check 代替 strict fresh Linux comparison。
