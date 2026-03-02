# 添加新的 MuJoCo 版本

本页描述将新的 `dist/<ver>/` 加入仓库的常见流程。

## 步骤

1. 选择目标 ref（tag 或 commit hash）。
2. 构建：

   ```bash
   python3 forge_cli.py build --version <mjver> --with-checks
   ```

3. 审阅 ABI/导出差异：
   - `dist/<mjver>/abi/exports.lst`
   - `dist/<mjver>/abi/exports_report_funcs.md`
4. 若导出变化，判断是否可接受，并按需更新 wrappers/gates。
5. 提交新的 `dist/<mjver>/`（以及必要的流水线改动）。

## 小建议

- 若无特殊需求，优先构建上游 tag（例如 `3.5.0`）。
- 提交新 `dist/<ver>` 时，建议在 CI 使用 `verify-dist`。

