#!/usr/bin/env python3
"""Architecture/maintainability QA for mujoco-wasm-forge.

This tool focuses on producing objective evidence (interfaces, structure,
coupling, redundancy, hotspots) and enforcing a small set of high-signal gates.

It intentionally uses only the Python standard library.
"""

from __future__ import annotations

import argparse
import ast
import hashlib
import json
import re
import subprocess
import sys
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Iterable, Mapping, Sequence


_WINDOW_LINES = 20
_MAX_TEXT_BYTES = 1_000_000

_EXCLUDED_PREFIXES = (
    "dist/",
    "introspect/official/",
)

_DOC_ENV_VARS_GATED_PREFIXES = ("MJWF_", "QUALITY_")
_DOC_ENV_VARS_GATED_EXACT = ("WASM_MAX_BYTES", "JS_MAX_BYTES", "MAX_INIT_MS")


def _dist_version_sort_key(repo_root: Path):
  sys.path.insert(0, str(repo_root))
  try:
    from dist_version import version_sort_key
  finally:
    sys.path.pop(0)
  return version_sort_key


def _now_iso() -> str:
  return datetime.now(timezone.utc).isoformat()


def _repo_root() -> Path:
  script_dir = Path(__file__).resolve().parent
  proc = subprocess.run(
      ["git", "rev-parse", "--show-toplevel"],
      cwd=str(script_dir),
      check=True,
      stdout=subprocess.PIPE,
      stderr=subprocess.PIPE,
      text=True,
  )
  return Path(proc.stdout.strip()).resolve()


def _git_ls_files(root: Path) -> list[str]:
  proc = subprocess.run(
      ["git", "-C", str(root), "ls-files", "-z"],
      check=True,
      stdout=subprocess.PIPE,
  )
  return [p for p in proc.stdout.decode("utf-8", errors="replace").split("\0") if p]


def _is_excluded(rel_posix: str) -> bool:
  return any(rel_posix.startswith(prefix) for prefix in _EXCLUDED_PREFIXES)


def _read_text_best_effort(path: Path) -> str | None:
  try:
    if path.stat().st_size > _MAX_TEXT_BYTES:
      return None
    return path.read_text(encoding="utf-8", errors="replace")
  except OSError:
    return None


def _extract_cmd_build_stages(forge_cli: Path) -> list[dict]:
  text = forge_cli.read_text(encoding="utf-8")
  tree = ast.parse(text, filename=str(forge_cli))

  func: ast.FunctionDef | None = None
  for node in tree.body:
    if isinstance(node, ast.FunctionDef) and node.name == "cmd_build":
      func = node
      break
  if func is None:
    return []

  def _call_name(call: ast.Call) -> str | None:
    if isinstance(call.func, ast.Name):
      return call.func.id
    return None

  def _is_with_checks_test(test: ast.AST) -> bool:
    return (
        isinstance(test, ast.Attribute)
        and isinstance(test.value, ast.Name)
        and test.value.id == "args"
        and test.attr == "with_checks"
    )

  stages: list[dict] = []

  def _add_stage(name: str, conditional: bool) -> None:
    if not name.startswith("_"):
      return
    stages.append({"name": name, "conditional": conditional})

  for stmt in func.body:
    if isinstance(stmt, ast.Expr) and isinstance(stmt.value, ast.Call):
      name = _call_name(stmt.value)
      if name:
        _add_stage(name, conditional=False)
      continue

    if isinstance(stmt, ast.If) and _is_with_checks_test(stmt.test):
      for nested in stmt.body:
        if isinstance(nested, ast.Expr) and isinstance(nested.value, ast.Call):
          name = _call_name(nested.value)
          if name:
            _add_stage(name, conditional=True)

  return stages


def _mermaid_pipeline(stages: Sequence[Mapping[str, object]]) -> str:
  if not stages:
    return "flowchart TD\n  A[cmd_build] --> B[no stages found]\n"

  lines = ["flowchart TD"]
  node_ids: list[str] = []
  for idx, stage in enumerate(stages):
    name = str(stage.get("name", ""))
    conditional = bool(stage.get("conditional", False))
    label = f"{name} (with_checks)" if conditional else name
    node_id = f"S{idx}"
    node_ids.append(node_id)
    safe = label.replace('"', "'")
    lines.append(f'  {node_id}["{safe}"]')
  for a, b in zip(node_ids, node_ids[1:]):
    lines.append(f"  {a} --> {b}")
  return "\n".join(lines) + "\n"


def _extract_cli_surface(repo_root: Path) -> dict:
  py = sys.executable or "python3"
  proc = subprocess.run(
      [py, "forge_cli.py", "--help"],
      cwd=str(repo_root),
      stdout=subprocess.PIPE,
      stderr=subprocess.STDOUT,
      text=True,
  )
  help_text = proc.stdout
  commands: list[str] = []
  m = re.search(r"\{([^}]+)\}", help_text)
  if m:
    commands = [part.strip() for part in m.group(1).split(",") if part.strip()]

  subcommand_help: dict[str, str] = {}
  for cmd in commands:
    sub = subprocess.run(
        [py, "forge_cli.py", cmd, "--help"],
        cwd=str(repo_root),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    subcommand_help[cmd] = sub.stdout

  return {
      "help": help_text,
      "commands": sorted(commands),
      "subcommand_help": subcommand_help,
      "exit_code": proc.returncode,
  }


def _extract_documented_cli_commands(doc_paths: Sequence[Path]) -> dict[str, set[str]]:
  per_doc: dict[str, set[str]] = {}
  for doc in doc_paths:
    text = _read_text_best_effort(doc)
    cmds: set[str] = set()
    if text:
      for line in text.splitlines():
        m = re.match(r"^##\s+`([^`]+)`\s*$", line.strip())
        if m:
          cmds.add(m.group(1))
    per_doc[doc.as_posix()] = cmds
  return per_doc


def _extract_documented_env_vars(doc_paths: Sequence[Path]) -> dict[str, set[str]]:
  per_doc: dict[str, set[str]] = {}
  token_re = re.compile(r"`([A-Z][A-Z0-9_]+)(?:=[^`]+)?`")
  for doc in doc_paths:
    text = _read_text_best_effort(doc)
    tokens: set[str] = set()
    if text:
      for line in text.splitlines():
        if line.lstrip().startswith("- "):
          tokens.update(token_re.findall(line))
    per_doc[doc.as_posix()] = tokens
  return per_doc


def _extract_env_vars_from_files(repo_root: Path, rel_paths: Sequence[str]) -> dict:
  py_re = re.compile(r"os\.(?:environ\.get|getenv)\(\s*['\"]([^'\"]+)['\"]")
  py_subscript_re = re.compile(r"os\.environ\[\s*['\"]([^'\"]+)['\"]\s*\]")
  js_attr_re = re.compile(r"\bprocess\.env\.([A-Z0-9_]+)\b")
  js_index_re = re.compile(r"\bprocess\.env\[\s*['\"]([^'\"]+)['\"]\s*\]")
  cmake_env_re = re.compile(r"ENV\{\s*([A-Za-z0-9_]+)\s*\}")

  matches: dict[str, list[dict]] = defaultdict(list)

  for rel in rel_paths:
    if _is_excluded(rel):
      continue
    path = repo_root / rel
    ext = path.suffix.lower()
    if ext not in (".py", ".mjs", ".js", ".cmake") and path.name != "CMakeLists.txt":
      continue
    text = _read_text_best_effort(path)
    if not text:
      continue

    def _add(var: str, kind: str, lineno: int) -> None:
      matches[var].append({"path": rel, "line": lineno, "kind": kind})

    if ext == ".py":
      for i, line in enumerate(text.splitlines(), start=1):
        for m in py_re.finditer(line):
          _add(m.group(1), "python", i)
        for m in py_subscript_re.finditer(line):
          _add(m.group(1), "python", i)
    elif ext in (".mjs", ".js"):
      for i, line in enumerate(text.splitlines(), start=1):
        for m in js_attr_re.finditer(line):
          _add(m.group(1), "js", i)
        for m in js_index_re.finditer(line):
          _add(m.group(1), "js", i)
    else:
      for i, line in enumerate(text.splitlines(), start=1):
        for m in cmake_env_re.finditer(line):
          _add(m.group(1), "cmake", i)

  return {
      "vars": sorted(matches.keys()),
      "occurrences": matches,
  }


def _module_name_for_py(rel: str) -> str:
  rel_no_ext = rel[:-3] if rel.endswith(".py") else rel
  parts = rel_no_ext.split("/")
  if parts and parts[-1] == "__init__":
    parts = parts[:-1]
  return ".".join(parts)


def _build_python_import_graph(repo_root: Path, rel_paths: Sequence[str]) -> dict:
  py_files = [p for p in rel_paths if p.endswith(".py") and not _is_excluded(p)]
  module_to_rel: dict[str, str] = {}
  for rel in py_files:
    module_to_rel[_module_name_for_py(rel)] = rel

  def _resolve_relative(base_module: str, level: int, module: str | None) -> str | None:
    if level <= 0:
      return module
    base_parts = base_module.split(".")
    if base_parts:
      base_parts = base_parts[:-1]
    if level > len(base_parts) + 1:
      return None
    prefix = base_parts[: len(base_parts) - (level - 1)]
    if module:
      prefix.extend(module.split("."))
    return ".".join(prefix) if prefix else (module or "")

  edges: dict[str, set[str]] = defaultdict(set)
  parse_errors: list[dict] = []

  for rel in py_files:
    src_mod = _module_name_for_py(rel)
    path = repo_root / rel
    text = _read_text_best_effort(path)
    if text is None:
      continue
    try:
      tree = ast.parse(text, filename=rel)
    except SyntaxError as exc:
      parse_errors.append({"path": rel, "error": str(exc)})
      continue

    for node in ast.walk(tree):
      if isinstance(node, ast.Import):
        for alias in node.names:
          mod = alias.name
          if mod in module_to_rel:
            edges[src_mod].add(mod)
      elif isinstance(node, ast.ImportFrom):
        resolved = _resolve_relative(src_mod, node.level, node.module)
        if resolved is None:
          continue
        if resolved in module_to_rel:
          edges[src_mod].add(resolved)
        for alias in node.names:
          cand = f"{resolved}.{alias.name}" if resolved else alias.name
          if cand in module_to_rel:
            edges[src_mod].add(cand)

  return {
      "modules": module_to_rel,
      "edges": {k: sorted(v) for k, v in edges.items()},
      "parse_errors": parse_errors,
  }


def _find_import_cycles(graph: Mapping[str, Sequence[str]]) -> list[list[str]]:
  cycles: list[list[str]] = []
  visited: set[str] = set()
  stack: list[str] = []
  in_stack: set[str] = set()

  def _dfs(node: str) -> None:
    visited.add(node)
    stack.append(node)
    in_stack.add(node)
    for nxt in graph.get(node, ()):
      if nxt not in visited:
        _dfs(nxt)
      elif nxt in in_stack:
        idx = stack.index(nxt)
        cycle = stack[idx:] + [nxt]
        cycles.append(cycle)
    stack.pop()
    in_stack.remove(node)

  for node in sorted(graph.keys()):
    if node not in visited:
      _dfs(node)

  # Deduplicate by canonical rotation.
  unique: dict[tuple[str, ...], list[str]] = {}
  for cyc in cycles:
    body = cyc[:-1]
    if not body:
      continue
    min_idx = min(range(len(body)), key=lambda i: body[i])
    rotated = body[min_idx:] + body[:min_idx]
    key = tuple(rotated)
    unique[key] = rotated + [rotated[0]]

  return [unique[k] for k in sorted(unique.keys())]


def _layer_violations(modules: Mapping[str, str], edges: Mapping[str, Sequence[str]]) -> list[dict]:
  def _rel(mod: str) -> str | None:
    return modules.get(mod)

  violations: list[dict] = []
  for src, dsts in edges.items():
    src_rel = _rel(src) or ""
    for dst in dsts:
      dst_rel = _rel(dst) or ""
      if src_rel.startswith("abi_exports/") and dst_rel.startswith("check/"):
        violations.append({"src": src_rel, "dst": dst_rel, "rule": "abi_exports must not import check"})
  return violations


def _dist_contract(repo_root: Path, rel_paths: Sequence[str]) -> dict:
  tracked = set(rel_paths)
  versions: set[str] = set()
  for rel in rel_paths:
    if not rel.startswith("dist/"):
      continue
    parts = rel.split("/", 2)
    if len(parts) >= 2 and parts[1]:
      versions.add(parts[1])
  version_sort_key = _dist_version_sort_key(repo_root)
  sorted_versions = sorted(versions, key=version_sort_key)

  def _req(ver: str, rel: str) -> bool:
    path = f"dist/{ver}/{rel}"
    return path in tracked and (repo_root / path).is_file()

  results: dict[str, dict] = {}
  missing_any = False
  for ver in sorted_versions:
    required = {
        "mujoco.js": _req(ver, "mujoco.js"),
        "mujoco.wasm": _req(ver, "mujoco.wasm"),
        "abi/exports.lst": _req(ver, "abi/exports.lst"),
    }
    pthreads_present = any(p.startswith(f"dist/{ver}/pthreads/") for p in tracked)
    pthreads_required = {
        "pthreads/mujoco.js": _req(ver, "pthreads/mujoco.js"),
        "pthreads/mujoco.wasm": _req(ver, "pthreads/mujoco.wasm"),
    } if pthreads_present else {}
    missing = [k for k, ok in {**required, **pthreads_required}.items() if not ok]
    if missing:
      missing_any = True
    results[ver] = {
        "required": required,
        "pthreads_present": pthreads_present,
        "pthreads_required": pthreads_required,
        "missing": missing,
    }

  return {
      "versions": sorted_versions,
      "results": results,
      "missing_any": missing_any,
  }


def _artifact_graph(repo_root: Path, versions: Sequence[str]) -> dict:
  stage_map = {
      "runtime": [
          "dist/<ver>/mujoco.js",
          "dist/<ver>/mujoco.wasm",
      ],
      "abi_contract": [
          "dist/<ver>/abi/exports.lst",
      ],
      "introspect": [
          "dist/<ver>/abi/mujoco_ast.json",
          "dist/<ver>/abi/functions_introspect_like.json",
          "dist/<ver>/abi/structs_introspect_like.json",
          "dist/<ver>/abi/enums_introspect_like.json",
      ],
      "wrappers": [
          "dist/<ver>/abi/wrapper_exports.json",
          "dist/<ver>/abi/wrapper_exports_funcs.json",
          "dist/<ver>/abi/mjapi.json",
      ],
      "nm_views": [
          "dist/<ver>/abi/nm_symbols.json",
          "dist/<ver>/abi/nm_coverage.json",
      ],
      "pthreads_runtime": [
          "dist/<ver>/pthreads/mujoco.js",
          "dist/<ver>/pthreads/mujoco.wasm",
      ],
  }

  per_version: dict[str, dict] = {}
  for ver in versions:
    base = repo_root / "dist" / ver
    stages: dict[str, dict] = {}
    for stage, templates in stage_map.items():
      resolved: dict[str, bool] = {}
      for tmpl in templates:
        rel = tmpl.replace("<ver>", ver)
        resolved[tmpl] = (repo_root / rel).is_file()
      stages[stage] = resolved

    # If pthreads dir is absent, don't treat pthreads artifacts as missing.
    if not (base / "pthreads").is_dir():
      stages["pthreads_runtime"] = {k: True for k in stage_map["pthreads_runtime"]}
    per_version[ver] = stages

  return {
      "stage_map": stage_map,
      "per_version": per_version,
  }


def _upgrade_story(repo_root: Path, rel_paths: Sequence[str]) -> dict:
  touchpoints = [
      "forge_cli.py",
      "app/CMakeLists.txt",
      "abi_exports/",
      "introspect/",
      "check/",
      "dist/<ver>/",
      "docs/en/reference_abi_contract.md",
      "docs/en/reference_checks_gates.md",
      "docs/en/reference_env_vars.md",
      "docs/en/reference_cli.md",
  ]
  present = []
  for p in touchpoints:
    if "<ver>" in p:
      present.append({"path": p, "present": True})
      continue
    present.append({"path": p, "present": (repo_root / p).exists()})

  generator_scripts = sorted([p for p in rel_paths if p.startswith("abi_exports/") and p.endswith(".py")])
  check_scripts = sorted([p for p in rel_paths if p.startswith("check/") and (p.endswith(".mjs") or p.endswith(".sh") or p.endswith(".py"))])

  steps = [
      "Build the new upstream ref: `python forge_cli.py build --version <ver> --with-checks`.",
      "Review ABI diffs under `dist/<ver>/abi/` (exports.lst + introspect_like + wrapper manifests).",
      "Commit the new `dist/<ver>/` tree (including `abi/`).",
      "If CI uses reproducible verification, run `python forge_cli.py verify-dist --version <ver>`.",
  ]

  return {
      "touchpoints": present,
      "generator_scripts": generator_scripts,
      "check_scripts": check_scripts,
      "steps": steps,
  }


def _python_hotspots(repo_root: Path, rel_paths: Sequence[str]) -> dict:
  py_files = [p for p in rel_paths if p.endswith(".py") and not _is_excluded(p)]
  file_loc: list[dict] = []
  func_loc: list[dict] = []
  subprocess_counts: list[dict] = []

  for rel in py_files:
    path = repo_root / rel
    text = _read_text_best_effort(path)
    if text is None:
      continue
    lines = text.splitlines()
    file_loc.append({"path": rel, "loc": len(lines)})

    try:
      tree = ast.parse(text, filename=rel)
    except SyntaxError:
      continue

    class _Visitor(ast.NodeVisitor):
      def visit_FunctionDef(self, node: ast.FunctionDef) -> None:
        if hasattr(node, "end_lineno") and node.end_lineno and node.lineno:
          func_loc.append(
              {
                  "path": rel,
                  "name": node.name,
                  "loc": int(node.end_lineno) - int(node.lineno) + 1,
                  "line": node.lineno,
              }
          )
        self.generic_visit(node)

      def visit_AsyncFunctionDef(self, node: ast.AsyncFunctionDef) -> None:
        if hasattr(node, "end_lineno") and node.end_lineno and node.lineno:
          func_loc.append(
              {
                  "path": rel,
                  "name": node.name,
                  "loc": int(node.end_lineno) - int(node.lineno) + 1,
                  "line": node.lineno,
              }
          )
        self.generic_visit(node)

    _Visitor().visit(tree)

    class _SubprocessVisitor(ast.NodeVisitor):
      def __init__(self) -> None:
        self.count = 0

      def visit_Call(self, node: ast.Call) -> None:
        if (
            isinstance(node.func, ast.Attribute)
            and isinstance(node.func.value, ast.Name)
            and node.func.value.id == "subprocess"
            and node.func.attr == "run"
        ):
          self.count += 1
        self.generic_visit(node)

    sp = _SubprocessVisitor()
    sp.visit(tree)
    if sp.count:
      subprocess_counts.append({"path": rel, "subprocess_run_calls": sp.count})

  file_loc.sort(key=lambda x: x["loc"], reverse=True)
  func_loc.sort(key=lambda x: x["loc"], reverse=True)
  subprocess_counts.sort(key=lambda x: x["subprocess_run_calls"], reverse=True)

  return {
      "file_loc_top": file_loc[:20],
      "func_loc_top": func_loc[:20],
      "subprocess_run_top": subprocess_counts[:20],
  }


def _duplicate_blocks(repo_root: Path, rel_paths: Sequence[str]) -> dict:
  interesting_ext = {".py", ".mjs", ".js", ".sh", ".ps1", ".cmake", ".md", ".yml", ".yaml", ".txt", ".lst", ".c", ".cc", ".h"}
  seen: dict[str, dict] = {}
  duplicates: dict[str, list[dict]] = defaultdict(list)

  def _normalize(line: str) -> str:
    return re.sub(r"\s+", " ", line.strip())

  for rel in rel_paths:
    if _is_excluded(rel):
      continue
    path = repo_root / rel
    if path.suffix.lower() not in interesting_ext and path.name != "CMakeLists.txt":
      continue
    text = _read_text_best_effort(path)
    if not text:
      continue
    norm_lines: list[str] = []
    line_map: list[int] = []
    for lineno, line in enumerate(text.splitlines(), start=1):
      norm = _normalize(line)
      if norm:
        norm_lines.append(norm)
        line_map.append(lineno)
    if len(norm_lines) < _WINDOW_LINES:
      continue
    for i in range(0, len(norm_lines) - _WINDOW_LINES + 1):
      window = "\n".join(norm_lines[i : i + _WINDOW_LINES])
      h = hashlib.sha1(window.encode("utf-8")).hexdigest()
      occ = {"path": rel, "start_line": line_map[i]}
      if h in seen:
        if not duplicates[h]:
          duplicates[h].append(seen[h]["occ"])
        duplicates[h].append(occ)
      else:
        seen[h] = {"occ": occ}

  blocks: list[dict] = []
  for h, occs in duplicates.items():
    blocks.append(
        {
            "hash": h,
            "line_count": _WINDOW_LINES,
            "occurrences": occs,
            "occurrence_count": len(occs),
        }
    )

  blocks.sort(key=lambda b: (b["occurrence_count"], b["hash"]), reverse=True)
  return {"top": blocks[:20]}


def _version_literals(repo_root: Path, rel_paths: Sequence[str]) -> dict:
  pat = re.compile(r"\b\d+\.\d+\.\d+\b")
  hits: dict[str, list[dict]] = defaultdict(list)
  for rel in rel_paths:
    if _is_excluded(rel):
      continue
    path = repo_root / rel
    text = _read_text_best_effort(path)
    if not text:
      continue
    for i, line in enumerate(text.splitlines(), start=1):
      for m in pat.finditer(line):
        hits[m.group(0)].append({"path": rel, "line": i})
  items = sorted(hits.items(), key=lambda kv: len(kv[1]), reverse=True)
  top = [{"version": v, "count": len(occs), "occurrences": occs[:20]} for v, occs in items[:20]]
  return {"top": top}


def _decision_page(report: Mapping[str, object]) -> list[str]:
  risks: list[str] = []

  hotspots = report.get("hotspots", {})
  if isinstance(hotspots, dict):
    funcs = hotspots.get("func_loc_top")
    if isinstance(funcs, list) and funcs:
      top = funcs[0]
      risks.append(
          f"Hotspot: {top.get('path')}:{top.get('line')} function {top.get('name')} is {top.get('loc')} LOC."
      )

  dups = report.get("duplicates", {})
  if isinstance(dups, dict):
    top_dups = dups.get("top")
    if isinstance(top_dups, list) and top_dups:
      d = top_dups[0]
      risks.append(
          f"Duplication: a {_WINDOW_LINES}-line block repeats {d.get('occurrence_count')} times."
      )

  gate = report.get("gate", {})
  if isinstance(gate, dict):
    failures = gate.get("failures")
    if isinstance(failures, list) and failures:
      risks.append(f"Gate failures: {len(failures)} high-signal regressions detected.")

  return risks[:3]


def _write_outputs(out_dir: Path, report: Mapping[str, object]) -> None:
  out_dir.mkdir(parents=True, exist_ok=True)
  (out_dir / "arch_report.json").write_text(
      json.dumps(report, sort_keys=True, indent=2) + "\n",
      encoding="utf-8",
  )

  pipeline_mermaid = report.get("pipeline", {}).get("mermaid") if isinstance(report.get("pipeline"), dict) else None
  decision = report.get("decision_page", [])
  md_lines: list[str] = []
  md_lines.append("# Architecture Review Decision Page")
  md_lines.append("")
  md_lines.append("## Top risk candidates")
  if decision:
    for idx, item in enumerate(decision, start=1):
      md_lines.append(f"{idx}. {item}")
  else:
    md_lines.append("- (none detected)")
  md_lines.append("")
  md_lines.append("## Maintainer decisions (fill)")
  md_lines.append("- Choose 0-2 refactor tasks from the evidence below and track them as issues/ADRs.")
  md_lines.append("")
  md_lines.append("# Evidence")
  md_lines.append("")

  md_lines.append("## Pipeline (cmd_build)")
  md_lines.append("")
  if pipeline_mermaid:
    md_lines.append("```mermaid")
    md_lines.append(str(pipeline_mermaid).rstrip("\n"))
    md_lines.append("```")
  else:
    md_lines.append("- No pipeline stages found.")
  md_lines.append("")

  artifacts = report.get("artifacts", {})
  if isinstance(artifacts, dict):
    md_lines.append("## Artifacts (dist contract view)")
    md_lines.append("")
    dist = report.get("dist_contract", {})
    if isinstance(dist, dict):
      versions = dist.get("versions", [])
      md_lines.append(f"- Committed dist versions: {', '.join(versions) if versions else '(none)'}")
    md_lines.append("")

  gate = report.get("gate", {})
  if isinstance(gate, dict):
    failures = gate.get("failures", [])
    md_lines.append("## Gate summary")
    md_lines.append("")
    if failures:
      md_lines.append(f"- Status: FAIL ({len(failures)} issues)")
      for f in failures[:20]:
        md_lines.append(f"  - {f.get('kind')}: {f.get('message')}")
    else:
      md_lines.append("- Status: PASS")
    md_lines.append("")

  md_lines.append("## Interfaces")
  md_lines.append("")
  interfaces = report.get("interfaces", {})
  if isinstance(interfaces, dict):
    cli = interfaces.get("cli", {})
    if isinstance(cli, dict):
      cmds = cli.get("commands", [])
      md_lines.append(f"- CLI commands: {', '.join(cmds) if cmds else '(none)'}")
    env = interfaces.get("env_vars", {})
    if isinstance(env, dict):
      vars_ = env.get("vars", [])
      md_lines.append(f"- Env vars discovered (code scan): {len(vars_)}")
  md_lines.append("")

  md_lines.append("## Hotspots (top)")
  md_lines.append("")
  hotspots = report.get("hotspots", {})
  if isinstance(hotspots, dict):
    for item in hotspots.get("func_loc_top", [])[:10]:
      md_lines.append(f"- {item.get('path')}:{item.get('line')} {item.get('name')} ({item.get('loc')} LOC)")
  md_lines.append("")

  md_lines.append("## Duplicate blocks (top)")
  md_lines.append("")
  dups = report.get("duplicates", {})
  if isinstance(dups, dict):
    top = dups.get("top", [])[:5]
    if not top:
      md_lines.append("- (none detected)")
    for d in top:
      md_lines.append(f"- {d.get('occurrence_count')}x repeats (hash {d.get('hash')})")
      for occ in d.get("occurrences", [])[:8]:
        md_lines.append(f"  - {occ.get('path')}:{occ.get('start_line')}")
  md_lines.append("")

  upgrade = report.get("upgrade", {})
  if isinstance(upgrade, dict):
    md_lines.append("## Upgrade story (template)")
    md_lines.append("")
    for step in upgrade.get("steps", [])[:10]:
      md_lines.append(f"- {step}")
    md_lines.append("")

  (out_dir / "arch_report.md").write_text("\n".join(md_lines) + "\n", encoding="utf-8")


def cmd_arch(args: argparse.Namespace) -> int:
  repo_root = _repo_root()
  rel_paths = _git_ls_files(repo_root)
  scan_paths = [p for p in rel_paths if not _is_excluded(p)]

  forge_cli = repo_root / "forge_cli.py"
  pipeline_stages = _extract_cmd_build_stages(forge_cli) if forge_cli.is_file() else []
  pipeline = {
      "stages": pipeline_stages,
      "mermaid": _mermaid_pipeline(pipeline_stages),
  }

  cli_surface = _extract_cli_surface(repo_root)
  cli_docs = _extract_documented_cli_commands(
      [
          repo_root / "docs/en/reference_cli.md",
          repo_root / "docs/zh/reference_cli.md",
      ]
  )
  documented_cli = set().union(*cli_docs.values()) if cli_docs else set()

  env_docs = _extract_documented_env_vars(
      [
          repo_root / "docs/en/reference_env_vars.md",
          repo_root / "docs/zh/reference_env_vars.md",
      ]
  )
  documented_env = set().union(*env_docs.values()) if env_docs else set()

  env_vars = _extract_env_vars_from_files(repo_root, scan_paths)
  env_in_code = set(env_vars["vars"])
  gated_env = {
      v
      for v in env_in_code
      if v.startswith(_DOC_ENV_VARS_GATED_PREFIXES) or v in _DOC_ENV_VARS_GATED_EXACT
  }

  undocumented_env = sorted(gated_env - documented_env)
  stale_env = sorted(documented_env - env_in_code)

  py_graph = _build_python_import_graph(repo_root, scan_paths)
  cycles = _find_import_cycles(py_graph.get("edges", {}))
  violations = _layer_violations(py_graph.get("modules", {}), py_graph.get("edges", {}))

  dist_contract = _dist_contract(repo_root, rel_paths)
  artifacts = _artifact_graph(repo_root, dist_contract.get("versions", []))
  hotspots = _python_hotspots(repo_root, scan_paths)
  duplicates = _duplicate_blocks(repo_root, scan_paths)
  versions = _version_literals(repo_root, scan_paths)
  upgrade = _upgrade_story(repo_root, rel_paths)

  failures: list[dict] = []
  cli_cmds = set(cli_surface.get("commands", []))
  if documented_cli and cli_cmds != documented_cli:
    failures.append(
        {
            "kind": "cli_docs_mismatch",
            "message": f"CLI commands differ: code={sorted(cli_cmds)} docs={sorted(documented_cli)}",
        }
    )
  if undocumented_env:
    failures.append(
        {
            "kind": "undocumented_env_vars",
            "message": f"Env vars used in code but not documented: {undocumented_env}",
        }
    )
  if cycles:
    failures.append({"kind": "python_import_cycles", "message": f"Detected {len(cycles)} cycle(s)."})
  if violations:
    failures.append(
        {
            "kind": "layer_violations",
            "message": f"Detected {len(violations)} layering violation(s).",
        }
    )
  if dist_contract.get("missing_any"):
    failures.append({"kind": "dist_contract", "message": "Missing required dist/<ver> artifacts."})

  report: dict[str, object] = {
      "generated_at": _now_iso(),
      "repo_root": repo_root.as_posix(),
      "pipeline": pipeline,
      "interfaces": {
          "cli": {
              "commands": cli_surface.get("commands", []),
              "exit_code": cli_surface.get("exit_code", None),
              "documented_commands": {k: sorted(v) for k, v in cli_docs.items()},
          },
          "env_vars": {
              "vars": env_vars["vars"],
              "documented": {k: sorted(v) for k, v in env_docs.items()},
              "undocumented_gated": undocumented_env,
              "stale_docs_entries": stale_env,
          },
      },
      "architecture": {
          "python_import_cycles": cycles,
          "layer_violations": violations,
          "python_parse_errors": py_graph.get("parse_errors", []),
      },
      "dist_contract": dist_contract,
      "artifacts": artifacts,
      "hotspots": hotspots,
      "duplicates": duplicates,
      "upgrade": {**upgrade, "version_literals": versions},
      "gate": {
          "mode": args.mode,
          "failures": failures,
      },
  }
  report["decision_page"] = _decision_page(report)

  out_dir = Path(args.out).expanduser()
  if not out_dir.is_absolute():
    out_dir = repo_root / out_dir
  _write_outputs(out_dir, report)

  if args.mode == "gate" and failures:
    return 2
  return 0


def build_parser() -> argparse.ArgumentParser:
  parser = argparse.ArgumentParser(prog="qa")
  subparsers = parser.add_subparsers(dest="command", required=True)

  p_arch = subparsers.add_parser("arch", help="Architecture/maintainability evidence + gates.")
  p_arch.add_argument(
      "--mode",
      choices=("report", "gate"),
      default="gate",
      help="report always exits 0; gate fails on high-signal regressions.",
  )
  p_arch.add_argument("--out", default="_qa", help="Output directory for arch_report.{json,md}.")
  p_arch.set_defaults(func=cmd_arch)
  return parser


def main(argv: Sequence[str] | None = None) -> int:
  parser = build_parser()
  args = parser.parse_args(argv)
  return int(args.func(args))


if __name__ == "__main__":
  raise SystemExit(main())
