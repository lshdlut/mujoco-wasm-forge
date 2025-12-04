#!/usr/bin/env python3
import json
import os
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Set, Tuple

import yaml

HDR_PREAMBLE = """
// AUTO-GENERATED. Do not edit by hand. See codegen/spec_337.yaml
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

SRC_PREAMBLE = """
// AUTO-GENERATED. Do not edit by hand. See codegen/spec_337.yaml
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

// We can't access g_pool from here; expose small inline accessors via declarations.
// For simplicity in this generator, we re-declare pointer getters here and forward
// to mjwf_handles.c via mjwf_helper_valid and direct field access using a trick: define
// lightweight accessors in this TU using weak symbols to be resolved by linker.
// Getters implemented in mjwf_handles.c
extern mjModel* _mjwf_model_of(int h);
extern mjData*  _mjwf_data_of(int h);
""".lstrip()

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
}

STRUCT_PREFIX = {
    "mjModel": "model",
    "mjData": "data",
}


def sanitize_segment(segment: str) -> str:
    cleaned = re.sub(r"[^0-9A-Za-z_]+", "_", segment)
    cleaned = re.sub(r"_+", "_", cleaned)
    return cleaned.strip("_")


def default_pointer_name(struct: str, field: str) -> str:
    prefix = STRUCT_PREFIX.get(struct, sanitize_segment(struct))
    cleaned_field = sanitize_segment(field)
    if cleaned_field:
        return f"{prefix}_{cleaned_field}"
    return prefix


def pointer_needs_address(field: "FieldDef") -> bool:
    return "*" not in field.ctype and not field.array_dims


class PointerRegistry:
    def __init__(self) -> None:
        self.pointers: List["PointerExport"] = []
        self.used_names: Dict[str, "PointerExport"] = {}
        self.matched: Dict[str, List[str]] = {"mjModel": [], "mjData": []}

    def register(self, ptr: "PointerExport") -> None:
        if ptr.name in self.used_names:
            return
        self.pointers.append(ptr)
        self.used_names[ptr.name] = ptr
        self.matched.setdefault(ptr.struct, []).append(ptr.field)

@dataclass
class FieldDef:
    struct: str
    name: str
    ctype: str
    base_type: str
    comment: Optional[str]
    line: int
    array_dims: List[str] = field(default_factory=list)
    nested_fields: Dict[str, "FieldDef"] = field(default_factory=dict)

@dataclass
class PointerExport:
    name: str
    struct: str
    field: str
    dtype: str
    ctype: str
    len_expr: Optional[str]
    rule_id: str
    address_of: bool = False

@dataclass
class CountExport:
    name: str
    struct: str
    target_field: str
    dim_name: Optional[str]
    mode: str  # currently only "sum"
    rule_id: str
    base_name: str = ""
    func_name: Optional[str] = None


@dataclass
class DimExport:
    name: str
    func_name: str
    struct: str
    expr: str
    base_name: str

@dataclass
class DerivedExport:
    name: str               # e.g. data_contact_pos
    owner: str              # "mjModel" or "mjData"
    base_field: str         # e.g. "contact"
    target_struct: str      # e.g. "mjContact"
    subfield: str           # e.g. "pos", "frame", "geom1", ...
    dtype: str              # f64/f32/i32/u8
    width_expr: str         # number of elements per item (expression string)
    is_array: bool          # true if subfield is an array (needs inner loop)
    need_expr: str          # e.g. "m->nconmax*3"
    count_expr: str         # e.g. "d->ncon"
    rule_id: str

def load_text(path: Path) -> str:
    with open(path, "r", encoding="utf-8") as fh:
        return fh.read()

def strip_comments(src: str) -> str:
    # Remove block comments
    src = re.sub(r"/\*[\s\S]*?\*/", "", src)
    # Remove line comments
    src = re.sub(r"//.*", "", src)
    return src

def find_struct_block(src: str, struct_name: str) -> Tuple[str, int]:
    pattern = re.compile(
        rf"(?:typedef\s+)?struct\s+{re.escape(struct_name)}_?\s*\{{", re.MULTILINE
    )
    match = pattern.search(src)
    if not match:
        raise RuntimeError(f"Could not find struct definition for {struct_name}")
    start = match.end()
    depth = 1
    i = start
    while i < len(src) and depth > 0:
        if src[i] == "{":
            depth += 1
        elif src[i] == "}":
            depth -= 1
        i += 1
    block = src[start : i - 1]
    return block, src[: start].count("\n") + 1

def iter_declarations(block: str) -> Sequence[str]:
    segments: List[str] = []
    acc: List[str] = []
    i = 0
    in_line_comment = False
    in_block_comment = False
    pending_flush = False
    length = len(block)
    brace_depth = 0
    while i < length:
        ch = block[i]
        nxt = block[i + 1] if i + 1 < length else ""
        if in_line_comment:
            acc.append(ch)
            if ch == "\n":
                in_line_comment = False
                if pending_flush:
                    segments.append("".join(acc))
                    acc = []
                    pending_flush = False
            i += 1
            continue
        if in_block_comment:
            acc.append(ch)
            if ch == "*" and nxt == "/":
                acc.append(nxt)
                in_block_comment = False
                i += 2
            else:
                i += 1
            continue
        if ch == "/" and nxt == "/":
            acc.append(ch)
            acc.append(nxt)
            in_line_comment = True
            i += 2
            continue
        if ch == "/" and nxt == "*":
            acc.append(ch)
            acc.append(nxt)
            in_block_comment = True
            i += 2
            continue
        acc.append(ch)
        if ch == "{":
            brace_depth += 1
        elif ch == "}":
            brace_depth = max(0, brace_depth - 1)
        if ch == ";":
            if brace_depth == 0:
                pending_flush = True
            i += 1
            continue
        if pending_flush and ch == "\n" and brace_depth == 0:
            segments.append("".join(acc))
            acc = []
            pending_flush = False
            i += 1
            continue
        i += 1
    if acc:
        segments.append("".join(acc))
    return segments

def load_structs_from_scan(path: Path) -> Dict[str, Dict[str, FieldDef]]:
    data = json.loads(path.read_text(encoding="utf-8"))
    structs: Dict[str, Dict[str, FieldDef]] = {}
    for struct_name, struct_data in data.get("structs", {}).items():
        fields: Dict[str, FieldDef] = {}
        for entry in struct_data.get("fields", []):
            ctype = (entry.get("type") or "").strip()
            if "*" not in ctype:
                continue
            base = ctype.replace("const", " ").replace("*", " ").strip()
            base = re.sub(r"\s+", " ", base)
            name = entry.get("name")
            if not name:
                continue
            comment = entry.get("comment")
            line = entry.get("line", -1)
            array_dims = list(entry.get("array") or [])
            fields[name] = FieldDef(
                struct=struct_name,
                name=name,
                ctype=ctype,
                base_type=base,
                comment=comment,
                line=line,
                array_dims=array_dims,
            )
        structs[struct_name] = fields
    return structs

def load_dim_hints(path: Path) -> Dict[Tuple[str, str], List[str]]:
    data = json.loads(path.read_text(encoding="utf-8"))
    hints: Dict[Tuple[str, str], List[str]] = {}
    for dim, entries in data.get("dim_map", {}).items():
        for entry in entries or []:
            struct = entry.get("struct")
            field = entry.get("field")
            if not struct or not field:
                continue
            key = (struct, field)
            if key not in hints:
                hints[key] = []
            if dim not in hints[key]:
                hints[key].append(dim)
    return hints

def parse_struct_block(block: str, struct_name: str, base_line: int) -> Dict[str, FieldDef]:
    fields: Dict[str, FieldDef] = {}
    search_pos = 0
    for raw in iter_declarations(block):
        segment = raw.strip()
        if not segment:
            continue
        decl_clean = re.sub(r"/\*[\s\S]*?\*/", "", raw)
        decl_clean = re.sub(r"//.*", "", decl_clean)
        decl_lines = [ln.rstrip().strip() for ln in decl_clean.splitlines() if ln.strip()]
        if not decl_lines:
            continue
        start_idx = block.find(raw, search_pos)
        if start_idx == -1:
            start_idx = search_pos
        line_no = base_line + block[:start_idx].count("\n")
        search_pos = start_idx + len(raw) + 1
        comment_parts: List[str] = []
        for line in raw.splitlines():
            idx = line.find("//")
            if idx != -1:
                comment_parts.append(line[idx + 2 :].strip())
        comment = " ".join(comment_parts).strip() or None

        first_line = decl_lines[0]
        if first_line.startswith("struct"):
            tail_line = decl_lines[-1]
            tail_match = re.search(r"}\s*([A-Za-z0-9_]+)((?:\[[^\]]+\])*)\s*;?$", tail_line)
            if not tail_match:
                continue
            name = tail_match.group(1)
            array_part = tail_match.group(2) or ""
            array_dims: List[str] = []
            for dim in re.findall(r"\[([^\]]+)\]", array_part):
                dim_clean = dim.strip()
                if dim_clean:
                    array_dims.append(dim_clean)
            brace_start = raw.find("{")
            brace_end = raw.rfind("}")
            if brace_start == -1 or brace_end == -1 or brace_end <= brace_start:
                continue
            inner_block = raw[brace_start + 1 : brace_end]
            nested_struct_name = f"{struct_name}.{name}"
            nested_fields = parse_struct_block(inner_block, nested_struct_name, line_no + 1)
            fields[name] = FieldDef(
                struct=struct_name,
                name=name,
                ctype="struct",
                base_type=nested_struct_name,
                comment=comment,
                line=line_no,
                array_dims=array_dims,
                nested_fields=nested_fields,
            )
            continue

        decl_line = decl_lines[0].rstrip(";").strip()
        m = re.match(r"([A-Za-z0-9_\s\*]+?)\s+([A-Za-z0-9_]+)((?:\[[^\]]+\])*)", decl_line)
        if not m:
            continue
        ctype = m.group(1).strip()
        name = m.group(2)
        base = ctype.replace("const", " ").replace("*", " ").strip()
        base = re.sub(r"\s+", " ", base)
        array_part = m.group(3) or ""
        array_dims: List[str] = []
        if array_part:
            for dim in re.findall(r"\[([^\]]+)\]", array_part):
                dim_clean = dim.strip()
                if dim_clean:
                    array_dims.append(dim_clean)
        fields[name] = FieldDef(
            struct=struct_name,
            name=name,
            ctype=ctype,
            base_type=base,
            comment=comment,
            line=line_no,
            array_dims=array_dims,
        )
    return fields

def parse_fields(src: str, struct_name: str) -> Dict[str, FieldDef]:
    block, base_line = find_struct_block(src, struct_name)
    return parse_struct_block(block, struct_name, base_line)

def resolve_header_path(spec_dir: Path, search_roots: Sequence[str], header: str) -> Path:
    for root in search_roots:
        candidate = (spec_dir / root).resolve() / header
        if candidate.is_file():
            return candidate
    raise RuntimeError(f"Unable to locate header {header} in search paths: {search_roots}")

def normalize_dims(dims_section) -> Tuple[Dict[str, str], List[Tuple[str, str]]]:
    if not dims_section:
        return {}, []
    if isinstance(dims_section, dict):
        items = list(dims_section.items())
    else:
        items: List[Tuple[str, str]] = []
        for entry in dims_section:
            if isinstance(entry, dict):
                items.extend(entry.items())
    merged: Dict[str, str] = {}
    ordered: List[Tuple[str, str]] = []
    for key, value in items:
        ordered.append((key, value))
        merged[key] = value
        merged[key.lower()] = value
    return merged, ordered

def normalize_counts(counts_section) -> List[Dict[str, str]]:
    if not counts_section:
        return []
    if isinstance(counts_section, dict):
        return [counts_section]
    return list(counts_section)

def compose_export_basename(struct: str, identifier: str) -> str:
    prefix = STRUCT_PREFIX.get(struct, sanitize_segment(struct))
    suffix = sanitize_segment(identifier)
    return f"{prefix}_{suffix}" if suffix else prefix

def infer_struct_from_expr(expr: str, name: str) -> str:
    has_model = bool(re.search(r"\bm->", expr))
    has_data = bool(re.search(r"\bd->", expr))
    if has_model and has_data:
        raise RuntimeError(f"Ambiguous owner for dim '{name}' (expr={expr})")
    if has_model:
        return "mjModel"
    if has_data:
        return "mjData"
    raise RuntimeError(f"Unable to infer owner for dim '{name}' (expr={expr})")

def dtype_to_ctype(dtype: str, default: str) -> str:
    if dtype in CTYPE_BY_DTYPE:
        return CTYPE_BY_DTYPE[dtype]
    return default

def dtype_to_basetype(dtype: str, default: str) -> str:
    if dtype in BASETYPE_BY_DTYPE:
        return BASETYPE_BY_DTYPE[dtype]
    return default

def infer_dtype_for_field(field: FieldDef, requested: Optional[str]) -> Optional[str]:
    if requested and requested != "auto":
        return requested
    base = field.base_type.strip()
    base = re.sub(r"\s+", " ", base)
    candidates = [
        base,
        base.replace("const ", ""),
        base.replace("const", "").strip(),
    ]
    for cand in candidates:
        if cand in AUTO_DTYPE_BY_BASE:
            return AUTO_DTYPE_BY_BASE[cand]
    return None

def parse_comment_length(field: FieldDef, dims: Dict[str, str]) -> Optional[str]:
    comment = field.comment or ""
    matches = re.findall(r"\(([^)]+)\)", comment)
    for expr_raw in reversed(matches):
        expr_norm = expr_raw.replace("×", "x").replace("X", "x").replace("·", "*")
        parts = re.split(r"[x\*]", expr_norm)
        tokens: List[str] = []
        valid = True
        for part in parts:
            token = part.strip()
            if not token:
                continue
            token = re.sub(r'[^\w]', '', token)
            if not token:
                continue
            if token.isdigit():
                tokens.append(token)
                continue
            lower = token.lower()
            if lower in dims:
                tokens.append(lower)
                continue
            if token in dims:
                tokens.append(token)
                continue
            valid = False
            break
        if valid and tokens:
            return "*".join(tokens)
    return None

def generate_pointer_decl(ptr: PointerExport) -> str:
    return f"EMSCRIPTEN_KEEPALIVE {ptr.ctype} mjwf_{ptr.name}_ptr(int h);\n"

def generate_int_decl(func_name: str) -> str:
    return f"EMSCRIPTEN_KEEPALIVE int {func_name}(int h);\n"

def needs_model(expr: str) -> bool:
    return "m->" in expr

def needs_data(expr: str) -> bool:
    return "d->" in expr

def generate_pointer_impl(ptr: PointerExport) -> str:
    owner = "m" if ptr.struct == "mjModel" else "d"
    field_expr = f"{owner}->{ptr.field}"
    void_line = "  (void)d;\n" if owner == "m" else "  (void)m;\n"
    if ptr.address_of:
        field_expr = f"&({field_expr})"
    return (
        f"EMSCRIPTEN_KEEPALIVE {ptr.ctype} mjwf_{ptr.name}_ptr(int h) {{\n"
        f"  if (!mjwf_helper_valid(h)) return NULL;\n"
        f"  mjModel* m = _mjwf_model_of(h);\n"
        f"  mjData* d  = _mjwf_data_of(h);\n"
        f"{void_line}"
        f"  return {owner} ? ({ptr.ctype})({field_expr}) : NULL;\n"
        f"}}\n\n"
    )

def generate_derived_decl(der: DerivedExport) -> str:
    ctype = dtype_to_ctype(der.dtype, "void*")
    return f"EMSCRIPTEN_KEEPALIVE {ctype} mjwf_{der.name}_ptr(int h);\n"

def generate_derived_impl(der: DerivedExport) -> str:
    owner = "m" if der.owner == "mjModel" else "d"
    other = "d" if owner == "m" else "m"
    ctype = dtype_to_ctype(der.dtype, "void*")
    basetype = dtype_to_basetype(der.dtype, "int32_t")
    zero_value = "0"
    if basetype in ("double", "float"):
        zero_value = "0.0"
    width_expr = der.width_expr or "1"
    base_access = f"{owner}->{der.base_field}"
    lines: List[str] = [
        f"EMSCRIPTEN_KEEPALIVE {ctype} mjwf_{der.name}_ptr(int h) {{",
        "  if (!mjwf_helper_valid(h)) return NULL;",
        "  mjModel* m = _mjwf_model_of(h);",
        "  mjData* d  = _mjwf_data_of(h);",
        f"  (void){other};",
        f"  if (!{owner}) return NULL;",
        f"  const {der.target_struct}* base = {base_access};",
        "  if (!base) return NULL;",
        f"  const int stride = ({width_expr});",
        "  if (stride <= 0) return NULL;",
        f"  int need = (int)({der.need_expr});",
        "  if (need <= 0) return NULL;",
        f"  static {basetype}* buf = NULL;",
        "  static int cap = 0;",
        f"  if (cap < need) {{ if (buf) free(buf); buf = ({basetype}*)malloc(sizeof({basetype}) * need); cap = buf ? need : 0; }}",
        "  if (!buf) return NULL;",
        f"  int n = (int)({der.count_expr});",
        "  if (n < 0) n = 0;",
        "  int max_items = (stride > 0) ? (need / stride) : 0;",
        "  if (n > max_items) n = max_items;",
        "  for (int i = 0; i < n; ++i) {",
        f"    const {der.target_struct}* c = base + i;",
        "    int off = i * stride;",
    ]
    if der.is_array:
        lines.extend([
            "    for (int j = 0; j < stride; ++j) {",
            f"      buf[off + j] = ({basetype})c->{der.subfield}[j];",
            "    }",
        ])
    else:
        lines.append(f"    buf[off] = ({basetype})c->{der.subfield};")
    lines.extend([
        "  }",
        f"  for (int k = n * stride; k < need; ++k) buf[k] = {zero_value};",
        f"  return ({ctype})buf;",
        "}\n",
    ])
    return "\n".join(lines)

def generate_dim_impl(dim: DimExport) -> str:
    func_name = dim.func_name
    expr = dim.expr
    need_m = needs_model(expr)
    need_d = needs_data(expr)
    guards = []
    if need_m:
        guards.append("!m")
    if need_d:
        guards.append("!d")
    guard_line = ""
    if guards:
        guard_line = f"  if ({' || '.join(guards)}) return 0;\n"
    unused_lines = ""
    if not need_m:
        unused_lines += "  (void)m;\n"
    if not need_d:
        unused_lines += "  (void)d;\n"
    return (
        f"EMSCRIPTEN_KEEPALIVE int {func_name}(int h) {{\n"
        f"  if (!mjwf_helper_valid(h)) return 0;\n"
        f"  mjModel* m = _mjwf_model_of(h);\n"
        f"  mjData* d  = _mjwf_data_of(h);\n"
        f"{guard_line}{unused_lines}  return (int)({expr});\n"
        f"}}\n\n"
    )

def generate_count_impl(count: CountExport, dims: Dict[str, str], field_defs: Dict[str, Dict[str, FieldDef]]) -> str:
    if count.mode != "sum":
        raise RuntimeError(f"Unsupported count mode: {count.mode}")
    owner = "m" if count.struct == "mjModel" else "d"
    dim_expr = dims.get(count.dim_name, None) if count.dim_name else None
    if not dim_expr:
        raise RuntimeError(f"Count {count.name} requires a valid dim reference; missing dim {count.dim_name!r}")
    field = field_defs[count.struct].get(count.target_field)
    if not field:
        raise RuntimeError(f"Count {count.name} references unknown field {count.target_field}")
    basetype = dtype_to_basetype("i32", field.ctype.replace("*", "").strip())
    unused_line = "  (void)d;\n" if owner == "m" else "  (void)m;\n"
    return (
        f"EMSCRIPTEN_KEEPALIVE int {count.func_name}(int h) {{\n"
        f"  if (!mjwf_helper_valid(h)) return 0;\n"
        f"  mjModel* m = _mjwf_model_of(h);\n"
        f"  mjData* d  = _mjwf_data_of(h);\n"
        f"{unused_line}"
        f"  if (!{owner}) return 0;\n"
        f"  {basetype}* arr = {owner}->{count.target_field};\n"
        f"  if (!arr) return 0;\n"
        f"  int total = 0;\n"
        f"  int limit = (int)({dim_expr});\n"
        f"  for (int i = 0; i < limit; ++i) total += arr[i];\n"
        f"  return total;\n"
        f"}}\n\n"
    )

def compile_rules(spec_rules, auto_rules, field_map, dims, dim_hints, registry: PointerRegistry) -> List[str]:
    diagnostics: List[str] = []

    for idx, rule in enumerate(spec_rules):
        struct = rule.get("struct")
        if struct not in field_map:
            raise RuntimeError(f"Rule {idx} targets unknown struct {struct}")
        pattern = rule.get("match")
        if not pattern:
            raise RuntimeError(f"Rule {idx} missing 'match'")
        regex = re.compile(pattern)
        requested_dtype = rule.get("dtype")
        len_expr_rule = rule.get("len")
        use_comment_len = bool(rule.get("len_from_comment"))
        export_count = rule.get("export_count")
        rule_id = rule.get("id") or f"rule_{idx}"
        for fname, fdef in field_map[struct].items():
            if not regex.search(fname):
                continue
            dtype = infer_dtype_for_field(fdef, requested_dtype)
            if not dtype:
                diagnostics.append(f"[{rule_id}] Unable to infer dtype for field {fname} (ctype={fdef.ctype})")
                continue
            len_expr = len_expr_rule
            if not len_expr and use_comment_len:
                inferred = parse_comment_length(fdef, dims)
                if inferred:
                    len_expr = inferred
                else:
                    diagnostics.append(f"[{rule_id}] Unable to infer length for field {fname} from comment")
            if not len_expr:
                dims_for_field = dim_hints.get((struct, fname)) if dim_hints else None
                if dims_for_field:
                    len_expr = dims_for_field[0]
            pointer_name = rule.get("export_name") or fname
            ctype = dtype_to_ctype(dtype, fdef.ctype.strip())
            pointer_name = rule.get("export_name") or default_pointer_name(struct, fname)
            registry.register(PointerExport(
                name=pointer_name,
                struct=struct,
                field=fname,
                dtype=dtype,
                ctype=ctype,
                len_expr=len_expr,
                rule_id=rule_id,
                address_of=pointer_needs_address(fdef),
            ))
            if export_count:
                pass

    for idx, rule in enumerate(auto_rules):
        struct = rule.get("struct")
        if struct not in field_map:
            raise RuntimeError(f"Auto rule {idx} targets unknown struct {struct}")
        include_re = re.compile(rule.get("match", ".*"))
        exclude_pat = rule.get("exclude")
        exclude_re = re.compile(exclude_pat) if exclude_pat else None
        allow_types = set(rule.get("allow_types", []))
        requested_dtype = rule.get("dtype")
        use_comment_len = bool(rule.get("len_from_comment", True))
        len_expr_rule = rule.get("len")
        rule_id = rule.get("id") or f"auto_{idx}"
        for fname, fdef in field_map[struct].items():
            if fname in registry.matched.get(struct, []):
                continue
            if not include_re.search(fname):
                continue
            if exclude_re and exclude_re.search(fname):
                continue
            if allow_types and fdef.base_type not in allow_types:
                continue
            dtype = infer_dtype_for_field(fdef, requested_dtype)
            if not dtype:
                diagnostics.append(f"[{rule_id}] Unable to infer dtype for field {fname} (ctype={fdef.ctype})")
                continue
            len_expr = len_expr_rule
            if not len_expr and use_comment_len:
                inferred = parse_comment_length(fdef, dims)
                if inferred:
                    len_expr = inferred
                else:
                    diagnostics.append(f"[{rule_id}] Unable to infer length for field {fname} from comment")
            if not len_expr:
                dims_for_field = dim_hints.get((struct, fname)) if dim_hints else None
                if dims_for_field:
                    len_expr = dims_for_field[0]
            pointer_name = rule.get("export_name") or default_pointer_name(struct, fname)
            ctype = dtype_to_ctype(dtype, fdef.ctype.strip())
            registry.register(PointerExport(
                name=pointer_name,
                struct=struct,
                field=fname,
                dtype=dtype,
                ctype=ctype,
                len_expr=len_expr,
                rule_id=rule_id,
                address_of=pointer_needs_address(fdef),
            ))
    return diagnostics

def compute_skipped(field_map, matched) -> Dict[str, List[str]]:
    out: Dict[str, List[str]] = {}
    for struct, fields in field_map.items():
        out[struct] = []
        for fname in sorted(fields.keys()):
            if fname in matched.get(struct, []):
                continue
            out[struct].append(fname)
    return out

def write_report(report_path: Optional[Path], data: dict) -> None:
    if not report_path:
        return
    report_path.parent.mkdir(parents=True, exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as fh:
        json.dump(data, fh, indent=2, sort_keys=True)
        fh.write("\n")

def main() -> int:
    if len(sys.argv) != 4:
        print("Usage: gen_exports.py <spec.yaml> <out.h> <out.c>")
        return 2

    spec_path, out_h, out_c = sys.argv[1:]
    spec_path = Path(spec_path).resolve()
    out_h = Path(out_h).resolve()
    out_c = Path(out_c).resolve()

    spec = yaml.safe_load(spec_path.read_text(encoding="utf-8"))

    spec_dir = spec_path.parent
    headers_cfg = spec.get("headers", {})
    search_roots = headers_cfg.get("search", [
        "../../external/mujoco/include/mujoco",
        "../../local_tools/external/mujoco/include/mujoco",
    ])
    mjmodel_header = headers_cfg.get("mjModel", "mjmodel.h")
    mjdata_header = headers_cfg.get("mjData", "mjdata.h")
    report_json = headers_cfg.get("report_json")
    struct_headers = headers_cfg.get("struct_headers", {})

    header_model_path = resolve_header_path(spec_dir, search_roots, mjmodel_header)
    header_data_path = resolve_header_path(spec_dir, search_roots, mjdata_header)

    scan_cfg = spec.get("scan", {}) or {}
    structs_from_scan: Dict[str, Dict[str, FieldDef]] = {}
    dim_hints: Dict[Tuple[str, str], List[str]] = {}
    structs_path = scan_cfg.get("structs")
    if structs_path:
        scan_structs_path = (spec_dir / structs_path).resolve()
        if scan_structs_path.is_file():
            structs_from_scan = load_structs_from_scan(scan_structs_path)
    dim_map_path = scan_cfg.get("dim_map")
    if dim_map_path:
        scan_dim_path = (spec_dir / dim_map_path).resolve()
        if scan_dim_path.is_file():
            dim_hints = load_dim_hints(scan_dim_path)

    # Build two maps: scan-based (may be broader) and header-based (ground truth)
    scan_model_fields = structs_from_scan.get("mjModel") or {}
    scan_data_fields = structs_from_scan.get("mjData") or {}
    header_model_fields = parse_fields(load_text(header_model_path), "mjModel")
    header_data_fields = parse_fields(load_text(header_data_path), "mjData")

    # Prefer scan-derived type/len hints but cross-validate against header names
    def reconcile_fields(scan: Dict[str, FieldDef], header: Dict[str, FieldDef]) -> Dict[str, FieldDef]:
        out: Dict[str, FieldDef] = {}
        for name, sdef in scan.items():
            if name in header:
                hdef = header[name]
                if hdef.nested_fields and not sdef.nested_fields:
                    sdef.nested_fields = hdef.nested_fields
                out[name] = sdef
        # Include any header-only pointer fields as well
        for name, hdef in header.items():
            out.setdefault(name, hdef)
        return out

    model_fields = reconcile_fields(scan_model_fields, header_model_fields)
    data_fields = reconcile_fields(scan_data_fields, header_data_fields)

    field_map = {"mjModel": model_fields, "mjData": data_fields}
    header_field_map = {"mjModel": header_model_fields, "mjData": header_data_fields}
    struct_cache: Dict[str, Dict[str, FieldDef]] = {
        "mjModel": model_fields,
        "mjData": data_fields,
    }
    visited_struct_paths: Set[Tuple[str, Tuple[str, ...]]] = set()

    def resolve_struct_header(struct_name: str, owner_struct: str) -> Path:
        header_override = struct_headers.get(struct_name)
        if header_override:
            return resolve_header_path(spec_dir, search_roots, header_override)
        if owner_struct == "mjModel":
            return header_model_path
        if owner_struct == "mjData":
            return header_data_path
        return header_model_path

    def get_struct_fields(struct_name: str, owner_struct: str) -> Dict[str, FieldDef]:
        if struct_name in struct_cache:
            return struct_cache[struct_name]
        header_path = resolve_struct_header(struct_name, owner_struct)
        struct_cache[struct_name] = parse_fields(load_text(header_path), struct_name)
        return struct_cache[struct_name]

    def expand_struct(
        owner_struct: str,
        field_path: List[str],
        struct_name: str,
        prefix_parts: List[str],
        rule_id: str,
        preloaded_fields: Optional[Dict[str, FieldDef]] = None,
    ) -> None:
        key = (struct_name, tuple(prefix_parts))
        if key in visited_struct_paths:
            return
        visited_struct_paths.add(key)
        nested_fields = preloaded_fields if preloaded_fields is not None else get_struct_fields(struct_name, owner_struct)
        for sub_name, sub_def in nested_fields.items():
            next_field_path = field_path + [sub_name]
            next_prefix_parts = prefix_parts + [sub_name]
            field_expr = ".".join(next_field_path)
            name_key = "_".join(next_prefix_parts)
            dtype = infer_dtype_for_field(sub_def, None)
            base_type = sub_def.base_type
            is_struct_type = (
                bool(sub_def.nested_fields)
                or (
                    "*" not in sub_def.ctype
                    and not sub_def.array_dims
                    and base_type.startswith("mj")
                    and len(base_type) > 2
                    and base_type[2].isupper()
                )
            )
            len_expr = None
            if sub_def.array_dims:
                len_expr = "*".join(sub_def.array_dims)
            if dtype:
                registry.register(PointerExport(
                    name=default_pointer_name(owner_struct, name_key),
                    struct=owner_struct,
                    field=field_expr,
                    dtype=dtype,
                    ctype=dtype_to_ctype(dtype, sub_def.ctype.strip() or sub_def.base_type),
                    len_expr=len_expr,
                    rule_id=f"{rule_id}:{sub_name}",
                    address_of=pointer_needs_address(sub_def),
                ))
            elif not is_struct_type:
                diagnostics.append(
                    f"[{rule_id}] Unable to infer dtype for nested field {struct_name}.{sub_name} (ctype={sub_def.ctype})"
                )
            if is_struct_type:
                if sub_def.nested_fields:
                    nested_name = base_type or f"{struct_name}.{sub_name}"
                    expand_struct(owner_struct, next_field_path, nested_name, next_prefix_parts, rule_id, sub_def.nested_fields)
                else:
                    expand_struct(owner_struct, next_field_path, base_type, next_prefix_parts, rule_id)

    dims_lookup, dim_entries = normalize_dims(spec.get("dims"))
    dim_exports: List[DimExport] = []
    for dim_name, expr in dim_entries:
        struct_name = infer_struct_from_expr(expr, dim_name)
        func_basename = compose_export_basename(struct_name, dim_name)
        dim_exports.append(DimExport(
            name=dim_name,
            func_name=f"mjwf_{func_basename}",
            struct=struct_name,
            expr=expr,
            base_name=func_basename,
        ))
    dims = dims_lookup
    counts_cfg = normalize_counts(spec.get("counts"))
    auto_rules = spec.get("auto", [])
    rules = spec.get("rules", [])

    diagnostics: List[str] = []
    registry = PointerRegistry()
    diagnostics.extend(compile_rules(rules, auto_rules, field_map, dims, dim_hints, registry))

    expand_cfg = spec.get("expand", [])
    for entry in expand_cfg or []:
        owner = entry.get("owner")
        field = entry.get("field")
        if not owner or not field:
            diagnostics.append(f"[expand] Invalid entry missing owner/field: {entry}")
            continue
        if owner not in field_map:
            diagnostics.append(f"[expand] Unknown owner struct {owner}")
            continue
        owner_fields = header_field_map[owner]
        if field not in owner_fields:
            diagnostics.append(f"[expand] Owner {owner} has no field '{field}'")
            continue
        base_field = owner_fields[field]
        target_struct = entry.get("struct") or base_field.base_type
        if not target_struct:
            diagnostics.append(f"[expand] Unable to determine target struct for {owner}.{field}")
            continue
        prefix = entry.get("prefix")
        prefix_parts = [prefix] if prefix else [field]
        rule_id = entry.get("id") or f"expand_{owner}_{field}"
        expand_struct(owner, [field], target_struct, prefix_parts, rule_id)

    pointers = registry.pointers
    skipped = compute_skipped(field_map, registry.matched)

    count_exports: List[CountExport] = []
    for entry in counts_cfg:
        name = entry.get("name")
        struct = entry.get("struct")
        mode = entry.get("mode", "sum")
        target_field = entry.get("field")
        dim_name = entry.get("dim")
        if not name or not struct or not target_field:
            raise RuntimeError(f"Invalid count entry: {entry}")
        rule_id = entry.get("id") or f"count_{name}"
        func_basename = compose_export_basename(struct, name)
        count_exports.append(CountExport(
            name=name,
            struct=struct,
            target_field=target_field,
            dim_name=dim_name,
            mode=mode,
            rule_id=rule_id,
            base_name=func_basename,
            func_name=f"mjwf_{func_basename}",
        ))

    # Derived views (optional packed scratch buffers)
    derived_cfg = spec.get("derived", []) or []
    derived_exports: List[DerivedExport] = []
    # helper to expand dim names inside expressions, e.g., nconmax -> m->nconmax
    def expand_dim_expr(expr: str) -> str:
        if not expr:
            return expr
        # simple token replacement by dims map; keep order to avoid substr conflicts
        keys = sorted(dims.keys(), key=lambda x: -len(x))
        out = expr
        for k in keys:
            v = dims.get(k)
            if not v:
                continue
            out = re.sub(rf"\b{re.escape(k)}\b", v, out)
        return out
    for entry in derived_cfg:
        name = entry.get("name") or entry.get("id")
        owner = entry.get("owner")
        base = entry.get("base")
        target_struct = entry.get("struct")
        field = entry.get("field")
        dtype = entry.get("dtype")
        width_expr_raw = entry.get("width", "1")
        width_expr = str(width_expr_raw) if width_expr_raw is not None else "1"
        need_expr = expand_dim_expr(entry.get("len"))
        count_expr = expand_dim_expr(entry.get("count"))
        rule_id = entry.get("id") or f"derived_{name}"
        is_array = bool(entry.get("array", False))
        if not all([name, owner, base, target_struct, field, dtype, need_expr, count_expr]):
            continue
        derived_exports.append(DerivedExport(
            name=name,
            owner=owner,
            base_field=base,
            target_struct=target_struct,
            subfield=field,
            dtype=dtype,
            width_expr=width_expr,
            is_array=is_array,
            need_expr=need_expr,
            count_expr=count_expr,
            rule_id=rule_id,
        ))

    # Header emission
    with open(out_h, "w", encoding="utf-8") as fh:
        fh.write(HDR_PREAMBLE)
        for ptr in pointers:
            fh.write(generate_pointer_decl(ptr))
        for der in derived_exports:
            fh.write(generate_derived_decl(der))
        for dim in dim_exports:
            fh.write(generate_int_decl(dim.func_name))
        for count in count_exports:
            fh.write(generate_int_decl(count.func_name))
        fh.write(HDR_POST)

    # Source emission
    with open(out_c, "w", encoding="utf-8") as fc:
        fc.write(SRC_PREAMBLE)
        for ptr in pointers:
            fc.write(generate_pointer_impl(ptr))
        for der in derived_exports:
            fc.write(generate_derived_impl(der))
        for dim in dim_exports:
            fc.write(generate_dim_impl(dim))
        for count in count_exports:
            fc.write(generate_count_impl(count, dims, field_map))

    extra_exports: List[str] = []
    for ptr in pointers:
        extra_exports.append(f"{ptr.name}_ptr")
    for der in derived_exports:
        extra_exports.append(f"{der.name}_ptr")
    for dim in dim_exports:
        extra_exports.append(dim.base_name)
    for count in count_exports:
        extra_exports.append(count.base_name)
    extras_cfg = spec.get("extras", {}) or {}
    for name in extras_cfg.get("exports", []) or []:
        if name:
            extra_exports.append(name)
    extra_exports_path = out_h.parent / "mjwf_extra_exports.lst"
    with open(extra_exports_path, "w", encoding="utf-8") as fe:
        for name in sorted(set(extra_exports)):
            fe.write(f"{name}\n")

    report_path = (spec_dir / report_json).resolve() if report_json else None
    report_data = {
        "matched": registry.matched,
        "skipped": skipped,
        "diagnostics": diagnostics,
        "pointers": [
            {
                "name": ptr.name,
                "struct": ptr.struct,
                "field": ptr.field,
                "dtype": ptr.dtype,
                "len": ptr.len_expr,
                "rule": ptr.rule_id,
            }
            for ptr in pointers
        ],
        "counts": [
            {
                "name": c.name,
                "func": c.func_name,
                "struct": c.struct,
                "field": c.target_field,
                "dim": c.dim_name,
                "rule": c.rule_id,
            }
            for c in count_exports
        ],
        "dims": [
            {
                "name": d.name,
                "func": d.func_name,
                "struct": d.struct,
                "expr": d.expr,
            }
            for d in dim_exports
        ],
        "extras": extras_cfg.get("exports", []),
    }
    write_report(report_path, report_data)

    return 0

if __name__ == "__main__":
    sys.exit(main())
