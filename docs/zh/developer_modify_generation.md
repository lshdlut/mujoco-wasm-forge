# 修改 introspection / wrapper 生成逻辑

这类改动通常会联动 `dist/<ver>/abi/*` 的产物，并可能影响最终导出符号集合。

## 关键触点

- header 扫描 / AST 提取：`introspect/`
- wrappers 与导出清单生成：`abi_exports/`
- 符号清单 / 覆盖率：`abi_impl/`
- 构建与链接 flags：`app/CMakeLists.txt`

## 推荐流程

1. 完成代码修改。
2. 用一个 canary 版本重建并跑 checks：

   ```bash
   python3 forge_cli.py build --version <mjver> --with-checks
   ```

3. 审阅 `dist/<ver>/abi/*` diffs，确认变化是有意的。
4. 如果契约变化，补齐 gates/tests。

