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
extern mjvScene* _mjwf_scene_of(int h);
extern mjvOption* _mjwf_vopt_of(int h);
extern mjvCamera* _mjwf_cam_of(int h);
extern mjvPerturb* _mjwf_pert_of(int h);

EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_buffer_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->buffer);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_qpos0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->qpos0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_qpos_spring_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->qpos_spring);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_parentid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_parentid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_rootid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_rootid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_weldid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_weldid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_mocapid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_mocapid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_jntnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_jntnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_jntadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_jntadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_dofnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_dofnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_dofadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_dofadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_treeid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_treeid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_geomnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_geomnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_geomadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_geomadr);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_body_simple_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->body_simple);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_body_sameframe_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->body_sameframe);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_quat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_quat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_ipos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_ipos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_iquat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_iquat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_mass_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_mass);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_subtreemass_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_subtreemass);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_inertia_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_inertia);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_invweight0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_invweight0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_gravcomp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_gravcomp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_margin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_margin);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_body_user_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->body_user);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_plugin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_plugin);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_contype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_contype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_conaffinity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_conaffinity);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_bvhadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_bvhadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_body_bvhnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->body_bvhnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_bvh_depth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->bvh_depth);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_bvh_child_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->bvh_child);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_bvh_nodeid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->bvh_nodeid);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_bvh_aabb_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->bvh_aabb);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_oct_depth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->oct_depth);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_oct_child_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->oct_child);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_oct_aabb_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->oct_aabb);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_oct_coeff_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->oct_coeff);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_jnt_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->jnt_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_jnt_qposadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->jnt_qposadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_jnt_dofadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->jnt_dofadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_jnt_bodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->jnt_bodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_jnt_group_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->jnt_group);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_jnt_limited_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->jnt_limited);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_jnt_actfrclimited_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->jnt_actfrclimited);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_jnt_actgravcomp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->jnt_actgravcomp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_solref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_solref);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_solimp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_solimp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_axis_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_axis);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_stiffness_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_stiffness);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_range_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_range);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_actfrcrange_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_actfrcrange);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_margin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_margin);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_jnt_user_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->jnt_user);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_dof_bodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->dof_bodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_dof_jntid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->dof_jntid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_dof_parentid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->dof_parentid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_dof_treeid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->dof_treeid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_dof_Madr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->dof_Madr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_dof_simplenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->dof_simplenum);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_dof_solref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->dof_solref);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_dof_solimp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->dof_solimp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_dof_frictionloss_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->dof_frictionloss);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_dof_armature_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->dof_armature);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_dof_damping_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->dof_damping);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_dof_invweight0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->dof_invweight0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_dof_M0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->dof_M0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_dof_length_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->dof_length);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tree_bodyadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tree_bodyadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tree_bodynum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tree_bodynum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tree_dofadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tree_dofadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tree_dofnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tree_dofnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tree_sleep_policy_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tree_sleep_policy);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_contype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_contype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_conaffinity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_conaffinity);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_condim_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_condim);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_bodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_bodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_dataid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_dataid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_matid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_matid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_group_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_group);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_priority_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_priority);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_geom_plugin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->geom_plugin);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_geom_sameframe_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->geom_sameframe);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_solmix_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_solmix);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_solref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_solref);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_solimp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_solimp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_size_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_size);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_aabb_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_aabb);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_rbound_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_rbound);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_quat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_quat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_friction_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_friction);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_margin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_margin);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_gap_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_gap);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_fluid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_fluid);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_geom_user_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->geom_user);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_geom_rgba_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->geom_rgba);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_site_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->site_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_site_bodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->site_bodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_site_matid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->site_matid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_site_group_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->site_group);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_site_sameframe_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->site_sameframe);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_site_size_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->site_size);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_site_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->site_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_site_quat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->site_quat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_site_user_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->site_user);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_site_rgba_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->site_rgba);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_cam_mode_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->cam_mode);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_cam_bodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->cam_bodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_cam_targetbodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->cam_targetbodyid);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_cam_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->cam_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_cam_quat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->cam_quat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_cam_poscom0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->cam_poscom0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_cam_pos0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->cam_pos0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_cam_mat0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->cam_mat0);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_cam_projection_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->cam_projection);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_cam_fovy_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->cam_fovy);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_cam_ipd_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->cam_ipd);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_cam_resolution_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->cam_resolution);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_cam_output_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->cam_output);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_cam_sensorsize_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->cam_sensorsize);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_cam_intrinsic_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->cam_intrinsic);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_cam_user_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->cam_user);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_light_mode_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->light_mode);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_light_bodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->light_bodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_light_targetbodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->light_targetbodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_light_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->light_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_light_texid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->light_texid);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_light_castshadow_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->light_castshadow);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_bulbradius_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_bulbradius);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_intensity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_intensity);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_range_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_range);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_light_active_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->light_active);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_light_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->light_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_light_dir_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->light_dir);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_light_poscom0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->light_poscom0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_light_pos0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->light_pos0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_light_dir0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->light_dir0);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_attenuation_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_attenuation);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_cutoff_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_cutoff);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_exponent_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_exponent);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_ambient_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_ambient);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_diffuse_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_diffuse);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_light_specular_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->light_specular);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_contype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_contype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_conaffinity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_conaffinity);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_condim_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_condim);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_priority_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_priority);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_solmix_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_solmix);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_solref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_solref);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_solimp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_solimp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_friction_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_friction);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_margin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_margin);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_gap_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_gap);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_flex_internal_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->flex_internal);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_selfcollide_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_selfcollide);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_activelayers_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_activelayers);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_passive_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_passive);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_dim_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_dim);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_matid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_matid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_group_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_group);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_interp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_interp);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_nodeadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_nodeadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_nodenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_nodenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_vertadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_vertadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_vertnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_vertnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_edgeadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_edgeadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_edgenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_edgenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_elemadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_elemadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_elemnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_elemnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_elemdataadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_elemdataadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_elemedgeadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_elemedgeadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_shellnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_shellnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_shelldataadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_shelldataadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_evpairadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_evpairadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_evpairnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_evpairnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_texcoordadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_texcoordadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_nodebodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_nodebodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_vertbodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_vertbodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_vertedgeadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_vertedgeadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_vertedgenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_vertedgenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_vertedge_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_vertedge);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_edge_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_edge);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_edgeflap_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_edgeflap);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_elem_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_elem);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_elemtexcoord_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_elemtexcoord);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_elemedge_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_elemedge);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_elemlayer_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_elemlayer);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_shell_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_shell);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_evpair_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_evpair);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_vert_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_vert);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_vert0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_vert0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_vertmetric_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_vertmetric);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_node_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_node);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_node0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_node0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flexedge_length0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flexedge_length0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flexedge_invweight0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flexedge_invweight0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_radius_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_radius);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_size_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_size);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_stiffness_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_stiffness);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_bending_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_bending);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_damping_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_damping);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_edgestiffness_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_edgestiffness);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_flex_edgedamping_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->flex_edgedamping);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_edgeequality_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_edgeequality);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_flex_rigid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->flex_rigid);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_flexedge_rigid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->flexedge_rigid);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_flex_centered_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->flex_centered);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_flex_flatskin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->flex_flatskin);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_bvhadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_bvhadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flex_bvhnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flex_bvhnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flexedge_J_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flexedge_J_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flexedge_J_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flexedge_J_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flexedge_J_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flexedge_J_colind);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flexvert_J_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flexvert_J_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flexvert_J_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flexvert_J_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_flexvert_J_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->flexvert_J_colind);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_flex_rgba_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->flex_rgba);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_flex_texcoord_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->flex_texcoord);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_vertadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_vertadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_vertnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_vertnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_faceadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_faceadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_facenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_facenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_bvhadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_bvhadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_bvhnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_bvhnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_octadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_octadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_octnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_octnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_normaladr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_normaladr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_normalnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_normalnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_texcoordadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_texcoordadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_texcoordnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_texcoordnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_graphadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_graphadr);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mesh_vert_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mesh_vert);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mesh_normal_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mesh_normal);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mesh_texcoord_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mesh_texcoord);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_face_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_face);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_facenormal_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_facenormal);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_facetexcoord_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_facetexcoord);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_graph_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_graph);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_mesh_scale_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->mesh_scale);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_mesh_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->mesh_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_mesh_quat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->mesh_quat);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_pathadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_pathadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_polynum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_polynum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_polyadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_polyadr);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_mesh_polynormal_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->mesh_polynormal);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_polyvertadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_polyvertadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_polyvertnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_polyvertnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_polyvert_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_polyvert);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_polymapadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_polymapadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_polymapnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_polymapnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mesh_polymap_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mesh_polymap);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_matid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_matid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_group_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_group);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_skin_rgba_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->skin_rgba);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_skin_inflate_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->skin_inflate);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_vertadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_vertadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_vertnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_vertnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_texcoordadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_texcoordadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_faceadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_faceadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_facenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_facenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_boneadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_boneadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_bonenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_bonenum);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_skin_vert_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->skin_vert);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_skin_texcoord_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->skin_texcoord);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_face_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_face);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_bonevertadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_bonevertadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_bonevertnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_bonevertnum);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_skin_bonebindpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->skin_bonebindpos);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_skin_bonebindquat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->skin_bonebindquat);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_bonebodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_bonebodyid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_bonevertid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_bonevertid);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_skin_bonevertweight_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->skin_bonevertweight);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_skin_pathadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->skin_pathadr);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_hfield_size_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->hfield_size);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_hfield_nrow_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->hfield_nrow);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_hfield_ncol_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->hfield_ncol);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_hfield_adr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->hfield_adr);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_hfield_data_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->hfield_data);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_hfield_pathadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->hfield_pathadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tex_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tex_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tex_colorspace_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tex_colorspace);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tex_height_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tex_height);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tex_width_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tex_width);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tex_nchannel_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tex_nchannel);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tex_adr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tex_adr);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_tex_data_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->tex_data);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tex_pathadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tex_pathadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mat_texid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mat_texid);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_mat_texuniform_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->mat_texuniform);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mat_texrepeat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mat_texrepeat);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mat_emission_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mat_emission);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mat_specular_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mat_specular);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mat_shininess_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mat_shininess);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mat_reflectance_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mat_reflectance);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mat_metallic_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mat_metallic);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mat_roughness_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mat_roughness);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_mat_rgba_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->mat_rgba);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_pair_dim_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->pair_dim);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_pair_geom1_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->pair_geom1);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_pair_geom2_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->pair_geom2);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_pair_signature_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->pair_signature);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_pair_solref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->pair_solref);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_pair_solreffriction_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->pair_solreffriction);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_pair_solimp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->pair_solimp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_pair_margin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->pair_margin);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_pair_gap_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->pair_gap);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_pair_friction_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->pair_friction);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_exclude_signature_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->exclude_signature);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_eq_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->eq_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_eq_obj1id_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->eq_obj1id);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_eq_obj2id_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->eq_obj2id);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_eq_objtype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->eq_objtype);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_eq_active0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->eq_active0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_eq_solref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->eq_solref);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_eq_solimp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->eq_solimp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_eq_data_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->eq_data);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tendon_adr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tendon_adr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tendon_num_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tendon_num);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tendon_matid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tendon_matid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tendon_group_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tendon_group);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tendon_treenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tendon_treenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tendon_treeid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tendon_treeid);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_tendon_limited_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->tendon_limited);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_tendon_actfrclimited_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->tendon_actfrclimited);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_width_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_width);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_solref_lim_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_solref_lim);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_solimp_lim_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_solimp_lim);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_solref_fri_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_solref_fri);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_solimp_fri_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_solimp_fri);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_range_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_range);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_actfrcrange_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_actfrcrange);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_margin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_margin);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_stiffness_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_stiffness);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_damping_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_damping);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_armature_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_armature);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_frictionloss_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_frictionloss);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_lengthspring_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_lengthspring);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_length0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_length0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_invweight0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_invweight0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tendon_user_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tendon_user);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_tendon_rgba_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (float*)(m->tendon_rgba);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_wrap_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->wrap_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_wrap_objid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->wrap_objid);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_wrap_prm_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->wrap_prm);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_trntype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_trntype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_dyntype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_dyntype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_gaintype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_gaintype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_biastype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_biastype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_trnid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_trnid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_actadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_actadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_actnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_actnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_group_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_group);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_history_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_history);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_historyadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_historyadr);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_delay_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_delay);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_actuator_ctrllimited_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->actuator_ctrllimited);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_actuator_forcelimited_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->actuator_forcelimited);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_actuator_actlimited_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->actuator_actlimited);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_dynprm_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_dynprm);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_gainprm_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_gainprm);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_biasprm_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_biasprm);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_actuator_actearly_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->actuator_actearly);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_ctrlrange_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_ctrlrange);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_forcerange_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_forcerange);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_actrange_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_actrange);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_gear_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_gear);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_cranklength_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_cranklength);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_acc0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_acc0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_length0_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_length0);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_lengthrange_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_lengthrange);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_actuator_user_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->actuator_user);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_actuator_plugin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->actuator_plugin);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_datatype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_datatype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_needstage_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_needstage);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_objtype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_objtype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_objid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_objid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_reftype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_reftype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_refid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_refid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_intprm_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_intprm);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_dim_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_dim);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_adr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_adr);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_sensor_cutoff_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->sensor_cutoff);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_sensor_noise_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->sensor_noise);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_history_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_history);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_historyadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_historyadr);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_sensor_delay_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->sensor_delay);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_sensor_interval_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->sensor_interval);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_sensor_user_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->sensor_user);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_sensor_plugin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->sensor_plugin);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_plugin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->plugin);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_plugin_stateadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->plugin_stateadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_plugin_statenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->plugin_statenum);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_plugin_attr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->plugin_attr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_plugin_attradr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->plugin_attradr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_numeric_adr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->numeric_adr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_numeric_size_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->numeric_size);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_numeric_data_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->numeric_data);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_text_adr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->text_adr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_text_size_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->text_size);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_text_data_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->text_data);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tuple_adr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tuple_adr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tuple_size_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tuple_size);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tuple_objtype_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tuple_objtype);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_tuple_objid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->tuple_objid);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_tuple_objprm_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->tuple_objprm);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_key_time_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->key_time);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_key_qpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->key_qpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_key_qvel_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->key_qvel);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_key_act_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->key_act);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_key_mpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->key_mpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_key_mquat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->key_mquat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_key_ctrl_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (double*)(m->key_ctrl);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_bodyadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_bodyadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_jntadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_jntadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_geomadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_geomadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_siteadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_siteadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_camadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_camadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_lightadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_lightadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_flexadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_flexadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_meshadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_meshadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_skinadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_skinadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_hfieldadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_hfieldadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_texadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_texadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_matadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_matadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_pairadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_pairadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_excludeadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_excludeadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_eqadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_eqadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_tendonadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_tendonadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_actuatoradr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_actuatoradr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_sensoradr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_sensoradr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_numericadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_numericadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_textadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_textadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_tupleadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_tupleadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_keyadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_keyadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_name_pluginadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->name_pluginadr);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_names_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->names);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_names_map_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->names_map);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_model_paths_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (uint8_t*)(m->paths);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_B_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->B_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_B_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->B_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_B_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->B_colind);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_M_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->M_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_M_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->M_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_M_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->M_colind);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mapM2M_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mapM2M);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_D_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->D_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_D_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->D_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_D_diag_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->D_diag);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_D_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->D_colind);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mapM2D_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mapM2D);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_mapD2M_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return NULL;
  return (int32_t*)(m->mapD2M);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_pstack_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(&d->pstack);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_pbase_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(&d->pbase);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_parena_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(&d->parena);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_maxuse_stack_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(&d->maxuse_stack);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_maxuse_threadstack_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->maxuse_threadstack);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_maxuse_arena_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(&d->maxuse_arena);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_maxuse_con_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(&d->maxuse_con);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_maxuse_efc_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(&d->maxuse_efc);
}
EMSCRIPTEN_KEEPALIVE mjSolverStat* mjwf_data_solver_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (mjSolverStat*)(d->solver);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_solver_niter_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->solver_niter);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_solver_nnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->solver_nnz);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_solver_fwdinv_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->solver_fwdinv);
}
EMSCRIPTEN_KEEPALIVE mjWarningStat* mjwf_data_warning_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (mjWarningStat*)(d->warning);
}
EMSCRIPTEN_KEEPALIVE mjTimerStat* mjwf_data_timer_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (mjTimerStat*)(d->timer);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_data_flg_energypos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (uint8_t*)(&d->flg_energypos);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_data_flg_energyvel_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (uint8_t*)(&d->flg_energyvel);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_data_flg_subtreevel_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (uint8_t*)(&d->flg_subtreevel);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_data_flg_rnepost_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (uint8_t*)(&d->flg_rnepost);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_time_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(&d->time);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_energy_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->energy);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_data_buffer_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (uint8_t*)(d->buffer);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_data_arena_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (uint8_t*)(d->arena);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qvel_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qvel);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_act_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->act);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_history_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->history);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qacc_warmstart_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qacc_warmstart);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_plugin_state_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->plugin_state);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_ctrl_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->ctrl);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_applied_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_applied);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_xfrc_applied_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->xfrc_applied);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_data_eq_active_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (uint8_t*)(d->eq_active);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_mocap_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->mocap_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_mocap_quat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->mocap_quat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qacc_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qacc);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_act_dot_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->act_dot);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_userdata_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->userdata);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_sensordata_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->sensordata);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_tree_asleep_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->tree_asleep);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_plugin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->plugin);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_plugin_data_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->plugin_data);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_xpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->xpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_xquat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->xquat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_xmat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->xmat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_xipos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->xipos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_ximat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->ximat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_xanchor_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->xanchor);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_xaxis_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->xaxis);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_geom_xpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->geom_xpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_geom_xmat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->geom_xmat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_site_xpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->site_xpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_site_xmat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->site_xmat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cam_xpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cam_xpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cam_xmat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cam_xmat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_light_xpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->light_xpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_light_xdir_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->light_xdir);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_subtree_com_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->subtree_com);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cdof_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cdof);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cinert_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cinert);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_flexvert_xpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->flexvert_xpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_flexelem_aabb_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->flexelem_aabb);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_flexedge_J_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->flexedge_J);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_flexedge_length_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->flexedge_length);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_flexvert_J_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->flexvert_J);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_flexvert_length_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->flexvert_length);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_bvh_aabb_dyn_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->bvh_aabb_dyn);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_ten_wrapadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->ten_wrapadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_ten_wrapnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->ten_wrapnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_ten_J_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->ten_J_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_ten_J_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->ten_J_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_ten_J_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->ten_J_colind);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_ten_J_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->ten_J);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_ten_length_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->ten_length);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_wrap_obj_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->wrap_obj);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_wrap_xpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->wrap_xpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_actuator_length_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->actuator_length);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_moment_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->moment_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_moment_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->moment_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_moment_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->moment_colind);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_actuator_moment_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->actuator_moment);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_crb_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->crb);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qM_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qM);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_M_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->M);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qLD_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qLD);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qLDiagInv_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qLDiagInv);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_data_bvh_active_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (uint8_t*)(d->bvh_active);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_tree_awake_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->tree_awake);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_body_awake_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->body_awake);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_body_awake_ind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->body_awake_ind);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_parent_awake_ind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->parent_awake_ind);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_dof_awake_ind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->dof_awake_ind);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_flexedge_velocity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->flexedge_velocity);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_ten_velocity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->ten_velocity);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_actuator_velocity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->actuator_velocity);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cvel_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cvel);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cdof_dot_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cdof_dot);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_bias_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_bias);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_spring_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_spring);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_damper_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_damper);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_gravcomp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_gravcomp);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_fluid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_fluid);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_passive_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_passive);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_subtree_linvel_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->subtree_linvel);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_subtree_angmom_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->subtree_angmom);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qH_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qH);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qHDiagInv_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qHDiagInv);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qDeriv_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qDeriv);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qLU_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qLU);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_actuator_force_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->actuator_force);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_actuator_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_actuator);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_smooth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_smooth);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qacc_smooth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qacc_smooth);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_constraint_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_constraint);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_qfrc_inverse_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->qfrc_inverse);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cacc_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cacc);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cfrc_int_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cfrc_int);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_cfrc_ext_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->cfrc_ext);
}
EMSCRIPTEN_KEEPALIVE mjContact* mjwf_data_contact_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (mjContact*)(d->contact);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_id_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_id);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_J_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_J_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_J_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_J_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_J_rowsuper_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_J_rowsuper);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_J_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_J_colind);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_J_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_J);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_pos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_margin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_margin);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_frictionloss_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_frictionloss);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_diagApprox_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_diagApprox);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_KBIP_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_KBIP);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_D_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_D);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_R_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_R);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_tendon_efcadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->tendon_efcadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_tree_island_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->tree_island);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_ntree_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_ntree);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_itreeadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_itreeadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_map_itree2tree_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->map_itree2tree);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_dof_island_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->dof_island);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_nv_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_nv);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_idofadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_idofadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_dofadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_dofadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_map_dof2idof_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->map_dof2idof);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_map_idof2dof_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->map_idof2dof);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_ifrc_smooth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->ifrc_smooth);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iacc_smooth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iacc_smooth);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iM_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iM_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iM_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iM_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iM_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iM_colind);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iM_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iM);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iLD_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iLD);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iLDiagInv_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iLDiagInv);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iacc_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iacc);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_island_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_island);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_ne_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_ne);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_nf_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_nf);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_nefc_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_nefc);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_island_iefcadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->island_iefcadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_map_efc2iefc_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->map_efc2iefc);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_map_iefc2efc_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->map_iefc2efc);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iefc_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iefc_type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iefc_id_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iefc_id);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iefc_J_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iefc_J_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iefc_J_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iefc_J_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iefc_J_rowsuper_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iefc_J_rowsuper);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iefc_J_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iefc_J_colind);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iefc_J_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iefc_J);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iefc_frictionloss_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iefc_frictionloss);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iefc_D_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iefc_D);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iefc_R_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iefc_R);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_AR_rownnz_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_AR_rownnz);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_AR_rowadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_AR_rowadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_AR_colind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_AR_colind);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_AR_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_AR);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_vel_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_vel);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_aref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_aref);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_b_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_b);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iefc_aref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iefc_aref);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_iefc_state_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->iefc_state);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_iefc_force_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->iefc_force);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_efc_state_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(d->efc_state);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_efc_force_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->efc_force);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_ifrc_constraint_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (double*)(d->ifrc_constraint);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_threadpool_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjData* d = _mjwf_data_of(h);
  if (!d) return NULL;
  return (int32_t*)(&d->threadpool);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_maxgeom_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(&s->maxgeom);
}
EMSCRIPTEN_KEEPALIVE mjvGeom* mjwf_scene_geoms_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (mjvGeom*)(s->geoms);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_geomorder_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->geomorder);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_flexedgeadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->flexedgeadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_flexedgenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->flexedgenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_flexvertadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->flexvertadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_flexvertnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->flexvertnum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_flexfaceadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->flexfaceadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_flexfacenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->flexfacenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_flexfaceused_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->flexfaceused);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_flexedge_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->flexedge);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_flexvert_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->flexvert);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_flexface_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->flexface);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_flexnormal_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->flexnormal);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_flextexcoord_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->flextexcoord);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_flexvertopt_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (uint8_t*)(&s->flexvertopt);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_flexedgeopt_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (uint8_t*)(&s->flexedgeopt);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_flexfaceopt_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (uint8_t*)(&s->flexfaceopt);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_flexskinopt_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (uint8_t*)(&s->flexskinopt);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_skinfacenum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->skinfacenum);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_skinvertadr_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->skinvertadr);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_skinvertnum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(s->skinvertnum);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_skinvert_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->skinvert);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_skinnormal_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->skinnormal);
}
EMSCRIPTEN_KEEPALIVE mjvLight* mjwf_scene_lights_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (mjvLight*)(s->lights);
}
EMSCRIPTEN_KEEPALIVE mjvGLCamera* mjwf_scene_camera_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (mjvGLCamera*)(s->camera);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_enabletransform_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (uint8_t*)(&s->enabletransform);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_translate_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->translate);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_rotate_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->rotate);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_scale_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(&s->scale);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_stereo_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(&s->stereo);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_scene_flags_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (uint8_t*)(s->flags);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_framewidth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(&s->framewidth);
}
EMSCRIPTEN_KEEPALIVE float* mjwf_scene_framergb_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (float*)(s->framergb);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_scene_status_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return NULL;
  return (int32_t*)(&s->status);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_vopt_label_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (int32_t*)(&vopt->label);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_vopt_frame_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (int32_t*)(&vopt->frame);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_vopt_geomgroup_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (uint8_t*)(vopt->geomgroup);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_vopt_sitegroup_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (uint8_t*)(vopt->sitegroup);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_vopt_jointgroup_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (uint8_t*)(vopt->jointgroup);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_vopt_tendongroup_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (uint8_t*)(vopt->tendongroup);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_vopt_actuatorgroup_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (uint8_t*)(vopt->actuatorgroup);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_vopt_flexgroup_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (uint8_t*)(vopt->flexgroup);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_vopt_skingroup_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (uint8_t*)(vopt->skingroup);
}
EMSCRIPTEN_KEEPALIVE uint8_t* mjwf_vopt_flags_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (uint8_t*)(vopt->flags);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_vopt_bvh_depth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (int32_t*)(&vopt->bvh_depth);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_vopt_flex_layer_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvOption* vopt = _mjwf_vopt_of(h);
  if (!vopt) return NULL;
  return (int32_t*)(&vopt->flex_layer);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_cam_type_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvCamera* cam = _mjwf_cam_of(h);
  if (!cam) return NULL;
  return (int32_t*)(&cam->type);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_cam_fixedcamid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvCamera* cam = _mjwf_cam_of(h);
  if (!cam) return NULL;
  return (int32_t*)(&cam->fixedcamid);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_cam_trackbodyid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvCamera* cam = _mjwf_cam_of(h);
  if (!cam) return NULL;
  return (int32_t*)(&cam->trackbodyid);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_cam_lookat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvCamera* cam = _mjwf_cam_of(h);
  if (!cam) return NULL;
  return (double*)(cam->lookat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_cam_distance_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvCamera* cam = _mjwf_cam_of(h);
  if (!cam) return NULL;
  return (double*)(&cam->distance);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_cam_azimuth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvCamera* cam = _mjwf_cam_of(h);
  if (!cam) return NULL;
  return (double*)(&cam->azimuth);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_cam_elevation_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvCamera* cam = _mjwf_cam_of(h);
  if (!cam) return NULL;
  return (double*)(&cam->elevation);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_cam_orthographic_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvCamera* cam = _mjwf_cam_of(h);
  if (!cam) return NULL;
  return (int32_t*)(&cam->orthographic);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_pert_select_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (int32_t*)(&pert->select);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_pert_flexselect_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (int32_t*)(&pert->flexselect);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_pert_skinselect_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (int32_t*)(&pert->skinselect);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_pert_active_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (int32_t*)(&pert->active);
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_pert_active2_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (int32_t*)(&pert->active2);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_pert_refpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (double*)(pert->refpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_pert_refquat_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (double*)(pert->refquat);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_pert_refselpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (double*)(pert->refselpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_pert_localpos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (double*)(pert->localpos);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_pert_localmass_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (double*)(&pert->localmass);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_pert_scale_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjvPerturb* pert = _mjwf_pert_of(h);
  if (!pert) return NULL;
  return (double*)(&pert->scale);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nq(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nq);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nv(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nv);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nu(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nu);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_na(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->na);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nbody(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nbody);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nbvh(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nbvh);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nbvhstatic(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nbvhstatic);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nbvhdynamic(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nbvhdynamic);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_noct(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->noct);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_njnt(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->njnt);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ntree(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ntree);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nM(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nM);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nB(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nB);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nC(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nC);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nD(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nD);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ngeom(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ngeom);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nsite(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nsite);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ncam(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ncam);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nlight(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nlight);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflex(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflex);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflexnode(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflexnode);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflexvert(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflexvert);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflexedge(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflexedge);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflexelem(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflexelem);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflexelemdata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflexelemdata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflexelemedge(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflexelemedge);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflexshelldata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflexshelldata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflexevpair(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflexevpair);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nflextexcoord(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nflextexcoord);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nJfe(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nJfe);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nJfv(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nJfv);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmesh(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmesh);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmeshvert(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmeshvert);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmeshnormal(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmeshnormal);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmeshtexcoord(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmeshtexcoord);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmeshface(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmeshface);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmeshgraph(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmeshgraph);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmeshpoly(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmeshpoly);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmeshpolyvert(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmeshpolyvert);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmeshpolymap(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmeshpolymap);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nskin(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nskin);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nskinvert(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nskinvert);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nskintexvert(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nskintexvert);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nskinface(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nskinface);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nskinbone(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nskinbone);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nskinbonevert(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nskinbonevert);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nhfield(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nhfield);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nhfielddata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nhfielddata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ntex(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ntex);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ntexdata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ntexdata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmat(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmat);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_npair(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->npair);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nexclude(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nexclude);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_neq(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->neq);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ntendon(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ntendon);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nwrap(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nwrap);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nsensor(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nsensor);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nnumeric(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nnumeric);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nnumericdata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nnumericdata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ntext(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ntext);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ntextdata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ntextdata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ntuple(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ntuple);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ntupledata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ntupledata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nkey(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nkey);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nmocap(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nmocap);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nplugin(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nplugin);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_npluginattr(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->npluginattr);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuser_body(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuser_body);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuser_jnt(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuser_jnt);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuser_geom(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuser_geom);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuser_site(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuser_site);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuser_cam(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuser_cam);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuser_tendon(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuser_tendon);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuser_actuator(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuser_actuator);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuser_sensor(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuser_sensor);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nnames(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nnames);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_npaths(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->npaths);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nnames_map(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nnames_map);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nJmom(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nJmom);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_ngravcomp(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->ngravcomp);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nemax(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nemax);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_njmax(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->njmax);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nconmax(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nconmax);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nuserdata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nuserdata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nsensordata(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nsensordata);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_npluginstate(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->npluginstate);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nhistory(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nhistory);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_narena(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->narena);
}
EMSCRIPTEN_KEEPALIVE int mjwf_model_nbuffer(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjModel* m = _mjwf_model_of(h);
  if (!m) return 0;
  return (int)(m->nbuffer);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_narena(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->narena);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nbuffer(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nbuffer);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nplugin(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nplugin);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_ncon(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->ncon);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_ne(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->ne);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nf(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nf);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nl(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nl);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nefc(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nefc);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nJ(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nJ);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nA(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nA);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nisland(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nisland);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nidof(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nidof);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_ntree_awake(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->ntree_awake);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nbody_awake(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nbody_awake);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nparent_awake(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nparent_awake);
}
EMSCRIPTEN_KEEPALIVE int mjwf_data_nv_awake(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjData* d = _mjwf_data_of(h);
  if (!d) return 0;
  return (int)(d->nv_awake);
}
EMSCRIPTEN_KEEPALIVE int mjwf_scene_ngeom(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return 0;
  return (int)(s->ngeom);
}
EMSCRIPTEN_KEEPALIVE int mjwf_scene_nflex(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return 0;
  return (int)(s->nflex);
}
EMSCRIPTEN_KEEPALIVE int mjwf_scene_nskin(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return 0;
  return (int)(s->nskin);
}
EMSCRIPTEN_KEEPALIVE int mjwf_scene_nlight(int h) {
  if (!mjwf_helper_valid(h)) return 0;
  mjvScene* s = _mjwf_scene_of(h);
  if (!s) return 0;
  return (int)(s->nlight);
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_timestep_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.timestep));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_impratio_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.impratio));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_tolerance_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.tolerance));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_ls_tolerance_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.ls_tolerance));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_noslip_tolerance_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.noslip_tolerance));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_ccd_tolerance_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.ccd_tolerance));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_sleep_tolerance_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.sleep_tolerance));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_gravity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.gravity[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_wind_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.wind[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_magnetic_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.magnetic[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_density_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.density));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_viscosity_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.viscosity));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_o_margin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.o_margin));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_o_solref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.o_solref[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_o_solimp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.o_solimp[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_opt_o_friction_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->opt.o_friction[0]));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_integrator_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.integrator));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_cone_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.cone));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_jacobian_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.jacobian));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_solver_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.solver));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_iterations_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.iterations));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_ls_iterations_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.ls_iterations));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_noslip_iterations_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.noslip_iterations));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_ccd_iterations_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.ccd_iterations));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_disableflags_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.disableflags));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_enableflags_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.enableflags));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_disableactuator_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.disableactuator));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_sdf_initpoints_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.sdf_initpoints));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_opt_sdf_iterations_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->opt.sdf_iterations));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_global_cameraid_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.global.cameraid));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_global_orthographic_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.global.orthographic));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_global_fovy_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.global.fovy));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_global_ipd_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.global.ipd));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_global_azimuth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.global.azimuth));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_global_elevation_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.global.elevation));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_global_linewidth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.global.linewidth));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_global_glow_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.global.glow));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_global_realtime_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.global.realtime));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_global_offwidth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.global.offwidth));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_global_offheight_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.global.offheight));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_global_ellipsoidinertia_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.global.ellipsoidinertia));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_global_bvactive_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.global.bvactive));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_quality_shadowsize_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.quality.shadowsize));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_quality_offsamples_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.quality.offsamples));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_quality_numslices_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.quality.numslices));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_quality_numstacks_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.quality.numstacks));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_quality_numquads_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.quality.numquads));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_headlight_ambient_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.headlight.ambient[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_headlight_diffuse_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.headlight.diffuse[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_headlight_specular_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.headlight.specular[0]));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_model_vis_headlight_active_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (int32_t*)(&(m->vis.headlight.active));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_stiffness_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.stiffness));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_stiffnessrot_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.stiffnessrot));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_force_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.force));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_torque_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.torque));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_alpha_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.alpha));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_fogstart_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.fogstart));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_fogend_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.fogend));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_znear_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.znear));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_zfar_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.zfar));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_haze_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.haze));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_shadowclip_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.shadowclip));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_shadowscale_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.shadowscale));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_map_actuatortendon_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.map.actuatortendon));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_forcewidth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.forcewidth));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_contactwidth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.contactwidth));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_contactheight_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.contactheight));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_connect_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.connect));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_com_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.com));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_camera_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.camera));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_light_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.light));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_selectpoint_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.selectpoint));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_jointlength_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.jointlength));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_jointwidth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.jointwidth));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_actuatorlength_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.actuatorlength));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_actuatorwidth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.actuatorwidth));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_framelength_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.framelength));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_framewidth_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.framewidth));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_constraint_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.constraint));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_slidercrank_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.slidercrank));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_scale_frustum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.scale.frustum));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_fog_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.fog[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_haze_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.haze[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_force_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.force[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_inertia_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.inertia[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_joint_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.joint[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_actuator_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.actuator[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_actuatornegative_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.actuatornegative[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_actuatorpositive_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.actuatorpositive[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_com_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.com[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_camera_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.camera[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_light_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.light[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_selectpoint_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.selectpoint[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_connect_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.connect[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_contactpoint_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.contactpoint[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_contactforce_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.contactforce[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_contactfriction_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.contactfriction[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_contacttorque_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.contacttorque[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_contactgap_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.contactgap[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_rangefinder_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.rangefinder[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_constraint_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.constraint[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_slidercrank_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.slidercrank[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_crankbroken_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.crankbroken[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_frustum_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.frustum[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_bv_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.bv[0]));
}
EMSCRIPTEN_KEEPALIVE float* mjwf_model_vis_rgba_bvactive_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (float*)(&(m->vis.rgba.bvactive[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_stat_meaninertia_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->stat.meaninertia));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_stat_meanmass_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->stat.meanmass));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_stat_meansize_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->stat.meansize));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_stat_extent_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->stat.extent));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_model_stat_center_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)d;
  if (!m) return NULL;
  return (double*)(&(m->stat.center[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_solver_improvement_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->solver[0].improvement));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_solver_gradient_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->solver[0].gradient));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_solver_lineslope_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->solver[0].lineslope));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_solver_nactive_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->solver[0].nactive));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_solver_nchange_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->solver[0].nchange));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_solver_neval_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->solver[0].neval));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_solver_nupdate_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->solver[0].nupdate));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_warning_lastinfo_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->warning[0].lastinfo));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_warning_number_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->warning[0].number));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_timer_duration_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->timer[0].duration));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_timer_number_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->timer[0].number));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_dist_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].dist));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_pos_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].pos[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_frame_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].frame[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_includemargin_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].includemargin));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_friction_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].friction[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_solref_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].solref[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_solreffriction_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].solreffriction[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_solimp_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].solimp[0]));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_mu_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].mu));
}
EMSCRIPTEN_KEEPALIVE double* mjwf_data_contact_H_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (double*)(&(d->contact[0].H[0]));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_dim_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].dim));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_geom1_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].geom1));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_geom2_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].geom2));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_geom_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].geom[0]));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_flex_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].flex[0]));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_elem_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].elem[0]));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_vert_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].vert[0]));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_exclude_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].exclude));
}
EMSCRIPTEN_KEEPALIVE int32_t* mjwf_data_contact_efc_address_ptr(int h) {
  if (!mjwf_helper_valid(h)) return NULL;
  mjModel* m = _mjwf_model_of(h);
  mjData* d  = _mjwf_data_of(h);
  (void)m;
  if (!d) return NULL;
  return (int32_t*)(&(d->contact[0].efc_address));
}
