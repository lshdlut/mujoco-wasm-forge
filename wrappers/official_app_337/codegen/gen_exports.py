#!/usr/bin/env python3
import sys, os, yaml

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

def emit_view_decl(name, dtype):
    cty = 'double*' if dtype == 'f64' else 'int32_t*'
    return f"EMSCRIPTEN_KEEPALIVE {cty} mjwf_{name}_ptr(int h);\n"

def emit_dim_decl(name):
    return f"EMSCRIPTEN_KEEPALIVE int mjwf_{name}(int h);\n"

def emit_view_impl(name, src, dtype):
    cty = 'double*' if dtype == 'f64' else 'int32_t*'
    return (
        f"EMSCRIPTEN_KEEPALIVE {cty} mjwf_{name}_ptr(int h) {{\n"
        f"  if (!mjwf_valid(h)) return NULL;\n"
        f"  mjData* d = _mjwf_data_of(h);\n"
        f"  return d ? ({cty})({src.replace('d->','d->')}) : NULL;\n"
        f"}}\n\n"
    )

def emit_dim_impl(name, expr):
    return (
        f"EMSCRIPTEN_KEEPALIVE int mjwf_{name}(int h) {{\n"
        f"  if (!mjwf_valid(h)) return 0;\n"
        f"  mjModel* m = _mjwf_model_of(h);\n"
        f"  return m ? (int)({expr}) : 0;\n"
        f"}}\n\n"
    )

def main():
    if len(sys.argv) != 4:
        print("Usage: gen_exports.py <spec.yaml> <out.h> <out.c>")
        return 2
    spec_path, out_h, out_c = sys.argv[1:]
    spec = yaml.safe_load(open(spec_path, 'r', encoding='utf-8'))
    views = spec.get('views', [])
    dims  = spec.get('dims', [])

    # Header
    with open(out_h, 'w', encoding='utf-8') as fh:
        fh.write(HDR_PREAMBLE)
        for v in views:
            fh.write(emit_view_decl(v['name'], v['dtype']))
        for d in dims:
            k, v = list(d.items())[0]
            fh.write(emit_dim_decl(k))
        fh.write(HDR_POST)

    # Source
    with open(out_c, 'w', encoding='utf-8') as fc:
        fc.write(SRC_PREAMBLE)
        for v in views:
            fc.write(emit_view_impl(v['name'], v['src'], v['dtype']))
        for d in dims:
            k, v = list(d.items())[0]
            fc.write(emit_dim_impl(k, v))

if __name__ == '__main__':
    sys.exit(main())
