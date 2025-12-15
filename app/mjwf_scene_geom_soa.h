// AUTO-GENERATED. Do not edit by hand.
// Source: abi_exports/gen_scene_geom_soa.py
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

EMSCRIPTEN_KEEPALIVE int mjwf_scene_update_and_pack(int h, int catmask);

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_type_ptr(int h);
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_dataid_ptr(int h);
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_objtype_ptr(int h);
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_objid_ptr(int h);
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_category_ptr(int h);
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_matid_ptr(int h);
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_texcoord_ptr(int h);
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_segid_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_size_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_pos_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_mat_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_rgba_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_emission_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_specular_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_shininess_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_reflectance_ptr(int h);
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_geoms_label_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_camdist_ptr(int h);
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_modelrbound_ptr(int h);
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_geoms_transparent_ptr(int h);

#ifdef __cplusplus
}
#endif
