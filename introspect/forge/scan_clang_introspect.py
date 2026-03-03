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

  python introspect/forge/scan_clang_introspect.py \\
      --header external/mujoco/include/mujoco/mujoco.h \\
      --out-dir dist/<ver>/abi \\
      [--clang clang]
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any, Dict, List, Mapping, Optional


def _repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def _to_repo_relpath(path: Path, repo_root: Path) -> str:
    """Return a stable repo-relative POSIX path, or just the basename."""
    try:
        rel = path.resolve().relative_to(repo_root.resolve())
    except (OSError, RuntimeError, ValueError):
        return path.name
    return rel.as_posix()


def _windows_path_to_wsl_mount(path: Path) -> Optional[str]:
    """Best-effort map `C:\\...` -> `/mnt/c/...` for WSL builds."""
    drive = path.drive
    if not drive or len(drive) < 2 or drive[1] != ":":
        return None
    drive_letter = drive[0].lower()
    parts = [p for p in path.parts if p not in (drive, drive + "\\", drive + "/")]
    if not parts:
        return f"/mnt/{drive_letter}"
    return "/".join(["/mnt", drive_letter, *parts])


def _repo_root_prefixes(repo_root: Path) -> List[str]:
    prefixes = set()
    root_str = str(repo_root)
    prefixes.add(root_str)
    prefixes.add(root_str.replace("\\", "/"))
    wsl = _windows_path_to_wsl_mount(repo_root)
    if wsl:
        prefixes.add(wsl)
    return sorted(prefixes, key=len, reverse=True)


def _relativize_repo_paths_in_text(text: str, repo_root: Path) -> str:
    """Strip repo root prefixes from serialized artifacts to avoid local paths."""
    for prefix in _repo_root_prefixes(repo_root):
        text = text.replace(prefix + "\\", "")
        text = text.replace(prefix + "/", "")
    return text


def _ensure_local_introspect_on_path() -> Path:
    """Expose introspect.official as the top-level `introspect` package.

    We keep an unmodified copy of MuJoCo's official introspect package under
    `introspect/official`. The shim in `introspect/__init__.py` re-exports
    those modules so that imports like `from introspect import ast_nodes`
    continue to behave exactly as upstream code expects.
    """
    repo_root = Path(__file__).resolve().parents[2]
    py_root = repo_root  # repo root; `introspect` lives directly under here
    pkg_root = py_root / "introspect" / "official"
    if not pkg_root.is_dir():
        raise RuntimeError(f"introspect.official package not found under {py_root}")
    if str(py_root) not in sys.path:
        sys.path.insert(0, str(py_root))
    import importlib  # noqa: WPS433

    # This will resolve to the shim in introspect/__init__.py, which in turn
    # exposes the official modules.
    importlib.import_module("introspect")
    return py_root


def _run_clang_ast_dump_to_file(
    header: Path,
    json_out: Path,
    clang: str,
    repo_root: Path,
) -> None:
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
        encoding="utf-8",
        cwd=str(repo_root),
    )
    if proc.returncode != 0:
        raise RuntimeError(
            f"clang AST dump failed (exit {proc.returncode}). "
            f"Command: {' '.join(cmd)}\n"
            f"stderr:\n{proc.stderr}"
        )
    json_out.write_text(
        _relativize_repo_paths_in_text(proc.stdout, repo_root),
        encoding="utf-8",
    )


def _run_generator_module(
    module: str, args: List[str], py_root: Path, cwd: Path
) -> str:
    """在子进程中运行官方 generate_* 模块，返回其 stdout 源码."""
    env = os.environ.copy()
    existing = env.get("PYTHONPATH")
    env["PYTHONPATH"] = str(py_root) + (os.pathsep + existing if existing else "")
    # Ensure subprocess stdout/stderr use UTF-8 so captured sources are portable.
    env.setdefault("PYTHONUTF8", "1")
    env.setdefault("PYTHONIOENCODING", "utf-8")
    cmd = [sys.executable, "-m", module] + args
    proc = subprocess.run(
        cmd,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        env=env,
        cwd=str(cwd),
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
        param_decls: List[str] = []
        param_names: List[str] = []
        for idx, p in enumerate(getattr(f, "parameters", ())
        ):
            pname = getattr(p, "name", None) or f"p{idx}"
            ptype = getattr(p, "type", None)
            # Keep the rich AST view for downstream consumers.
            params.append(
                {
                    "name": pname,
                    "nullable": bool(getattr(p, "nullable", False)),
                    "type": _type_repr(ptype),
                }
            )
            # Additionally, capture the C-like declarator string using the
            # official AST printer. This lets gen_funcs.py consume only the
            # JSON output without having to re-import introspect.
            decl = ptype.decl(pname) if ptype is not None else pname
            param_decls.append(str(decl))
            param_names.append(str(pname))

        return_type_obj = getattr(f, "return_type", None)
        return_decl = (
            return_type_obj.decl() if return_type_obj is not None else "void"
        )

        out.append(
            {
                "name": name,
                "return_type": _type_repr(return_type_obj),
                "return_decl": str(return_decl),
                "params": params,
                "param_decls": param_decls,
                "param_names": param_names,
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

    repo_root = _repo_root()

    header_arg = args.header
    header_abs = (repo_root / header_arg) if not header_arg.is_absolute() else header_arg
    header_abs = header_abs.resolve()
    if not header_abs.is_file():
        raise SystemExit(f"Header not found: {header_abs}")

    out_dir = args.out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    py_root = _ensure_local_introspect_on_path()

    # Prefer repo-relative paths for reproducible artifacts.
    header_for_tools = _to_repo_relpath(header_abs, repo_root)
    header_for_meta = header_for_tools

    ast_json_path = out_dir / "mujoco_ast.json"
    _run_clang_ast_dump_to_file(
        Path(header_for_tools),
        ast_json_path,
        clang=args.clang,
        repo_root=repo_root,
    )

    # 运行官方三个 generate_* 模块，拿到 FUNCTIONS / STRUCTS / ENUMS 的 Python 源码
    funcs_src = _run_generator_module(
        "introspect.official.codegen.generate_functions",
        ["--header_path", header_for_tools, "--json_path", str(ast_json_path)],
        py_root,
        cwd=repo_root,
    )
    structs_src = _run_generator_module(
        "introspect.official.codegen.generate_structs",
        ["--json_path", str(ast_json_path)],
        py_root,
        cwd=repo_root,
    )
    enums_src = _run_generator_module(
        "introspect.official.codegen.generate_enums",
        ["--json_path", str(ast_json_path)],
        py_root,
        cwd=repo_root,
    )

    # 在当前进程内 exec 生成的源码，只做一次 py->json 转换
    functions_map = _load_mapping_from_source(funcs_src, "FUNCTIONS")
    structs_map = _load_mapping_from_source(structs_src, "STRUCTS")
    enums_map = _load_mapping_from_source(enums_src, "ENUMS")

    functions = _functions_to_json(functions_map)
    structs = _structs_to_json(structs_map)
    enums = _enums_to_json(enums_map)

    meta = {
        "header": header_for_meta,
        "clang": args.clang,
        "ast_json": _to_repo_relpath(ast_json_path, repo_root),
    }

    (out_dir / "functions_introspect_like.json").write_text(
        _relativize_repo_paths_in_text(
            json.dumps({"meta": meta, "functions": functions}, indent=2, sort_keys=True),
            repo_root,
        ),
        encoding="utf-8",
    )
    (out_dir / "structs_introspect_like.json").write_text(
        _relativize_repo_paths_in_text(
            json.dumps({"meta": meta, "structs": structs}, indent=2, sort_keys=True),
            repo_root,
        ),
        encoding="utf-8",
    )
    (out_dir / "enums_introspect_like.json").write_text(
        _relativize_repo_paths_in_text(
            json.dumps({"meta": meta, "enums": enums}, indent=2, sort_keys=True),
            repo_root,
        ),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
