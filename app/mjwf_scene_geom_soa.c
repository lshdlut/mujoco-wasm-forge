// AUTO-GENERATED. Do not edit by hand.
// Source: abi_exports/gen_scene_geom_soa.py
#include "mjwf_scene_geom_soa.h"
#include <mujoco/mujoco.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

#ifndef MJWF_MAXH
#define MJWF_MAXH 64
#endif

// helper entrypoints are defined in mjwf_handles.c
int mjwf_helper_valid(int h);
extern mjModel* _mjwf_model_of(int h);
extern mjData*  _mjwf_data_of(int h);

typedef struct MjwfVisState {
  int init;
  int maxgeom;
  mjvOption vopt;
  mjvCamera cam;
  mjvPerturb pert;
  mjvScene scn;
  int32_t* geoms_type;
  int32_t* geoms_dataid;
  int32_t* geoms_objtype;
  int32_t* geoms_objid;
  int32_t* geoms_category;
  int32_t* geoms_matid;
  int32_t* geoms_texcoord;
  int32_t* geoms_segid;
  float* geoms_size;
  float* geoms_pos;
  float* geoms_mat;
  float* geoms_rgba;
  float* geoms_emission;
  float* geoms_specular;
  float* geoms_shininess;
  float* geoms_reflectance;
  uint8_t* geoms_label;
  float* geoms_camdist;
  float* geoms_modelrbound;
  uint8_t* geoms_transparent;
} MjwfVisState;

static MjwfVisState g_vis[MJWF_MAXH];

static void mjwf_vis_clear_slot(int h) {
  if (h <= 0 || h >= MJWF_MAXH) return;
  MjwfVisState* st = &g_vis[h];
  if (!st->init) return;
  mjv_freeScene(&st->scn);
  free(st->geoms_type);
  st->geoms_type = NULL;
  free(st->geoms_dataid);
  st->geoms_dataid = NULL;
  free(st->geoms_objtype);
  st->geoms_objtype = NULL;
  free(st->geoms_objid);
  st->geoms_objid = NULL;
  free(st->geoms_category);
  st->geoms_category = NULL;
  free(st->geoms_matid);
  st->geoms_matid = NULL;
  free(st->geoms_texcoord);
  st->geoms_texcoord = NULL;
  free(st->geoms_segid);
  st->geoms_segid = NULL;
  free(st->geoms_size);
  st->geoms_size = NULL;
  free(st->geoms_pos);
  st->geoms_pos = NULL;
  free(st->geoms_mat);
  st->geoms_mat = NULL;
  free(st->geoms_rgba);
  st->geoms_rgba = NULL;
  free(st->geoms_emission);
  st->geoms_emission = NULL;
  free(st->geoms_specular);
  st->geoms_specular = NULL;
  free(st->geoms_shininess);
  st->geoms_shininess = NULL;
  free(st->geoms_reflectance);
  st->geoms_reflectance = NULL;
  free(st->geoms_label);
  st->geoms_label = NULL;
  free(st->geoms_camdist);
  st->geoms_camdist = NULL;
  free(st->geoms_modelrbound);
  st->geoms_modelrbound = NULL;
  free(st->geoms_transparent);
  st->geoms_transparent = NULL;
  st->maxgeom = 0;
  st->init = 0;
}

void mjwf_scene_reset(int h) {
  mjwf_vis_clear_slot(h);
}

static MjwfVisState* mjwf_vis_state(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  if (h <= 0 || h >= MJWF_MAXH) return NULL;
  MjwfVisState* st = &g_vis[h];
  if (!st->init) {
    memset(st, 0, sizeof(*st));
    mjv_defaultOption(&st->vopt);
    mjv_defaultCamera(&st->cam);
    mjv_defaultPerturb(&st->pert);
    mjv_defaultScene(&st->scn);
    st->init = 1;
  }
  return st;
}

static int mjwf_vis_reserve(MjwfVisState* st, const mjModel* m, int maxgeom) {
  if (!st || !m) return 0;
  if (maxgeom <= st->maxgeom) return 1;

  // (Re)allocate the MuJoCo scene buffers.
  if (st->scn.maxgeom > 0) {
    mjv_freeScene(&st->scn);
    mjv_defaultScene(&st->scn);
  }
  mjv_makeScene(m, &st->scn, maxgeom);

  // (Re)allocate SoA attribute buffers.
  int ok = 1;
  void* tmp_type = realloc(st->geoms_type, sizeof(int32_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_type) ok = 0;
  else st->geoms_type = (int32_t*)tmp_type;
  void* tmp_dataid = realloc(st->geoms_dataid, sizeof(int32_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_dataid) ok = 0;
  else st->geoms_dataid = (int32_t*)tmp_dataid;
  void* tmp_objtype = realloc(st->geoms_objtype, sizeof(int32_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_objtype) ok = 0;
  else st->geoms_objtype = (int32_t*)tmp_objtype;
  void* tmp_objid = realloc(st->geoms_objid, sizeof(int32_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_objid) ok = 0;
  else st->geoms_objid = (int32_t*)tmp_objid;
  void* tmp_category = realloc(st->geoms_category, sizeof(int32_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_category) ok = 0;
  else st->geoms_category = (int32_t*)tmp_category;
  void* tmp_matid = realloc(st->geoms_matid, sizeof(int32_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_matid) ok = 0;
  else st->geoms_matid = (int32_t*)tmp_matid;
  void* tmp_texcoord = realloc(st->geoms_texcoord, sizeof(int32_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_texcoord) ok = 0;
  else st->geoms_texcoord = (int32_t*)tmp_texcoord;
  void* tmp_segid = realloc(st->geoms_segid, sizeof(int32_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_segid) ok = 0;
  else st->geoms_segid = (int32_t*)tmp_segid;
  void* tmp_size = realloc(st->geoms_size, sizeof(float)*((size_t)3) * (size_t)maxgeom);
  if (!tmp_size) ok = 0;
  else st->geoms_size = (float*)tmp_size;
  void* tmp_pos = realloc(st->geoms_pos, sizeof(float)*((size_t)3) * (size_t)maxgeom);
  if (!tmp_pos) ok = 0;
  else st->geoms_pos = (float*)tmp_pos;
  void* tmp_mat = realloc(st->geoms_mat, sizeof(float)*((size_t)9) * (size_t)maxgeom);
  if (!tmp_mat) ok = 0;
  else st->geoms_mat = (float*)tmp_mat;
  void* tmp_rgba = realloc(st->geoms_rgba, sizeof(float)*((size_t)4) * (size_t)maxgeom);
  if (!tmp_rgba) ok = 0;
  else st->geoms_rgba = (float*)tmp_rgba;
  void* tmp_emission = realloc(st->geoms_emission, sizeof(float)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_emission) ok = 0;
  else st->geoms_emission = (float*)tmp_emission;
  void* tmp_specular = realloc(st->geoms_specular, sizeof(float)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_specular) ok = 0;
  else st->geoms_specular = (float*)tmp_specular;
  void* tmp_shininess = realloc(st->geoms_shininess, sizeof(float)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_shininess) ok = 0;
  else st->geoms_shininess = (float*)tmp_shininess;
  void* tmp_reflectance = realloc(st->geoms_reflectance, sizeof(float)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_reflectance) ok = 0;
  else st->geoms_reflectance = (float*)tmp_reflectance;
  void* tmp_label = realloc(st->geoms_label, sizeof(uint8_t)*((size_t)100) * (size_t)maxgeom);
  if (!tmp_label) ok = 0;
  else st->geoms_label = (uint8_t*)tmp_label;
  void* tmp_camdist = realloc(st->geoms_camdist, sizeof(float)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_camdist) ok = 0;
  else st->geoms_camdist = (float*)tmp_camdist;
  void* tmp_modelrbound = realloc(st->geoms_modelrbound, sizeof(float)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_modelrbound) ok = 0;
  else st->geoms_modelrbound = (float*)tmp_modelrbound;
  void* tmp_transparent = realloc(st->geoms_transparent, sizeof(uint8_t)*((size_t)1) * (size_t)maxgeom);
  if (!tmp_transparent) ok = 0;
  else st->geoms_transparent = (uint8_t*)tmp_transparent;
  if (!ok) return 0;
  st->maxgeom = maxgeom;
  return 1;
}

mjvScene* _mjwf_scene_of(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? &st->scn : NULL;
}

mjvOption* _mjwf_vopt_of(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? &st->vopt : NULL;
}

mjvCamera* _mjwf_cam_of(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? &st->cam : NULL;
}

mjvPerturb* _mjwf_pert_of(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? &st->pert : NULL;
}

EMSCRIPTEN_KEEPALIVE int mjwf_scene_update_and_pack(int h, int catmask) {
  if (!mjwf_helper_valid(h)) return -1;
  mjModel* m = _mjwf_model_of(h);
  mjData*  d = _mjwf_data_of(h);
  if (!m || !d) return -1;
  MjwfVisState* st = mjwf_vis_state(h);
  if (!st) return -1;

  int maxgeom = st->maxgeom > 0 ? st->maxgeom : 1024;
  for (int attempt = 0; attempt < 4; ++attempt) {
    if (!mjwf_vis_reserve(st, m, maxgeom)) return -1;
    mjv_updateScene(m, d, &st->vopt, &st->pert, &st->cam, catmask, &st->scn);
    if (!st->scn.status) break;
    // Geom buffer exhausted; grow and retry.
    maxgeom *= 2;
  }
  if (st->scn.status) return -1;

  const int n = st->scn.ngeom;
  if (n <= 0) return 0;

  for (int i = 0; i < n; ++i) {
    const mjvGeom* g = &st->scn.geoms[i];
    st->geoms_type[i] = (int32_t)g->type;
    st->geoms_dataid[i] = (int32_t)g->dataid;
    st->geoms_objtype[i] = (int32_t)g->objtype;
    st->geoms_objid[i] = (int32_t)g->objid;
    st->geoms_category[i] = (int32_t)g->category;
    st->geoms_matid[i] = (int32_t)g->matid;
    st->geoms_texcoord[i] = (int32_t)g->texcoord;
    st->geoms_segid[i] = (int32_t)g->segid;
    memcpy(&st->geoms_size[((size_t)i)*3], g->size, sizeof(float)*3);
    memcpy(&st->geoms_pos[((size_t)i)*3], g->pos, sizeof(float)*3);
    memcpy(&st->geoms_mat[((size_t)i)*9], g->mat, sizeof(float)*9);
    memcpy(&st->geoms_rgba[((size_t)i)*4], g->rgba, sizeof(float)*4);
    st->geoms_emission[i] = (float)g->emission;
    st->geoms_specular[i] = (float)g->specular;
    st->geoms_shininess[i] = (float)g->shininess;
    st->geoms_reflectance[i] = (float)g->reflectance;
    memcpy(&st->geoms_label[((size_t)i)*100], g->label, sizeof(uint8_t)*100);
    st->geoms_camdist[i] = (float)g->camdist;
    st->geoms_modelrbound[i] = (float)g->modelrbound;
    st->geoms_transparent[i] = (uint8_t)g->transparent;
  }

  return n;
}

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_type_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (int32_t*)(st->geoms_type) : NULL;
}

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_dataid_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (int32_t*)(st->geoms_dataid) : NULL;
}

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_objtype_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (int32_t*)(st->geoms_objtype) : NULL;
}

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_objid_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (int32_t*)(st->geoms_objid) : NULL;
}

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_category_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (int32_t*)(st->geoms_category) : NULL;
}

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_matid_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (int32_t*)(st->geoms_matid) : NULL;
}

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_texcoord_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (int32_t*)(st->geoms_texcoord) : NULL;
}

EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geoms_segid_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (int32_t*)(st->geoms_segid) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_size_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_size) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_pos_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_pos) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_mat_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_mat) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_rgba_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_rgba) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_emission_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_emission) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_specular_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_specular) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_shininess_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_shininess) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_reflectance_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_reflectance) : NULL;
}

EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_geoms_label_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (uint8_t*)(st->geoms_label) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_camdist_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_camdist) : NULL;
}

EMSCRIPTEN_KEEPALIVE float* mjwf_scene_geoms_modelrbound_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (float*)(st->geoms_modelrbound) : NULL;
}

EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_geoms_transparent_ptr(int h) {
  MjwfVisState* st = mjwf_vis_state(h);
  return st ? (uint8_t*)(st->geoms_transparent) : NULL;
}
