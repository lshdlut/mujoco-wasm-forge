"""
Struct export report for official_app_337.

包含两块对齐检查：
  1) baseline vs current：
     - baseline: local_tools/snapshots/337/before/mjwf_exports_generated.c
     - current:  wrappers/official_app_337/src/mjwf_exports_generated.c
     - 对比所有 `mjwf_*_ptr` 符号，列出：
         - baseline 有但 current 没有的（不允许默默丢字段/derived/dims）
         - current 新增的（候选“补漏”导出，需人工确认）

  2) introspect vs current：
     - 使用 external/mujoco introspect 转出来的
       dist/<ver>/abi/structs_introspect_like.json 作为“官方真相”。
     - 只针对 mjModel / mjData 的 pointer 字段做覆盖检查，不对字段名做任何
       前缀或黑名单过滤：凡是官方认为是 pointer 的字段，都要求我们至少有
       一个对应的 `mjwf_*_ptr` 导出。

实现思路（structs 路径）：
  - baseline/current：
      * 从两份 mjwf_exports_generated.c 中抽取所有 `mjwf_*_ptr` 基名集合，做 set diff。
  - introspect：
      * 读取 structs_introspect_like.json 的 mjModel/mjData pointer 字段列表；
      * 用 gen_exports.default_pointer_name 生成预期导出名，对 current 集合检查缺失；
      * 同时统计那些 current 的 pointer 名并非来自直接字段（派生视图、dims 等）。
"""

from __future__ import annotations

import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Set, Tuple

import sys


def _repo_root() -> Path:
  return Path(__file__).resolve().parents[2]


repo_root = _repo_root()
if str(repo_root) not in sys.path:
  sys.path.insert(0, str(repo_root))

from wrappers.official_app_337.codegen.gen_exports import default_pointer_name  # type: ignore


@dataclass
class PointerField:
  struct: str
  field: str
  type_kind: str
  inner_kind: str
  inner_name: str | None


def load_introspect_structs(path: Path) -> Dict[str, List[PointerField]]:
  data = json.loads(path.read_text(encoding="utf-8"))
  structs = data.get("structs", {})
  out: Dict[str, List[PointerField]] = {}
  for struct_name, s in structs.items():
    fields = []
    for f in s.get("fields", []):
      t = f.get("type") or {}
      kind = t.get("kind")
      if kind != "PointerType":
        continue
      inner = t.get("inner") or {}
      fields.append(
        PointerField(
          struct=struct_name,
          field=f.get("name") or "",
          type_kind=kind,
          inner_kind=inner.get("kind") or "",
          inner_name=inner.get("name"),
        )
      )
    out[struct_name] = fields
  return out


def load_pointer_exports(exports_c: Path) -> Set[str]:
  src = exports_c.read_text(encoding="utf-8")
  names = set(re.findall(r"\bmjwf_([A-Za-z0-9_]+)_ptr\b", src))
  return names


def main() -> None:
  repo_root = Path(__file__).resolve().parents[2]
  abi_dir = repo_root / "dist" / "3.3.7" / "abi"
  structs_json = abi_dir / "structs_introspect_like.json"
  current_exports_c = (
    repo_root / "wrappers" / "official_app_337" / "src" / "mjwf_exports_generated.c"
  )
  baseline_exports_c = (
    repo_root
    / "local_tools"
    / "snapshots"
    / "337"
    / "before"
    / "mjwf_exports_generated.c"
  )

  current_ptr_names = load_pointer_exports(current_exports_c)
  baseline_ptr_names: Set[str] = set()
  if baseline_exports_c.is_file():
    baseline_ptr_names = load_pointer_exports(baseline_exports_c)

  # 1) baseline vs current
  print("=== [1] Baseline vs current pointer exports (mjwf_*_ptr base names) ===")
  if baseline_ptr_names:
    missing_in_current = sorted(baseline_ptr_names - current_ptr_names)
    added_in_current = sorted(current_ptr_names - baseline_ptr_names)

    print("\n-- 1.1 baseline 有但 current 缺失 (should be empty) --")
    if not missing_in_current:
      print("  (none)")
    else:
      for name in missing_in_current:
        print(f"  {name}_ptr")

    print("\n-- 1.2 current 相比 baseline 新增 (候选补漏导出) --")
    if not added_in_current:
      print("  (none)")
    else:
      for name in added_in_current:
        print(f"  {name}_ptr")
  else:
    print("  baseline file not found; skipped baseline comparison.")

  # 2) introspect vs current
  structs = load_introspect_structs(structs_json)
  export_names = current_ptr_names

  missing: List[Tuple[str, str, str]] = []
  extra_unmatched: Set[str] = set(export_names)

  for struct_name in ("mjModel", "mjData"):
    for pf in structs.get(struct_name, []):
      if not pf.field:
        continue
      expected_base = default_pointer_name(struct_name, pf.field)
      expected_symbol = f"mjwf_{expected_base}_ptr"
      if expected_base in extra_unmatched:
        extra_unmatched.remove(expected_base)
      if expected_base not in export_names:
        missing.append((struct_name, pf.field, expected_symbol))

  print("\n=== [2] Introspect vs current (mjModel/mjData pointer fields) ===")
  print("\n-- 2.1 官方 pointer 字段缺失的导出 (struct.field -> symbol) --")
  if not missing:
    print("  (none)")
  else:
    for struct_name, field, sym in missing:
      print(f"  {struct_name}.{field} -> {sym}")

  print("\n-- 2.2 current 中无法直接映射到 mjModel/mjData 字段的 pointer 名 (derived/dims/count 等候选) --")
  if not extra_unmatched:
    print("  (none)")
  else:
    for name in sorted(extra_unmatched):
      print(f"  {name}_ptr")


if __name__ == "__main__":
  main()
