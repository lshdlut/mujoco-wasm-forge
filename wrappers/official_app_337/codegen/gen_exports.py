#!/usr/bin/env python3
import json
import os
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Tuple

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
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

// mjwf_valid/mjwf_n? etc. are defined in mjwf_handles.c
int mjwf_valid(int h);

typedef struct { mjModel* m; mjData* d; } _mjwf_view_pair;

// We can't access g_pool from here; expose small inline accessors via declarations.
// For simplicity in this generator, we re-declare pointer getters here and forward
// to mjwf_handles.c via mjwf_valid and direct field access using a trick: define
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

@dataclass
class FieldDef:
    struct: str
    name: str
    ctype: str
    base_type: str
    comment: Optional[str]
    line: int

@dataclass
class PointerExport:
    name: str
    struct: str
    field: str
    dtype: str
    ctype: str
    len_expr: Optional[str]
    rule_id: str

@dataclass
class CountExport:
    name: str
    struct: str
    target_field: str
    dim_name: Optional[str]
    mode: str  # currently only "sum"
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
        if ch == ";":
            pending_flush = True
            i += 1
            continue
        if pending_flush and ch == "\n":
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
            fields[name] = FieldDef(
                struct=struct_name,
                name=name,
                ctype=ctype,
                base_type=base,
                comment=comment,
                line=line,
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

def parse_fields(src: str, struct_name: str) -> Dict[str, FieldDef]:
    block, base_line = find_struct_block(src, struct_name)
    fields: Dict[str, FieldDef] = {}
    search_pos = 0
    for raw in iter_declarations(block):
        segment = raw.strip()
        if not segment:
            continue
        decl_clean = re.sub(r"//.*", "", raw)
        decl_clean = re.sub(r"/\*[\s\S]*?\*/", "", decl_clean)
        decl_lines = [ln.strip() for ln in decl_clean.splitlines() if ln.strip()]
        if not decl_lines:
            continue
        decl_line = decl_lines[0].rstrip(";").strip()
        m = re.match(r"([A-Za-z0-9_\s\*]+?)\s+([A-Za-z0-9_]+)(\[[^\]]+\])?", decl_line)
        if not m:
            continue
        ctype = m.group(1).strip()
        name = m.group(2)
        if "*" not in ctype:
            continue
        base = ctype.replace("const", " ").replace("*", " ").strip()
        base = re.sub(r"\s+", " ", base)
        comment_parts: List[str] = []
        for line in raw.splitlines():
            idx = line.find("//")
            if idx != -1:
                comment_parts.append(line[idx + 2 :].strip())
        comment = " ".join(comment_parts).strip() or None
        start_idx = block.find(raw, search_pos)
        if start_idx == -1:
            start_idx = search_pos
        line_no = base_line + block[:start_idx].count("\n")
        search_pos = start_idx + len(raw) + 1
        fields[name] = FieldDef(
            struct=struct_name,
            name=name,
            ctype=ctype,
            base_type=base,
            comment=comment,
            line=line_no,
        )
    return fields

def resolve_header_path(spec_dir: Path, search_roots: Sequence[str], header: str) -> Path:
    for root in search_roots:
        candidate = (spec_dir / root).resolve() / header
        if candidate.is_file():
            return candidate
    raise RuntimeError(f"Unable to locate header {header} in search paths: {search_roots}")

def normalize_dims(dims_section) -> Dict[str, str]:
    if not dims_section:
        return {}
    if isinstance(dims_section, dict):
        base_dims = dict(dims_section)
    else:
        base_dims: Dict[str, str] = {}
        for entry in dims_section:
            if isinstance(entry, dict):
                base_dims.update(entry)
    merged: Dict[str, str] = {}
    for key, value in base_dims.items():
        merged[key] = value
        merged[key.lower()] = value
    return merged

def normalize_counts(counts_section) -> List[Dict[str, str]]:
    if not counts_section:
        return []
    if isinstance(counts_section, dict):
        return [counts_section]
    return list(counts_section)

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

def generate_int_decl(name: str) -> str:
    return f"EMSCRIPTEN_KEEPALIVE int mjwf_{name}(int h);\n"

def needs_model(expr: str) -> bool:
    return "m->" in expr

def needs_data(expr: str) -> bool:
    return "d->" in expr

def generate_pointer_impl(ptr: PointerExport) -> str:
    owner = "m" if ptr.struct == "mjModel" else "d"
    field_expr = f"{owner}->{ptr.field}"
    void_line = "  (void)d;\n" if owner == "m" else "  (void)m;\n"
    return (
        f"EMSCRIPTEN_KEEPALIVE {ptr.ctype} mjwf_{ptr.name}_ptr(int h) {{\n"
        f"  if (!mjwf_valid(h)) return NULL;\n"
        f"  mjModel* m = _mjwf_model_of(h);\n"
        f"  mjData* d  = _mjwf_data_of(h);\n"
        f"{void_line}"
        f"  return {owner} ? ({ptr.ctype})({field_expr}) : NULL;\n"
        f"}}\n\n"
    )

def generate_dim_impl(name: str, expr: str) -> str:
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
        f"EMSCRIPTEN_KEEPALIVE int mjwf_{name}(int h) {{\n"
        f"  if (!mjwf_valid(h)) return 0;\n"
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
        f"EMSCRIPTEN_KEEPALIVE int mjwf_{count.name}(int h) {{\n"
        f"  if (!mjwf_valid(h)) return 0;\n"
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

def compile_rules(spec_rules, auto_rules, field_map, dims, dim_hints) -> Tuple[List[PointerExport], Dict[str, List[str]], List[str]]:
    pointers: List[PointerExport] = []
    matched: Dict[str, List[str]] = {"mjModel": [], "mjData": []}
    diagnostics: List[str] = []
    used_names: Dict[str, PointerExport] = {}

    def register_pointer(ptr: PointerExport) -> None:
        if ptr.name in used_names:
            return
        pointers.append(ptr)
        used_names[ptr.name] = ptr
        matched.setdefault(ptr.struct, []).append(ptr.field)

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
            register_pointer(PointerExport(
                name=pointer_name,
                struct=struct,
                field=fname,
                dtype=dtype,
                ctype=ctype,
                len_expr=len_expr,
                rule_id=rule_id,
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
            if fname in matched.get(struct, []):
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
            pointer_name = rule.get("export_name") or fname
            ctype = dtype_to_ctype(dtype, fdef.ctype.strip())
            register_pointer(PointerExport(
                name=pointer_name,
                struct=struct,
                field=fname,
                dtype=dtype,
                ctype=ctype,
                len_expr=len_expr,
                rule_id=rule_id,
            ))

    return pointers, matched, diagnostics

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

    if "mjModel" in structs_from_scan:
        model_fields = structs_from_scan["mjModel"]
    else:
        model_fields = parse_fields(load_text(header_model_path), "mjModel")
    if "mjData" in structs_from_scan:
        data_fields = structs_from_scan["mjData"]
    else:
        data_fields = parse_fields(load_text(header_data_path), "mjData")

    field_map = {"mjModel": model_fields, "mjData": data_fields}

    dims = normalize_dims(spec.get("dims"))
    counts_cfg = normalize_counts(spec.get("counts"))
    auto_rules = spec.get("auto", [])
    rules = spec.get("rules", [])

    pointers, matched, diagnostics = compile_rules(rules, auto_rules, field_map, dims, dim_hints)
    skipped = compute_skipped(field_map, matched)

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
        count_exports.append(CountExport(
            name=name,
            struct=struct,
            target_field=target_field,
            dim_name=dim_name,
            mode=mode,
            rule_id=rule_id,
        ))

    # Header emission
    with open(out_h, "w", encoding="utf-8") as fh:
        fh.write(HDR_PREAMBLE)
        for ptr in pointers:
            fh.write(generate_pointer_decl(ptr))
        for dim_name in dims.keys():
            fh.write(generate_int_decl(dim_name))
        for count in count_exports:
            fh.write(generate_int_decl(count.name))
        fh.write(HDR_POST)

    # Source emission
    with open(out_c, "w", encoding="utf-8") as fc:
        fc.write(SRC_PREAMBLE)
        for ptr in pointers:
            fc.write(generate_pointer_impl(ptr))
        for dim_name, expr in dims.items():
            fc.write(generate_dim_impl(dim_name, expr))
        for count in count_exports:
            fc.write(generate_count_impl(count, dims, field_map))

    extra_exports: List[str] = []
    for ptr in pointers:
        extra_exports.append(f"{ptr.name}_ptr")
    for count in count_exports:
        extra_exports.append(count.name)
    extra_exports_path = out_h.parent / "mjwf_extra_exports.lst"
    with open(extra_exports_path, "w", encoding="utf-8") as fe:
        for name in sorted(set(extra_exports)):
            fe.write(f"{name}\n")

    report_path = (spec_dir / report_json).resolve() if report_json else None
    report_data = {
        "matched": matched,
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
                "struct": c.struct,
                "field": c.target_field,
                "dim": c.dim_name,
                "rule": c.rule_id,
            }
            for c in count_exports
        ],
    }
    write_report(report_path, report_data)

    return 0

if __name__ == "__main__":
    sys.exit(main())
