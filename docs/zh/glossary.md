# 术语表

| 术语 | 在本项目中的含义 |
| --- | --- |
| ABI | WASM 构建对外暴露的可调用接口面：导出符号 + wrapper 约定。 |
| `dist/<ver>` | 某个 MuJoCo 版本/ref 的构建产物目录。 |
| `exports.lst` | 唯一导出清单（Emscripten：`-sEXPORTED_FUNCTIONS=@...`）。 |
| wrapper | 自动生成的 `mjwf_*` 封装函数，用于形成稳定且可审计的接口面。 |
| introspection | 从 `mujoco.h` 自动提取函数/结构体/枚举信息（含 AST）。 |
| gate | 门控检查：发现导出/ABI 漂移或质量回退（体积/初始化耗时）。 |
| smoke test | 最小运行时验证：编译并 step 一个小模型以捕获大回归。 |

