#!/usr/bin/env python3
"""
Generate struct-related WASM exports for official_app_337.

Design goals:
  - Depend only on MuJoCo's official introspection output
      dist/3.3.7/abi/structs_introspect_like.json
    and stop using the legacy scan.mjs structs.json / dim_map.json.
  - Keep rules explicit and simple, with minimal ad-hoc whitelists:
      * For all mjModel / mjData pointer or array fields export plain ptrs:
          mjwf_model_<field>_ptr, mjwf_data_<field>_ptr
      * For mjModel / mjData integer fields starting with 'n' export
        plain dimension functions:
          mjwf_model_n*(h), mjwf_data_n*(h)
  - Do not reintroduce YAML/spec-driven “counts/auto/derived/names/extras” layers.

This script generates only:
  - app/mjwf_abi_structs.h
  - app/mjwf_abi_structs.c
  - app/mjwf_abi_structs.lst

Usage (from repo root):
  python abi_exports/gen_structs.py app/mjwf_abi_structs.h app/mjwf_abi_structs.c
"""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional


# ---------------------------------------------------------------------------
# C header/source preambles
# ---------------------------------------------------------------------------

HDR_PREAMBLE = """\
// AUTO-GENERATED. Do not edit by hand.
// Source: abi_exports/gen_structs.py
#pragma once
#include <stdint.h>
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif

""".lstrip()

HDR_POST = """
#ifdef __cplusplus
}
#endif
"""

SRC_PREAMBLE = """\
// AUTO-GENERATED. Do not edit by hand.
// Source: abi_exports/gen_structs.py
#include <mujoco/mujoco.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

// helper entrypoints are defined in mjwf_handles.c
int mjwf_helper_valid(int h);

typedef struct { mjModel* m; mjData* d; } _mjwf_view_pair;

// Getters implemented in mjwf_handles.c
extern mjModel* _mjwf_model_of(int h);
extern mjData*  _mjwf_data_of(int h);

""".lstrip()


# ---------------------------------------------------------------------------
# Basic type and prefix configuration
# ---------------------------------------------------------------------------

# dtype mapping is kept compatible with the legacy generator so that JS-side
# code can reuse the same conventions.
CTYPE_BY_DTYPE = {
    "f64": "double*",
    "f32": "float*",
    "i32": "int32_t*",
    "u8": "uint8_t*",
}

BASETYPE_BY_DTYPE = {
    "f64": "double",
    "f32": "float",
    "i32": "int32_t",
    "u8": "uint8_t",
}

AUTO_DTYPE_BY_BASE = {
    "mjtNum": "f64",
    "double": "f64",
    "float": "f32",
    "mjtFloat": "f32",
    "mjtSize": "i32",
    "int": "i32",
    "int32_t": "i32",
    "uint32_t": "i32",
    "unsigned int": "i32",
    "unsigned": "i32",
    "mjtByte": "u8",
    "char": "u8",
    "signed char": "u8",
    "unsigned char": "u8",
    "uint8_t": "u8",
    "mjByte": "u8",
    "uintptr_t": "i32",
    "size_t": "i32",
    # Treat raw void* buffers as byte arrays on the JS side.
    "void": "u8",
}

STRUCT_PREFIX = {
    "mjModel": "model",
    "mjData": "data",
}

INT_DIM_BASES = {
    "int",
    "int32_t",
    "unsigned int",
    "unsigned",
    "mjtSize",
    "size_t",
}


def sanitize_segment(segment: str) -> str:
    import re

    cleaned = re.sub(r"[^0-9A-Za-z_]+", "_", segment)
    cleaned = re.sub(r"_+", "_", cleaned)
    return cleaned.strip("_")


def default_pointer_name(struct: str, field: str) -> str:
    """Canonical export name: mjModel.nq -> model_nq, mjData.qpos -> data_qpos."""
    prefix = STRUCT_PREFIX.get(struct, sanitize_segment(struct))
    cleaned_field = sanitize_segment(field)
    if cleaned_field:
        return f"{prefix}_{cleaned_field}"
    return prefix


@dataclass
class FieldInfo:
    struct: str
    name: str
    type: Dict[str, object]
    array_extent: Optional[List[object]]
    doc: Optional[str]


@dataclass
class PointerExport:
    struct: str            # "mjModel" or "mjData"
    field: str             # top-level field name
    ctype: str             # e.g. "double*"
    base_name: str         # e.g. "model_qpos"
    is_scalar: bool        # True if field is a scalar ValueType


@dataclass
class DimExport:
    struct: str            # "mjModel" or "mjData"
    field: str             # e.g. "nq"
    base_name: str         # e.g. "model_nq"


@dataclass
class DerivedExport:
    """Flatten nested struct fields into plain buffers.

    Supports both:
      - Single nested structs (for example mjModel.opt / vis / stat)
      - Struct arrays (for example mjData.contact / solver / timer / warning)
    """
    owner_struct: str       # "mjModel" or "mjData"
    base_field: str         # e.g. "opt" / "contact"
    target_struct: str      # e.g. "mjOption" / "mjContact"
    subfield: str           # e.g. "timestep" / "pos"
    dtype: str              # f64/f32/i32/u8
    basetype: str           # e.g. "double"
    width_expr: str         # elements per item ("1" or "3" or "3*4"...)
    count_expr: str         # how many items ("1" or "d->ncon" or "mjNISLAND*mjNSOLVER")
    base_name: str          # e.g. "model_opt_timestep" / "data_contact_pos"
    base_is_pointer: bool   # True if owner field itself is a pointer


def _repo_root() -> Path:
    # .../abi_exports -> repo_root at parents[1]
    return Path(__file__).resolve().parents[1]


def load_structs_introspect(path: Path) -> Dict[str, List[FieldInfo]]:
    data = json.loads(path.read_text(encoding="utf-8"))
    structs = data.get("structs", {})
    out: Dict[str, List[FieldInfo]] = {}
    for struct_name, s in structs.items():
        fields: List[FieldInfo] = []
        for f in s.get("fields", []):
            fields.append(
                FieldInfo(
                    struct=struct_name,
                    name=f.get("name") or "",
                    type=f.get("type") or {},
                    array_extent=f.get("array_extent"),
                    doc=f.get("doc"),
                )
            )
        out[struct_name] = fields
    return out


def _inner_value_type(t: Dict[str, object]) -> Optional[str]:
    kind = t.get("kind")
    if kind == "ValueType":
        return str(t.get("name"))
    if kind in ("PointerType", "ArrayType"):
        inner = t.get("inner") or {}
        return _inner_value_type(inner)  # type: ignore[arg-type]
    return None


def _is_pointer_like(t: Dict[str, object]) -> bool:
    """Pointer or array in C: something that can decay to pointer."""
    kind = t.get("kind")
    return kind in ("PointerType", "ArrayType")


def _dtype_for_type(t: Dict[str, object]) -> Optional[str]:
    base = _inner_value_type(t)
    if base is None:
        return None
    return AUTO_DTYPE_BY_BASE.get(base)


def collect_pointer_exports(structs: Dict[str, List[FieldInfo]]) -> List[PointerExport]:
    exports: List[PointerExport] = []
    for struct_name in ("mjModel", "mjData"):
        fields = structs.get(struct_name) or []
        for f in fields:
            if not f.name:
                continue

            t = f.type

            # 1) True pointer/array fields: expose the underlying C pointer.
            if _is_pointer_like(t):
                dtype = _dtype_for_type(t)
                if dtype is None:
                    # Pointer to non-numeric type (struct etc.): expose raw pointer.
                    base = _inner_value_type(t) or "void"
                    if base == "void":
                        dtype = "u8"
                    else:
                        # Let C type be `<base>*` and let JS side interpret
                        # the pointer as a raw address.
                        ctype = f"{base}*"
                        base_name = default_pointer_name(struct_name, f.name)
                        exports.append(
                            PointerExport(
                                struct=struct_name,
                                field=f.name,
                                ctype=ctype,
                                base_name=base_name,
                                is_scalar=False,
                            )
                        )
                        continue
                ctype = CTYPE_BY_DTYPE[dtype]
                base_name = default_pointer_name(struct_name, f.name)
                exports.append(
                    PointerExport(
                        struct=struct_name,
                        field=f.name,
                        ctype=ctype,
                        base_name=base_name,
                        is_scalar=False,
                    )
                )
                continue

            # 2) Pure numeric scalar fields: for non-dimension scalars, also
            #    provide a 1-element ptr view so that JS can treat data as
            #    a fully flat set of plain pointers.
            if t.get("kind") != "ValueType":
                continue

            # Dimension fields (nq / ncon / narena / etc.) only keep the
            # function form and do not export ptr variants.
            if f.name.startswith("n"):
                base = str(t.get("name"))
                if base in INT_DIM_BASES:
                    continue

            base = str(t.get("name"))
            dtype = AUTO_DTYPE_BY_BASE.get(base)
            if dtype is None:
                continue

            ctype = CTYPE_BY_DTYPE[dtype]
            base_name = default_pointer_name(struct_name, f.name)
            exports.append(
                PointerExport(
                    struct=struct_name,
                    field=f.name,
                    ctype=ctype,
                    base_name=base_name,
                    is_scalar=True,
                )
            )
    return exports


def collect_dim_exports(structs: Dict[str, List[FieldInfo]]) -> List[DimExport]:
    exports: List[DimExport] = []
    for struct_name in ("mjModel", "mjData"):
        fields = structs.get(struct_name) or []
        for f in fields:
            if not f.name or not f.name.startswith("n"):
                continue
            t = f.type
            if t.get("kind") != "ValueType":
                continue
            base = str(t.get("name"))
            if base not in INT_DIM_BASES:
                continue
            base_name = default_pointer_name(struct_name, f.name)
            exports.append(DimExport(struct=struct_name, field=f.name, base_name=base_name))
    return exports


def _extent_exprs(extents: List[object], owner_var: str) -> List[str]:
    """Convert array_extent list into C expression fragments."""
    out: List[str] = []
    for e in extents:
        if isinstance(e, str):
            # Simple rule: names starting with 'n' are treated as fields on
            # the owning struct (nq/nv/ncon/...).
            if e.startswith("n"):
                out.append(f"{owner_var}->{e}")
            else:
                out.append(e)
        else:
            out.append(str(e))
    return out


def _array_type_extents(t: Dict[str, object]) -> List[object]:
    """Extract extents from a JSON-encoded ArrayType (if any)."""
    if t.get("kind") != "ArrayType":
        return []
    raw = t.get("extents") or []
    out: List[object] = []
    for e in raw:
        out.append(e)
    return out


def _load_introspect_ast():
    """Import vendored introspect.ast_nodes from the local introspect package."""
    import sys as _sys

    repo_root = _repo_root()
    root_str = str(repo_root)
    if root_str not in _sys.path:
        _sys.path.insert(0, root_str)

    from introspect import ast_nodes as ia  # type: ignore

    return ia


def _dtype_and_width_from_ast_type(ast_type) -> Optional[tuple[str, str, List[int]]]:
    """Map introspect.ast_nodes type -> (dtype, basetype, extents)."""
    ia = _load_introspect_ast()

    if isinstance(ast_type, ia.ValueType):
        base = ast_type.name
        dtype = AUTO_DTYPE_BY_BASE.get(base)
        if dtype is None:
            return None
        return dtype, BASETYPE_BY_DTYPE[dtype], []

    if isinstance(ast_type, ia.ArrayType):
        inner = ast_type.inner_type
        if isinstance(inner, ia.ValueType):
            base = inner.name
            dtype = AUTO_DTYPE_BY_BASE.get(base)
            if dtype is None:
                return None
            return dtype, BASETYPE_BY_DTYPE[dtype], list(ast_type.extents)

    return None


def _iter_anonymous_struct_fields(type_dict: Dict[str, object]):
    """Yield (subfield, dtype, basetype, width_extents) for AnonymousStructDecl."""
    if type_dict.get("kind") != "AnonymousStructDecl":
        return
    repr_str = type_dict.get("repr")
    if not isinstance(repr_str, str):
        return

    # Evaluate the repr using vendored introspect AST nodes; this mirrors
    # how the official tooling reconstructs anonymous structs. Older repr
    # may include keyword-only args (nullable=...) that are not present in the
    # current __init__ signature, so strip them out first.
    ia = _load_introspect_ast()
    safe_repr = repr_str.replace(", nullable=False", "").replace(", nullable=True", "")

    ns = {
        "AnonymousStructDecl": ia.AnonymousStructDecl,
        "AnonymousUnionDecl": ia.AnonymousUnionDecl,
        "StructFieldDecl": ia.StructFieldDecl,
        "ValueType": ia.ValueType,
        "ArrayType": ia.ArrayType,
        "PointerType": ia.PointerType,
    }
    try:
        struct_obj = eval(safe_repr, ns, {})  # noqa: S307 - controlled input
    except Exception:
        return

    for field in getattr(struct_obj, "fields", []):
        if not isinstance(field, ia.StructFieldDecl):
            continue
        res = _dtype_and_width_from_ast_type(field.type)
        if res is None:
            continue
        dtype, basetype, extents = res
        yield field.name, dtype, basetype, extents


def collect_derived_exports(structs: Dict[str, List[FieldInfo]]) -> List[DerivedExport]:
    """Collect flattened views of nested struct fields (single + arrays).

    Rules:
      - Find fields on mjModel/mjData whose type is another struct:
          * ValueType with name == struct: single nested struct (opt/vis/stat)
          * PointerType(inner is struct) with array_extent: struct array
            (contact/solver/timer/warning)
      - For each numeric/array field of the target struct (with resolvable
        dtype), create a DerivedExport with:
          base_name = "<model|data>_<baseField>_<subfield>"
    """
    derived: List[DerivedExport] = []

    for owner_struct in ("mjModel", "mjData"):
        owner_var = "m" if owner_struct == "mjModel" else "d"
        owner_fields = structs.get(owner_struct) or []
        for f in owner_fields:
            if not f.name:
                continue
            t = f.type
            kind = t.get("kind")
            target_struct: Optional[str] = None
            base_is_pointer = False
            is_array = False

            if kind == "ValueType":
                name = t.get("name")
                if isinstance(name, str) and name in structs and name not in ("mjModel", "mjData"):
                    target_struct = name
                    base_is_pointer = False
                    is_array = False
            elif kind == "PointerType":
                inner = t.get("inner") or {}
                inner_kind = inner.get("kind")
                inner_name = inner.get("name")
                if inner_kind == "ValueType" and isinstance(inner_name, str) and inner_name in structs:
                    target_struct = inner_name
                    base_is_pointer = True
                    # Pointer-to-struct + array_extent => struct array (contact 等)。
                    is_array = bool(f.array_extent)
            elif kind == "ArrayType":
                # Static struct arrays (solver/timer/warning etc.), where the
                # number of elements is derived from the ArrayType extents.
                inner = t.get("inner") or {}
                inner_kind = inner.get("kind")
                inner_name = inner.get("name")
                if inner_kind == "ValueType" and isinstance(inner_name, str) and inner_name in structs:
                    target_struct = inner_name
                    base_is_pointer = True
                    is_array = True

            if not target_struct:
                continue

            # Compute count expression: arrays use extents, single structs use 1.
            if is_array:
                # Prefer the field's own array_extent (e.g. contact[ncon]).
                ext = list(f.array_extent) if f.array_extent else _array_type_extents(t)
                if ext:
                    parts = _extent_exprs(ext, owner_var)
                    count_expr = "*".join(parts) if parts else "1"
                else:
                    count_expr = "1"
            else:
                count_expr = "1"

            subfields = structs.get(target_struct) or []
            for sf in subfields:
                if not sf.name:
                    continue
                # Skip pointer-typed fields: those are covered by pointer
                # exports on the owning struct and do not have a well-defined
                # flattened view here.
                if sf.type.get("kind") == "PointerType":
                    continue

                dtype = _dtype_for_type(sf.type)
                if dtype is not None:
                    basetype = BASETYPE_BY_DTYPE[dtype]
                    extents = sf.array_extent or _array_type_extents(sf.type)
                    if extents:
                        width_parts = [str(e) for e in extents]
                        width_expr = "*".join(width_parts)
                    else:
                        width_expr = "1"

                    owner_prefix = STRUCT_PREFIX.get(owner_struct, sanitize_segment(owner_struct))
                    base_name = f"{owner_prefix}_{f.name}_{sf.name}"

                    derived.append(
                        DerivedExport(
                            owner_struct=owner_struct,
                            base_field=f.name,
                            target_struct=target_struct,
                            subfield=sf.name,
                            dtype=dtype,
                            basetype=basetype,
                            width_expr=width_expr,
                            count_expr=count_expr,
                            base_name=sanitize_segment(base_name),
                            base_is_pointer=base_is_pointer,
                        )
                    )

                # 匿名子 struct（mjVisual.global / quality / headlight / map / scale 等）：
                # 利用 introspect AST 的 repr 解析并展开。
                if sf.type.get("kind") == "AnonymousStructDecl":
                    owner_prefix = STRUCT_PREFIX.get(owner_struct, sanitize_segment(owner_struct))
                    for nested_name, ndtype, nbase, nextents in _iter_anonymous_struct_fields(sf.type):
                        width_expr = "1"
                        if nextents:
                            width_expr = "*".join(str(e) for e in nextents)
                        base_name = f"{owner_prefix}_{f.name}_{sf.name}_{nested_name}"
                        derived.append(
                            DerivedExport(
                                owner_struct=owner_struct,
                                base_field=f.name,
                                target_struct=target_struct,
                                subfield=f"{sf.name}.{nested_name}",
                                dtype=ndtype,
                                basetype=nbase,
                                width_expr=width_expr,
                                count_expr=count_expr,
                                base_name=sanitize_segment(base_name),
                                base_is_pointer=base_is_pointer,
                            )
                        )

    return derived


def generate_pointer_decl(pe: PointerExport) -> str:
    return f"EMSCRIPTEN_KEEPALIVE {pe.ctype} mjwf_{pe.base_name}_ptr(int h);\n"


def generate_dim_decl(de: DimExport) -> str:
    return f"EMSCRIPTEN_KEEPALIVE int mjwf_{de.base_name}(int h);\n"


def generate_derived_decl(de: DerivedExport) -> str:
    ctype = CTYPE_BY_DTYPE[de.dtype]
    return f"EMSCRIPTEN_KEEPALIVE {ctype} mjwf_{de.base_name}_ptr(int h);\n"


def generate_pointer_impl(pe: PointerExport) -> str:
    owner = "m" if pe.struct == "mjModel" else "d"
    other = "d" if owner == "m" else "m"
    # Scalar fields expose a 1-element pointer view; pointer/array fields
    # return the underlying C pointer directly.
    if pe.is_scalar:
        ptr_expr = f"&{owner}->{pe.field}"
    else:
        ptr_expr = f"{owner}->{pe.field}"
    lines = [
        f"EMSCRIPTEN_KEEPALIVE {pe.ctype} mjwf_{pe.base_name}_ptr(int h) {{",
        "  if (!mjwf_helper_valid(h)) return NULL;",
        "  mjModel* m = _mjwf_model_of(h);",
        "  mjData* d  = _mjwf_data_of(h);",
        f"  (void){other};",
        f"  if (!{owner}) return NULL;",
        f"  return ({pe.ctype})({ptr_expr});",
        "}",
        "",
    ]
    return "\n".join(lines)


def generate_dim_impl(de: DimExport) -> str:
    owner = "m" if de.struct == "mjModel" else "d"
    other = "d" if owner == "m" else "m"
    lines = [
        f"EMSCRIPTEN_KEEPALIVE int mjwf_{de.base_name}(int h) {{",
        "  if (!mjwf_helper_valid(h)) return 0;",
        "  mjModel* m = _mjwf_model_of(h);",
        "  mjData* d  = _mjwf_data_of(h);",
        f"  (void){other};",
        f"  if (!{owner}) return 0;",
        f"  return (int)({owner}->{de.field});",
        "}",
        "",
    ]
    return "\n".join(lines)


def generate_derived_impl(de: DerivedExport) -> str:
    """Expose a direct pointer view into the underlying C struct data.

    For nested structs (mjModel.opt / vis / stat, mjData.contact / solver /
    timer / warning) we deliberately avoid snapshot buffers here and instead
    return a pointer into the true mjModel/mjData memory so that JS-side
    code can read and write with minimal overhead.
    """
    ctype = CTYPE_BY_DTYPE[de.dtype]
    owner = "m" if de.owner_struct == "mjModel" else "d"
    other = "d" if owner == "m" else "m"
    # Base expression: single structs use owner->field, arrays use owner->field
    # as well (pointer-to-struct). We then take the address of the first
    # element's subfield so that the layout is a flat C array that JS can
    # wrap with a TypedArray.
    if de.base_is_pointer:
        # Pointer to struct array: owner->field is already a {target_struct}*.
        base_expr = f"{owner}->{de.base_field}"
        # First element in the array.
        first_obj = f"{base_expr}[0]"
    else:
        # Single nested struct stored by value on the owning struct.
        first_obj = f"{owner}->{de.base_field}"

    # Subfield path may already contain dots (for anonymous structs etc.).
    # For scalar fields we take &first_obj.subfield; for array fields we
    # take &first_obj.subfield[0]. We use width_expr to distinguish between
    # scalar ("1") and array cases.
    if de.width_expr == "1":
        field_expr = f"{first_obj}.{de.subfield}"
    else:
        field_expr = f"{first_obj}.{de.subfield}[0]"

    ptr_expr = f"&({field_expr})"

    lines: List[str] = [
        f"EMSCRIPTEN_KEEPALIVE {ctype} mjwf_{de.base_name}_ptr(int h) {{",
        "  if (!mjwf_helper_valid(h)) return NULL;",
        "  mjModel* m = _mjwf_model_of(h);",
        "  mjData* d  = _mjwf_data_of(h);",
        f"  (void){other};",
        f"  if (!{owner}) return NULL;",
        f"  return ({ctype})({ptr_expr});",
        "}",
        "",
    ]

    return "\n".join(lines)


def main(argv: List[str]) -> int:
    if len(argv) != 3:
        print("Usage: gen_structs.py <out.h> <out.c>", file=sys.stderr)
        return 2

    out_h = Path(argv[1]).resolve()
    out_c = Path(argv[2]).resolve()

    repo_root = _repo_root()
    # official_app_337 is tied to MuJoCo 3.3.7; keep the path explicit here.
    abi_structs = repo_root / "dist" / "3.3.7" / "abi" / "structs_introspect_like.json"
    if not abi_structs.is_file():
        raise SystemExit(f"structs_introspect_like.json not found at {abi_structs}")

    structs = load_structs_introspect(abi_structs)
    ptr_exports = collect_pointer_exports(structs)
    dim_exports = collect_dim_exports(structs)
    derived_exports = collect_derived_exports(structs)

    # -----------------------------------------------------------------------
    # Emit header
    # -----------------------------------------------------------------------
    out_h.parent.mkdir(parents=True, exist_ok=True)
    with out_h.open("w", encoding="utf-8") as fh:
        fh.write(HDR_PREAMBLE)
        for pe in ptr_exports:
            fh.write(generate_pointer_decl(pe))
        for de in dim_exports:
            fh.write(generate_dim_decl(de))
        for drv in derived_exports:
            fh.write(generate_derived_decl(drv))
        fh.write(HDR_POST)

    # -----------------------------------------------------------------------
    # Emit source
    # -----------------------------------------------------------------------
    out_c.parent.mkdir(parents=True, exist_ok=True)
    with out_c.open("w", encoding="utf-8") as fc:
        fc.write(SRC_PREAMBLE)
        for pe in ptr_exports:
            fc.write(generate_pointer_impl(pe))
        for de in dim_exports:
            fc.write(generate_dim_impl(de))
        for drv in derived_exports:
            fc.write(generate_derived_impl(drv))

    # Extra exports list: all exported symbol base names (ptr + dims).
    extra_names: List[str] = []
    for pe in ptr_exports:
        extra_names.append(f"{pe.base_name}_ptr")
    for de in dim_exports:
        extra_names.append(de.base_name)
    for drv in derived_exports:
        extra_names.append(f"{drv.base_name}_ptr")

    extra_path = out_h.parent / "mjwf_abi_structs.lst"
    with extra_path.open("w", encoding="utf-8") as fe:
        for name in sorted(set(extra_names)):
            fe.write(f"{name}\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
