"""
MuJoCo ABI scan via official introspect codegen pipeline.

We do NOT reimplement any scanning rules. Instead we:
  1) Use clang to dump a JSON AST for mujoco.h
  2) Invoke MuJoCo's own introspect codegen scripts:
       - introspect.codegen.generate_functions
       - introspect.codegen.generate_structs
       - introspect.codegen.generate_enums
  3) Capture the generated Python source that defines
       FUNCTIONS / STRUCTS / ENUMS
  4) exec 这段源码，在内存里拿到上述三个 dict
  5) 只做一件我们自己的事：把它们转换成 JSON

这样可以保证：所有“扫什么、怎么解析类型/维度/nullable”的规则
完全由官方脚本决定，我们只负责 py->json 的纯格式转换。

Usage (from repo root):

  python -m scripts.mujoco_abi.scan_clang_introspect \\
      --header external/mujoco/include/mujoco/mujoco.h \\
      --out-dir dist/3.3.7/abi \\
      [--clang clang]
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any, Dict, List, Mapping


def _ensure_local_introspect_on_path() -> Path:
    """Expose scripts/mujoco_abi/introspect as top-level `introspect` package.

    We take a verbatim copy of MuJoCo's official introspect package under
    scripts/mujoco_abi/introspect so that all scanning logic lives inside this
    repo and is driven from the ABI toolchain, without importing from
    external/ or local_tools/ at runtime.
    """
    repo_root = Path(__file__).resolve().parents[2]
    py_root = repo_root / "scripts" / "mujoco_abi"
    pkg_root = py_root / "introspect"
    if not pkg_root.is_dir():
        raise RuntimeError(f"introspect package not found under {py_root}")
    if str(py_root) not in sys.path:
        sys.path.insert(0, str(py_root))
    import importlib  # noqa: WPS433

    importlib.import_module("introspect")
    return py_root


def _run_clang_ast_dump_to_file(header: Path, json_out: Path, clang: str) -> None:
    """调用 clang 为 given header 输出 JSON AST 到文件."""
    include_dir = header.parent.parent  # .../include
    cmd = [
        clang,
        "-Xclang",
        "-ast-dump=json",
        "-fsyntax-only",
        "-fparse-all-comments",
        "-I",
        str(include_dir),
        "-x",
        "c",
        str(header),
    ]
    proc = subprocess.run(
        cmd,
        check=False,
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0:
        raise RuntimeError(
            f"clang AST dump failed (exit {proc.returncode}). "
            f"Command: {' '.join(cmd)}\n"
            f"stderr:\n{proc.stderr}"
        )
    json_out.write_text(proc.stdout, encoding="utf-8")


def _run_generator_module(module: str, args: List[str], py_root: Path) -> str:
    """在子进程中运行官方 generate_* 模块，返回其 stdout 源码."""
    env = os.environ.copy()
    existing = env.get("PYTHONPATH")
    env["PYTHONPATH"] = str(py_root) + (os.pathsep + existing if existing else "")
    cmd = [sys.executable, "-m", module] + args
    proc = subprocess.run(
        cmd,
        check=False,
        capture_output=True,
        text=True,
        env=env,
    )
    if proc.returncode != 0:
        raise RuntimeError(
            f"Generator {module} failed (exit {proc.returncode}). "
            f"Command: {' '.join(cmd)}\n"
            f"stderr:\n{proc.stderr}"
        )
    return proc.stdout


def _load_mapping_from_source(source: str, symbol: str) -> Mapping[str, Any]:
    """exec 官方生成的 Python 源码，取出顶层 dict（FUNCTIONS/STRUCTS/ENUMS）."""
    ns: Dict[str, Any] = {
        "__name__": "introspect._generated",
        "__package__": "introspect",
    }
    exec(source, ns, ns)  # noqa: S102
    mapping = ns.get(symbol)
    if mapping is None:
        raise RuntimeError(f"Symbol {symbol!r} not found in generated source.")
    if not isinstance(mapping, dict):
        raise RuntimeError(f"Symbol {symbol!r} is not a dict (got {type(mapping)}).")
    return mapping


def _type_repr(t: Any) -> Dict[str, Any]:
    """把 introspect.ast_nodes 里的类型节点转成 JSON 友好的结构."""
    cls_name = type(t).__name__
    out: Dict[str, Any] = {"kind": cls_name}
    if cls_name == "ValueType":
        out["name"] = getattr(t, "name", None)
        out["is_const"] = bool(getattr(t, "is_const", False))
        out["is_volatile"] = bool(getattr(t, "is_volatile", False))
    elif cls_name == "PointerType":
        out["inner"] = _type_repr(getattr(t, "inner_type", None))
        out["is_const"] = bool(getattr(t, "is_const", False))
        out["is_volatile"] = bool(getattr(t, "is_volatile", False))
        out["is_restrict"] = bool(getattr(t, "is_restrict", False))
    elif cls_name == "ArrayType":
        out["inner"] = _type_repr(getattr(t, "inner_type", None))
        extents = getattr(t, "extents", ())
        out["extents"] = [str(e) for e in extents]
    else:
        out["repr"] = repr(t)
    return out


def _functions_to_json(functions: Mapping[str, Any]) -> List[Dict[str, Any]]:
    out: List[Dict[str, Any]] = []
    for name, f in functions.items():
        params = []
        for p in getattr(f, "parameters", ()):
            params.append(
                {
                    "name": getattr(p, "name", None),
                    "nullable": bool(getattr(p, "nullable", False)),
                    "type": _type_repr(getattr(p, "type", None)),
                }
            )
        out.append(
            {
                "name": name,
                "return_type": _type_repr(getattr(f, "return_type", None)),
                "params": params,
                "doc": getattr(f, "doc", None),
            }
        )
    out.sort(key=lambda f: f["name"])
    return out


def _structs_to_json(structs: Mapping[str, Any]) -> Dict[str, Any]:
    result: Dict[str, Any] = {}
    for name, s in structs.items():
        fields_out: List[Dict[str, Any]] = []
        for field in getattr(s, "fields", ()):
            fields_out.append(
                {
                    "name": getattr(field, "name", None),
                    "type": _type_repr(getattr(field, "type", None)),
                    "doc": getattr(field, "doc", None),
                    "array_extent": getattr(field, "array_extent", None),
                }
            )
        result[name] = {
            "name": name,
            "declname": getattr(s, "declname", None),
            "fields": fields_out,
        }
    return result


def _enums_to_json(enums: Mapping[str, Any]) -> Dict[str, Any]:
    result: Dict[str, Any] = {}
    for name, e in enums.items():
        values = getattr(e, "values", {}) or {}
        result[name] = {
            "name": name,
            "declname": getattr(e, "declname", None),
            "values": values,
        }
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--header",
        type=Path,
        required=True,
        help="Path to mujoco.h in the MuJoCo C headers.",
    )
    parser.add_argument(
        "--clang",
        type=str,
        default="clang",
        help="clang executable to use for AST dump.",
    )
    parser.add_argument(
        "--out-dir",
        type=Path,
        required=True,
        help="Output directory for JSON scan artifacts.",
    )
    args = parser.parse_args()

    header = args.header.resolve()
    if not header.is_file():
        raise SystemExit(f"Header not found: {header}")

    out_dir = args.out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    py_root = _ensure_local_introspect_on_path()

    ast_json_path = out_dir / "mujoco_ast.json"
    _run_clang_ast_dump_to_file(header, ast_json_path, clang=args.clang)

    # 运行官方三个 generate_* 模块，拿到 FUNCTIONS / STRUCTS / ENUMS 的 Python 源码
    funcs_src = _run_generator_module(
        "introspect.codegen.generate_functions",
        ["--header_path", str(header), "--json_path", str(ast_json_path)],
        py_root,
    )
    structs_src = _run_generator_module(
        "introspect.codegen.generate_structs",
        ["--json_path", str(ast_json_path)],
        py_root,
    )
    enums_src = _run_generator_module(
        "introspect.codegen.generate_enums",
        ["--json_path", str(ast_json_path)],
        py_root,
    )

    # 在当前进程内 exec 生成的源码，只做一次 py->json 转换
    functions_map = _load_mapping_from_source(funcs_src, "FUNCTIONS")
    structs_map = _load_mapping_from_source(structs_src, "STRUCTS")
    enums_map = _load_mapping_from_source(enums_src, "ENUMS")

    functions = _functions_to_json(functions_map)
    structs = _structs_to_json(structs_map)
    enums = _enums_to_json(enums_map)

    meta = {
        "header": str(header),
        "clang": args.clang,
        "ast_json": str(ast_json_path),
    }

    (out_dir / "functions_introspect_like.json").write_text(
        json.dumps({"meta": meta, "functions": functions}, indent=2, sort_keys=True),
        encoding="utf-8",
    )
    (out_dir / "structs_introspect_like.json").write_text(
        json.dumps({"meta": meta, "structs": structs}, indent=2, sort_keys=True),
        encoding="utf-8",
    )
    (out_dir / "enums_introspect_like.json").write_text(
        json.dumps({"meta": meta, "enums": enums}, indent=2, sort_keys=True),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
