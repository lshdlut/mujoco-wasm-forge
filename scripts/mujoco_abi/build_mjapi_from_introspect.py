#!/usr/bin/env python3
"""Build mjapi.json (function declarations A-set) from official introspect.

This script uses the vendored MuJoCo introspect package under
scripts/mujoco_abi/introspect to enumerate API functions and convert them
into the JSON shape expected by gen_exports_from_abi.mjs.

It replaces the header-parsing path (autogen_wrappers.mjs + functions.json).
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Dict, List


def _ensure_introspect_on_path() -> None:
  """Expose scripts/mujoco_abi/introspect as top-level `introspect` package."""
  repo_root = Path(__file__).resolve().parents[2]
  py_root = repo_root / "scripts" / "mujoco_abi"
  pkg_root = py_root / "introspect"
  if not pkg_root.is_dir():
    raise RuntimeError(f"introspect package not found under {py_root}")
  if str(py_root) not in sys.path:
    sys.path.insert(0, str(py_root))


_ensure_introspect_on_path()
from introspect import functions as introspect_functions  # type: ignore  # noqa: E402


def _build_function_entry(name: str, f: Any) -> Dict[str, Any]:
  """Convert a FunctionDecl into the legacy mjapi/functions.json shape."""
  # Return type as a C declarator string.
  return_type = f.return_type.decl()

  param_decls: List[str] = []
  param_names: List[str] = []
  for idx, p in enumerate(getattr(f, "parameters", ())):
    pname = getattr(p, "name", None) or f"p{idx}"
    ptype = getattr(p, "type", None)
    # Use the introspect AST's own printer to get a C-like type declarator.
    decl = ptype.decl(pname) if ptype is not None else pname
    param_decls.append(str(decl))
    param_names.append(str(pname))

  # Introspect's FunctionDecl does not expose varargs directly; treating all
  # functions as non-variadic here keeps the export set aligned with the
  # canonical FUNCTIONS table.
  entry: Dict[str, Any] = {
    "name": name,
    "returnType": str(return_type),
    "params": param_decls[:],
    "paramDecls": param_decls,
    "paramNames": param_names,
    "isVariadic": False,
    "variadicIndex": -1,
    "baseParamNames": param_names[:],
  }
  return entry


def build_mjapi_payload() -> Dict[str, Any]:
  funcs_map = introspect_functions.FUNCTIONS
  names = sorted(funcs_map.keys())

  functions: List[Dict[str, Any]] = []
  for name in names:
    f = funcs_map[name]
    functions.append(_build_function_entry(name, f))

  payload: Dict[str, Any] = {
    "generatedAt": introspect_functions.__dict__.get("GENERATED_AT", None)
    or None,
    "includeDir": "external/mujoco/include",
    "headers": ["mujoco/mujoco.h", "mujoco/mjspec.h"],
    "count": len(names),
    "names": names,
    "functions": functions,
  }
  return payload


def main(argv: List[str] | None = None) -> None:
  parser = argparse.ArgumentParser()
  parser.add_argument(
    "--out",
    type=Path,
    required=True,
    help="Output path for mjapi.json.",
  )
  args = parser.parse_args(argv)

  payload = build_mjapi_payload()
  out_path: Path = args.out
  out_path.parent.mkdir(parents=True, exist_ok=True)
  out_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")
  print(
    f"[build-mjapi-from-introspect] wrote {out_path} "
    f"(functions={payload['count']})"
  )


if __name__ == "__main__":
  main()

