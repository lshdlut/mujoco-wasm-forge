// AUTO-GENERATED: MuJoCo WASM function wrapper implementations.
// Source: abi_exports/gen_funcs.py
#include "mjwf_abi_funcs.h"

#if defined(__EMSCRIPTEN__)
#  include <emscripten/emscripten.h>
#  define MJWF_API_IMPL EMSCRIPTEN_KEEPALIVE __attribute__((used, visibility("default")))
#else
#  define MJWF_API_IMPL __attribute__((used, visibility("default")))
#endif

MJWF_API_IMPL void mjwf_mj_Euler(const mjModel * m, mjData * d) {
  mj_Euler(m, d);
}

MJWF_API_IMPL void mjwf_mj_RungeKutta(const mjModel * m, mjData * d, int N) {
  mj_RungeKutta(m, d, N);
}

MJWF_API_IMPL int mjwf_mj_addBufferVFS(mjVFS * vfs, const char * name, const void * buffer, int nbuffer) {
  return mj_addBufferVFS(vfs, name, buffer, nbuffer);
}

MJWF_API_IMPL int mjwf_mj_addContact(const mjModel * m, mjData * d, const mjContact * con) {
  return mj_addContact(m, d, con);
}

MJWF_API_IMPL int mjwf_mj_addFileVFS(mjVFS * vfs, const char * directory, const char * filename) {
  return mj_addFileVFS(vfs, directory, filename);
}

MJWF_API_IMPL void mjwf_mj_addM(const mjModel * m, mjData * d, mjtNum * dst, int * rownnz, int * rowadr, int * colind) {
  mj_addM(m, d, dst, rownnz, rowadr, colind);
}

MJWF_API_IMPL void mjwf_mj_angmomMat(const mjModel * m, mjData * d, mjtNum * mat, int body) {
  mj_angmomMat(m, d, mat, body);
}

MJWF_API_IMPL void mjwf_mj_applyFT(const mjModel * m, mjData * d, const mjtNum force[3], const mjtNum torque[3], const mjtNum point[3], int body, mjtNum * qfrc_target) {
  mj_applyFT(m, d, force, torque, point, body, qfrc_target);
}

MJWF_API_IMPL void mjwf_mj_camlight(const mjModel * m, mjData * d) {
  mj_camlight(m, d);
}

MJWF_API_IMPL void mjwf_mj_checkAcc(const mjModel * m, mjData * d) {
  mj_checkAcc(m, d);
}

MJWF_API_IMPL void mjwf_mj_checkPos(const mjModel * m, mjData * d) {
  mj_checkPos(m, d);
}

MJWF_API_IMPL void mjwf_mj_checkVel(const mjModel * m, mjData * d) {
  mj_checkVel(m, d);
}

MJWF_API_IMPL void mjwf_mj_clearCache(mjCache * cache) {
  mj_clearCache(cache);
}

MJWF_API_IMPL void mjwf_mj_collision(const mjModel * m, mjData * d) {
  mj_collision(m, d);
}

MJWF_API_IMPL void mjwf_mj_comPos(const mjModel * m, mjData * d) {
  mj_comPos(m, d);
}

MJWF_API_IMPL void mjwf_mj_comVel(const mjModel * m, mjData * d) {
  mj_comVel(m, d);
}

MJWF_API_IMPL void mjwf_mj_compareFwdInv(const mjModel * m, mjData * d) {
  mj_compareFwdInv(m, d);
}

MJWF_API_IMPL mjModel * mjwf_mj_compile(mjSpec * s, const mjVFS * vfs) {
  return mj_compile(s, vfs);
}

MJWF_API_IMPL void mjwf_mj_constraintUpdate(const mjModel * m, mjData * d, const mjtNum * jar, mjtNum cost[1], int flg_coneHessian) {
  mj_constraintUpdate(m, d, jar, cost, flg_coneHessian);
}

MJWF_API_IMPL void mjwf_mj_contactForce(const mjModel * m, const mjData * d, int id, mjtNum result[6]) {
  mj_contactForce(m, d, id, result);
}

MJWF_API_IMPL int mjwf_mj_copyBack(mjSpec * s, const mjModel * m) {
  return mj_copyBack(s, m);
}

MJWF_API_IMPL mjData * mjwf_mj_copyData(mjData * dest, const mjModel * m, const mjData * src) {
  return mj_copyData(dest, m, src);
}

MJWF_API_IMPL mjModel * mjwf_mj_copyModel(mjModel * dest, const mjModel * src) {
  return mj_copyModel(dest, src);
}

MJWF_API_IMPL mjSpec * mjwf_mj_copySpec(const mjSpec * s) {
  return mj_copySpec(s);
}

MJWF_API_IMPL void mjwf_mj_crb(const mjModel * m, mjData * d) {
  mj_crb(m, d);
}

MJWF_API_IMPL void mjwf_mj_defaultLROpt(mjLROpt * opt) {
  mj_defaultLROpt(opt);
}

MJWF_API_IMPL void mjwf_mj_defaultOption(mjOption * opt) {
  mj_defaultOption(opt);
}

MJWF_API_IMPL void mjwf_mj_defaultSolRefImp(mjtNum * solref, mjtNum * solimp) {
  mj_defaultSolRefImp(solref, solimp);
}

MJWF_API_IMPL void mjwf_mj_defaultVFS(mjVFS * vfs) {
  mj_defaultVFS(vfs);
}

MJWF_API_IMPL void mjwf_mj_defaultVisual(mjVisual * vis) {
  mj_defaultVisual(vis);
}

MJWF_API_IMPL void mjwf_mj_deleteData(mjData * d) {
  mj_deleteData(d);
}

MJWF_API_IMPL int mjwf_mj_deleteFileVFS(mjVFS * vfs, const char * filename) {
  return mj_deleteFileVFS(vfs, filename);
}

MJWF_API_IMPL void mjwf_mj_deleteModel(mjModel * m) {
  mj_deleteModel(m);
}

MJWF_API_IMPL void mjwf_mj_deleteSpec(mjSpec * s) {
  mj_deleteSpec(s);
}

MJWF_API_IMPL void mjwf_mj_deleteVFS(mjVFS * vfs) {
  mj_deleteVFS(vfs);
}

MJWF_API_IMPL void mjwf_mj_differentiatePos(const mjModel * m, mjtNum * qvel, mjtNum dt, const mjtNum * qpos1, const mjtNum * qpos2) {
  mj_differentiatePos(m, qvel, dt, qpos1, qpos2);
}

MJWF_API_IMPL void mjwf_mj_energyPos(const mjModel * m, mjData * d) {
  mj_energyPos(m, d);
}

MJWF_API_IMPL void mjwf_mj_energyVel(const mjModel * m, mjData * d) {
  mj_energyVel(m, d);
}

MJWF_API_IMPL void mjwf_mj_factorM(const mjModel * m, mjData * d) {
  mj_factorM(m, d);
}

MJWF_API_IMPL void mjwf_mj_flex(const mjModel * m, mjData * d) {
  mj_flex(m, d);
}

MJWF_API_IMPL void mjwf_mj_forward(const mjModel * m, mjData * d) {
  mj_forward(m, d);
}

MJWF_API_IMPL void mjwf_mj_forwardSkip(const mjModel * m, mjData * d, int skipstage, int skipsensor) {
  mj_forwardSkip(m, d, skipstage, skipsensor);
}

MJWF_API_IMPL void mjwf_mj_freeLastXML(void) {
  mj_freeLastXML();
}

MJWF_API_IMPL void mjwf_mj_freeStack(mjData * d) {
  mj_freeStack(d);
}

MJWF_API_IMPL void mjwf_mj_fullM(const mjModel * m, mjtNum * dst, const mjtNum * M) {
  mj_fullM(m, dst, M);
}

MJWF_API_IMPL void mjwf_mj_fwdAcceleration(const mjModel * m, mjData * d) {
  mj_fwdAcceleration(m, d);
}

MJWF_API_IMPL void mjwf_mj_fwdActuation(const mjModel * m, mjData * d) {
  mj_fwdActuation(m, d);
}

MJWF_API_IMPL void mjwf_mj_fwdConstraint(const mjModel * m, mjData * d) {
  mj_fwdConstraint(m, d);
}

MJWF_API_IMPL void mjwf_mj_fwdPosition(const mjModel * m, mjData * d) {
  mj_fwdPosition(m, d);
}

MJWF_API_IMPL void mjwf_mj_fwdVelocity(const mjModel * m, mjData * d) {
  mj_fwdVelocity(m, d);
}

MJWF_API_IMPL mjtNum mjwf_mj_geomDistance(const mjModel * m, const mjData * d, int geom1, int geom2, mjtNum distmax, mjtNum fromto[6]) {
  return mj_geomDistance(m, d, geom1, geom2, distmax, fromto);
}

MJWF_API_IMPL mjCache * mjwf_mj_getCache(void) {
  return mj_getCache();
}

MJWF_API_IMPL size_t mjwf_mj_getCacheCapacity(const mjCache * cache) {
  return mj_getCacheCapacity(cache);
}

MJWF_API_IMPL size_t mjwf_mj_getCacheSize(const mjCache * cache) {
  return mj_getCacheSize(cache);
}

MJWF_API_IMPL const char * mjwf_mj_getPluginConfig(const mjModel * m, int plugin_id, const char * attrib) {
  return mj_getPluginConfig(m, plugin_id, attrib);
}

MJWF_API_IMPL void mjwf_mj_getState(const mjModel * m, const mjData * d, mjtNum * state, unsigned int sig) {
  mj_getState(m, d, state, sig);
}

MJWF_API_IMPL mjtNum mjwf_mj_getTotalmass(const mjModel * m) {
  return mj_getTotalmass(m);
}

MJWF_API_IMPL const char * mjwf_mj_id2name(const mjModel * m, int type, int id) {
  return mj_id2name(m, type, id);
}

MJWF_API_IMPL void mjwf_mj_implicit(const mjModel * m, mjData * d) {
  mj_implicit(m, d);
}

MJWF_API_IMPL void mjwf_mj_integratePos(const mjModel * m, mjtNum * qpos, const mjtNum * qvel, mjtNum dt) {
  mj_integratePos(m, qpos, qvel, dt);
}

MJWF_API_IMPL void mjwf_mj_invConstraint(const mjModel * m, mjData * d) {
  mj_invConstraint(m, d);
}

MJWF_API_IMPL void mjwf_mj_invPosition(const mjModel * m, mjData * d) {
  mj_invPosition(m, d);
}

MJWF_API_IMPL void mjwf_mj_invVelocity(const mjModel * m, mjData * d) {
  mj_invVelocity(m, d);
}

MJWF_API_IMPL void mjwf_mj_inverse(const mjModel * m, mjData * d) {
  mj_inverse(m, d);
}

MJWF_API_IMPL void mjwf_mj_inverseSkip(const mjModel * m, mjData * d, int skipstage, int skipsensor) {
  mj_inverseSkip(m, d, skipstage, skipsensor);
}

MJWF_API_IMPL int mjwf_mj_isDual(const mjModel * m) {
  return mj_isDual(m);
}

MJWF_API_IMPL int mjwf_mj_isPyramidal(const mjModel * m) {
  return mj_isPyramidal(m);
}

MJWF_API_IMPL int mjwf_mj_isSparse(const mjModel * m) {
  return mj_isSparse(m);
}

MJWF_API_IMPL void mjwf_mj_island(const mjModel * m, mjData * d) {
  mj_island(m, d);
}

MJWF_API_IMPL void mjwf_mj_jac(const mjModel * m, const mjData * d, mjtNum * jacp, mjtNum * jacr, const mjtNum point[3], int body) {
  mj_jac(m, d, jacp, jacr, point, body);
}

MJWF_API_IMPL void mjwf_mj_jacBody(const mjModel * m, const mjData * d, mjtNum * jacp, mjtNum * jacr, int body) {
  mj_jacBody(m, d, jacp, jacr, body);
}

MJWF_API_IMPL void mjwf_mj_jacBodyCom(const mjModel * m, const mjData * d, mjtNum * jacp, mjtNum * jacr, int body) {
  mj_jacBodyCom(m, d, jacp, jacr, body);
}

MJWF_API_IMPL void mjwf_mj_jacDot(const mjModel * m, const mjData * d, mjtNum * jacp, mjtNum * jacr, const mjtNum point[3], int body) {
  mj_jacDot(m, d, jacp, jacr, point, body);
}

MJWF_API_IMPL void mjwf_mj_jacGeom(const mjModel * m, const mjData * d, mjtNum * jacp, mjtNum * jacr, int geom) {
  mj_jacGeom(m, d, jacp, jacr, geom);
}

MJWF_API_IMPL void mjwf_mj_jacPointAxis(const mjModel * m, mjData * d, mjtNum * jacPoint, mjtNum * jacAxis, const mjtNum point[3], const mjtNum axis[3], int body) {
  mj_jacPointAxis(m, d, jacPoint, jacAxis, point, axis, body);
}

MJWF_API_IMPL void mjwf_mj_jacSite(const mjModel * m, const mjData * d, mjtNum * jacp, mjtNum * jacr, int site) {
  mj_jacSite(m, d, jacp, jacr, site);
}

MJWF_API_IMPL void mjwf_mj_jacSubtreeCom(const mjModel * m, mjData * d, mjtNum * jacp, int body) {
  mj_jacSubtreeCom(m, d, jacp, body);
}

MJWF_API_IMPL void mjwf_mj_kinematics(const mjModel * m, mjData * d) {
  mj_kinematics(m, d);
}

MJWF_API_IMPL void mjwf_mj_loadAllPluginLibraries(const char * directory, mjfPluginLibraryLoadCallback callback) {
  mj_loadAllPluginLibraries(directory, callback);
}

MJWF_API_IMPL mjModel * mjwf_mj_loadModel(const char * filename, const mjVFS * vfs) {
  return mj_loadModel(filename, vfs);
}

MJWF_API_IMPL void mjwf_mj_loadPluginLibrary(const char * path) {
  mj_loadPluginLibrary(path);
}

MJWF_API_IMPL mjModel * mjwf_mj_loadXML(const char * filename, const mjVFS * vfs, char * error, int error_sz) {
  return mj_loadXML(filename, vfs, error, error_sz);
}

MJWF_API_IMPL void mjwf_mj_local2Global(mjData * d, mjtNum xpos[3], mjtNum xmat[9], const mjtNum pos[3], const mjtNum quat[4], int body, mjtByte sameframe) {
  mj_local2Global(d, xpos, xmat, pos, quat, body, sameframe);
}

MJWF_API_IMPL void mjwf_mj_makeConstraint(const mjModel * m, mjData * d) {
  mj_makeConstraint(m, d);
}

MJWF_API_IMPL mjData * mjwf_mj_makeData(const mjModel * m) {
  return mj_makeData(m);
}

MJWF_API_IMPL void mjwf_mj_makeM(const mjModel * m, mjData * d) {
  mj_makeM(m, d);
}

MJWF_API_IMPL mjSpec * mjwf_mj_makeSpec(void) {
  return mj_makeSpec();
}

MJWF_API_IMPL void mjwf_mj_markStack(mjData * d) {
  mj_markStack(d);
}

MJWF_API_IMPL void mjwf_mj_mulJacTVec(const mjModel * m, const mjData * d, mjtNum * res, const mjtNum * vec) {
  mj_mulJacTVec(m, d, res, vec);
}

MJWF_API_IMPL void mjwf_mj_mulJacVec(const mjModel * m, const mjData * d, mjtNum * res, const mjtNum * vec) {
  mj_mulJacVec(m, d, res, vec);
}

MJWF_API_IMPL void mjwf_mj_mulM(const mjModel * m, const mjData * d, mjtNum * res, const mjtNum * vec) {
  mj_mulM(m, d, res, vec);
}

MJWF_API_IMPL void mjwf_mj_mulM2(const mjModel * m, const mjData * d, mjtNum * res, const mjtNum * vec) {
  mj_mulM2(m, d, res, vec);
}

MJWF_API_IMPL void mjwf_mj_multiRay(const mjModel * m, mjData * d, const mjtNum pnt[3], const mjtNum * vec, const mjtByte * geomgroup, mjtByte flg_static, int bodyexclude, int * geomid, mjtNum * dist, int nray, mjtNum cutoff) {
  mj_multiRay(m, d, pnt, vec, geomgroup, flg_static, bodyexclude, geomid, dist, nray, cutoff);
}

MJWF_API_IMPL int mjwf_mj_name2id(const mjModel * m, int type, const char * name) {
  return mj_name2id(m, type, name);
}

MJWF_API_IMPL void mjwf_mj_normalizeQuat(const mjModel * m, mjtNum * qpos) {
  mj_normalizeQuat(m, qpos);
}

MJWF_API_IMPL void mjwf_mj_objectAcceleration(const mjModel * m, const mjData * d, int objtype, int objid, mjtNum res[6], int flg_local) {
  mj_objectAcceleration(m, d, objtype, objid, res, flg_local);
}

MJWF_API_IMPL void mjwf_mj_objectVelocity(const mjModel * m, const mjData * d, int objtype, int objid, mjtNum res[6], int flg_local) {
  mj_objectVelocity(m, d, objtype, objid, res, flg_local);
}

MJWF_API_IMPL mjSpec * mjwf_mj_parseXML(const char * filename, const mjVFS * vfs, char * error, int error_sz) {
  return mj_parseXML(filename, vfs, error, error_sz);
}

MJWF_API_IMPL mjSpec * mjwf_mj_parseXMLString(const char * xml, const mjVFS * vfs, char * error, int error_sz) {
  return mj_parseXMLString(xml, vfs, error, error_sz);
}

MJWF_API_IMPL void mjwf_mj_passive(const mjModel * m, mjData * d) {
  mj_passive(m, d);
}

MJWF_API_IMPL void mjwf_mj_printData(const mjModel * m, const mjData * d, const char * filename) {
  mj_printData(m, d, filename);
}

MJWF_API_IMPL void mjwf_mj_printFormattedData(const mjModel * m, const mjData * d, const char * filename, const char * float_format) {
  mj_printFormattedData(m, d, filename, float_format);
}

MJWF_API_IMPL void mjwf_mj_printFormattedModel(const mjModel * m, const char * filename, const char * float_format) {
  mj_printFormattedModel(m, filename, float_format);
}

MJWF_API_IMPL void mjwf_mj_printFormattedScene(const mjvScene * s, const char * filename, const char * float_format) {
  mj_printFormattedScene(s, filename, float_format);
}

MJWF_API_IMPL void mjwf_mj_printModel(const mjModel * m, const char * filename) {
  mj_printModel(m, filename);
}

MJWF_API_IMPL void mjwf_mj_printScene(const mjvScene * s, const char * filename) {
  mj_printScene(s, filename);
}

MJWF_API_IMPL int mjwf_mj_printSchema(const char * filename, char * buffer, int buffer_sz, int flg_html, int flg_pad) {
  return mj_printSchema(filename, buffer, buffer_sz, flg_html, flg_pad);
}

MJWF_API_IMPL void mjwf_mj_projectConstraint(const mjModel * m, mjData * d) {
  mj_projectConstraint(m, d);
}

MJWF_API_IMPL mjtNum mjwf_mj_ray(const mjModel * m, const mjData * d, const mjtNum pnt[3], const mjtNum vec[3], const mjtByte * geomgroup, mjtByte flg_static, int bodyexclude, int geomid[1]) {
  return mj_ray(m, d, pnt, vec, geomgroup, flg_static, bodyexclude, geomid);
}

MJWF_API_IMPL mjtNum mjwf_mj_rayHfield(const mjModel * m, const mjData * d, int geomid, const mjtNum pnt[3], const mjtNum vec[3]) {
  return mj_rayHfield(m, d, geomid, pnt, vec);
}

MJWF_API_IMPL mjtNum mjwf_mj_rayMesh(const mjModel * m, const mjData * d, int geomid, const mjtNum pnt[3], const mjtNum vec[3]) {
  return mj_rayMesh(m, d, geomid, pnt, vec);
}

MJWF_API_IMPL int mjwf_mj_recompile(mjSpec * s, const mjVFS * vfs, mjModel * m, mjData * d) {
  return mj_recompile(s, vfs, m, d);
}

MJWF_API_IMPL void mjwf_mj_referenceConstraint(const mjModel * m, mjData * d) {
  mj_referenceConstraint(m, d);
}

MJWF_API_IMPL void mjwf_mj_resetCallbacks(void) {
  mj_resetCallbacks();
}

MJWF_API_IMPL void mjwf_mj_resetData(const mjModel * m, mjData * d) {
  mj_resetData(m, d);
}

MJWF_API_IMPL void mjwf_mj_resetDataDebug(const mjModel * m, mjData * d, unsigned char debug_value) {
  mj_resetDataDebug(m, d, debug_value);
}

MJWF_API_IMPL void mjwf_mj_resetDataKeyframe(const mjModel * m, mjData * d, int key) {
  mj_resetDataKeyframe(m, d, key);
}

MJWF_API_IMPL void mjwf_mj_rne(const mjModel * m, mjData * d, int flg_acc, mjtNum * result) {
  mj_rne(m, d, flg_acc, result);
}

MJWF_API_IMPL void mjwf_mj_rnePostConstraint(const mjModel * m, mjData * d) {
  mj_rnePostConstraint(m, d);
}

MJWF_API_IMPL int mjwf_mj_saveLastXML(const char * filename, const mjModel * m, char * error, int error_sz) {
  return mj_saveLastXML(filename, m, error, error_sz);
}

MJWF_API_IMPL void mjwf_mj_saveModel(const mjModel * m, const char * filename, void * buffer, int buffer_sz) {
  mj_saveModel(m, filename, buffer, buffer_sz);
}

MJWF_API_IMPL int mjwf_mj_saveXML(const mjSpec * s, const char * filename, char * error, int error_sz) {
  return mj_saveXML(s, filename, error, error_sz);
}

MJWF_API_IMPL int mjwf_mj_saveXMLString(const mjSpec * s, char * xml, int xml_sz, char * error, int error_sz) {
  return mj_saveXMLString(s, xml, xml_sz, error, error_sz);
}

MJWF_API_IMPL void mjwf_mj_sensorAcc(const mjModel * m, mjData * d) {
  mj_sensorAcc(m, d);
}

MJWF_API_IMPL void mjwf_mj_sensorPos(const mjModel * m, mjData * d) {
  mj_sensorPos(m, d);
}

MJWF_API_IMPL void mjwf_mj_sensorVel(const mjModel * m, mjData * d) {
  mj_sensorVel(m, d);
}

MJWF_API_IMPL size_t mjwf_mj_setCacheCapacity(mjCache * cache, size_t size) {
  return mj_setCacheCapacity(cache, size);
}

MJWF_API_IMPL void mjwf_mj_setConst(mjModel * m, mjData * d) {
  mj_setConst(m, d);
}

MJWF_API_IMPL void mjwf_mj_setKeyframe(mjModel * m, const mjData * d, int k) {
  mj_setKeyframe(m, d, k);
}

MJWF_API_IMPL int mjwf_mj_setLengthRange(mjModel * m, mjData * d, int index, const mjLROpt * opt, char * error, int error_sz) {
  return mj_setLengthRange(m, d, index, opt, error, error_sz);
}

MJWF_API_IMPL void mjwf_mj_setState(const mjModel * m, mjData * d, const mjtNum * state, unsigned int sig) {
  mj_setState(m, d, state, sig);
}

MJWF_API_IMPL void mjwf_mj_setTotalmass(mjModel * m, mjtNum newmass) {
  mj_setTotalmass(m, newmass);
}

MJWF_API_IMPL int mjwf_mj_sizeModel(const mjModel * m) {
  return mj_sizeModel(m);
}

MJWF_API_IMPL void mjwf_mj_solveM(const mjModel * m, mjData * d, mjtNum * x, const mjtNum * y, int n) {
  mj_solveM(m, d, x, y, n);
}

MJWF_API_IMPL void mjwf_mj_solveM2(const mjModel * m, mjData * d, mjtNum * x, const mjtNum * y, const mjtNum * sqrtInvD, int n) {
  mj_solveM2(m, d, x, y, sqrtInvD, n);
}

MJWF_API_IMPL void * mjwf_mj_stackAllocByte(mjData * d, size_t bytes, size_t alignment) {
  return mj_stackAllocByte(d, bytes, alignment);
}

MJWF_API_IMPL int * mjwf_mj_stackAllocInt(mjData * d, size_t size) {
  return mj_stackAllocInt(d, size);
}

MJWF_API_IMPL mjtNum * mjwf_mj_stackAllocNum(mjData * d, size_t size) {
  return mj_stackAllocNum(d, size);
}

MJWF_API_IMPL int mjwf_mj_stateSize(const mjModel * m, unsigned int sig) {
  return mj_stateSize(m, sig);
}

MJWF_API_IMPL void mjwf_mj_step(const mjModel * m, mjData * d) {
  mj_step(m, d);
}

MJWF_API_IMPL void mjwf_mj_step1(const mjModel * m, mjData * d) {
  mj_step1(m, d);
}

MJWF_API_IMPL void mjwf_mj_step2(const mjModel * m, mjData * d) {
  mj_step2(m, d);
}

MJWF_API_IMPL void mjwf_mj_subtreeVel(const mjModel * m, mjData * d) {
  mj_subtreeVel(m, d);
}

MJWF_API_IMPL void mjwf_mj_tendon(const mjModel * m, mjData * d) {
  mj_tendon(m, d);
}

MJWF_API_IMPL void mjwf_mj_transmission(const mjModel * m, mjData * d) {
  mj_transmission(m, d);
}

MJWF_API_IMPL int mjwf_mj_version(void) {
  return mj_version();
}

MJWF_API_IMPL const char * mjwf_mj_versionString(void) {
  return mj_versionString();
}

MJWF_API_IMPL void mjwf_mj_warning(mjData * d, int warning, int info) {
  mj_warning(d, warning, info);
}

MJWF_API_IMPL void mjwf_mjd_inverseFD(const mjModel * m, mjData * d, mjtNum eps, mjtByte flg_actuation, mjtNum * DfDq, mjtNum * DfDv, mjtNum * DfDa, mjtNum * DsDq, mjtNum * DsDv, mjtNum * DsDa, mjtNum * DmDq) {
  mjd_inverseFD(m, d, eps, flg_actuation, DfDq, DfDv, DfDa, DsDq, DsDv, DsDa, DmDq);
}

MJWF_API_IMPL void mjwf_mjd_quatIntegrate(const mjtNum vel[3], mjtNum scale, mjtNum Dquat[9], mjtNum Dvel[9], mjtNum Dscale[3]) {
  mjd_quatIntegrate(vel, scale, Dquat, Dvel, Dscale);
}

MJWF_API_IMPL void mjwf_mjd_subQuat(const mjtNum qa[4], const mjtNum qb[4], mjtNum Da[9], mjtNum Db[9]) {
  mjd_subQuat(qa, qb, Da, Db);
}

MJWF_API_IMPL void mjwf_mjd_transitionFD(const mjModel * m, mjData * d, mjtNum eps, mjtByte flg_centered, mjtNum * A, mjtNum * B, mjtNum * C, mjtNum * D) {
  mjd_transitionFD(m, d, eps, flg_centered, A, B, C, D);
}

MJWF_API_IMPL int mjwf_mjs_activatePlugin(mjSpec * s, const char * name) {
  return mjs_activatePlugin(s, name);
}

MJWF_API_IMPL mjsActuator * mjwf_mjs_addActuator(mjSpec * s, const mjsDefault * def) {
  return mjs_addActuator(s, def);
}

MJWF_API_IMPL mjsBody * mjwf_mjs_addBody(mjsBody * body, const mjsDefault * def) {
  return mjs_addBody(body, def);
}

MJWF_API_IMPL mjsCamera * mjwf_mjs_addCamera(mjsBody * body, const mjsDefault * def) {
  return mjs_addCamera(body, def);
}

MJWF_API_IMPL mjsDefault * mjwf_mjs_addDefault(mjSpec * s, const char * classname, const mjsDefault * parent) {
  return mjs_addDefault(s, classname, parent);
}

MJWF_API_IMPL mjsEquality * mjwf_mjs_addEquality(mjSpec * s, const mjsDefault * def) {
  return mjs_addEquality(s, def);
}

MJWF_API_IMPL mjsExclude * mjwf_mjs_addExclude(mjSpec * s) {
  return mjs_addExclude(s);
}

MJWF_API_IMPL mjsFlex * mjwf_mjs_addFlex(mjSpec * s) {
  return mjs_addFlex(s);
}

MJWF_API_IMPL mjsFrame * mjwf_mjs_addFrame(mjsBody * body, mjsFrame * parentframe) {
  return mjs_addFrame(body, parentframe);
}

MJWF_API_IMPL mjsJoint * mjwf_mjs_addFreeJoint(mjsBody * body) {
  return mjs_addFreeJoint(body);
}

MJWF_API_IMPL mjsGeom * mjwf_mjs_addGeom(mjsBody * body, const mjsDefault * def) {
  return mjs_addGeom(body, def);
}

MJWF_API_IMPL mjsHField * mjwf_mjs_addHField(mjSpec * s) {
  return mjs_addHField(s);
}

MJWF_API_IMPL mjsJoint * mjwf_mjs_addJoint(mjsBody * body, const mjsDefault * def) {
  return mjs_addJoint(body, def);
}

MJWF_API_IMPL mjsKey * mjwf_mjs_addKey(mjSpec * s) {
  return mjs_addKey(s);
}

MJWF_API_IMPL mjsLight * mjwf_mjs_addLight(mjsBody * body, const mjsDefault * def) {
  return mjs_addLight(body, def);
}

MJWF_API_IMPL mjsMaterial * mjwf_mjs_addMaterial(mjSpec * s, const mjsDefault * def) {
  return mjs_addMaterial(s, def);
}

MJWF_API_IMPL mjsMesh * mjwf_mjs_addMesh(mjSpec * s, const mjsDefault * def) {
  return mjs_addMesh(s, def);
}

MJWF_API_IMPL mjsNumeric * mjwf_mjs_addNumeric(mjSpec * s) {
  return mjs_addNumeric(s);
}

MJWF_API_IMPL mjsPair * mjwf_mjs_addPair(mjSpec * s, const mjsDefault * def) {
  return mjs_addPair(s, def);
}

MJWF_API_IMPL mjsPlugin * mjwf_mjs_addPlugin(mjSpec * s) {
  return mjs_addPlugin(s);
}

MJWF_API_IMPL mjsSensor * mjwf_mjs_addSensor(mjSpec * s) {
  return mjs_addSensor(s);
}

MJWF_API_IMPL mjsSite * mjwf_mjs_addSite(mjsBody * body, const mjsDefault * def) {
  return mjs_addSite(body, def);
}

MJWF_API_IMPL mjsSkin * mjwf_mjs_addSkin(mjSpec * s) {
  return mjs_addSkin(s);
}

MJWF_API_IMPL mjsTendon * mjwf_mjs_addTendon(mjSpec * s, const mjsDefault * def) {
  return mjs_addTendon(s, def);
}

MJWF_API_IMPL mjsText * mjwf_mjs_addText(mjSpec * s) {
  return mjs_addText(s);
}

MJWF_API_IMPL mjsTexture * mjwf_mjs_addTexture(mjSpec * s) {
  return mjs_addTexture(s);
}

MJWF_API_IMPL mjsTuple * mjwf_mjs_addTuple(mjSpec * s) {
  return mjs_addTuple(s);
}

MJWF_API_IMPL void mjwf_mjs_appendFloatVec(mjFloatVecVec * dest, const float * array, int size) {
  mjs_appendFloatVec(dest, array, size);
}

MJWF_API_IMPL void mjwf_mjs_appendIntVec(mjIntVecVec * dest, const int * array, int size) {
  mjs_appendIntVec(dest, array, size);
}

MJWF_API_IMPL void mjwf_mjs_appendString(mjStringVec * dest, const char * text) {
  mjs_appendString(dest, text);
}

MJWF_API_IMPL mjsActuator * mjwf_mjs_asActuator(mjsElement * element) {
  return mjs_asActuator(element);
}

MJWF_API_IMPL mjsBody * mjwf_mjs_asBody(mjsElement * element) {
  return mjs_asBody(element);
}

MJWF_API_IMPL mjsCamera * mjwf_mjs_asCamera(mjsElement * element) {
  return mjs_asCamera(element);
}

MJWF_API_IMPL mjsEquality * mjwf_mjs_asEquality(mjsElement * element) {
  return mjs_asEquality(element);
}

MJWF_API_IMPL mjsExclude * mjwf_mjs_asExclude(mjsElement * element) {
  return mjs_asExclude(element);
}

MJWF_API_IMPL mjsFlex * mjwf_mjs_asFlex(mjsElement * element) {
  return mjs_asFlex(element);
}

MJWF_API_IMPL mjsFrame * mjwf_mjs_asFrame(mjsElement * element) {
  return mjs_asFrame(element);
}

MJWF_API_IMPL mjsGeom * mjwf_mjs_asGeom(mjsElement * element) {
  return mjs_asGeom(element);
}

MJWF_API_IMPL mjsHField * mjwf_mjs_asHField(mjsElement * element) {
  return mjs_asHField(element);
}

MJWF_API_IMPL mjsJoint * mjwf_mjs_asJoint(mjsElement * element) {
  return mjs_asJoint(element);
}

MJWF_API_IMPL mjsKey * mjwf_mjs_asKey(mjsElement * element) {
  return mjs_asKey(element);
}

MJWF_API_IMPL mjsLight * mjwf_mjs_asLight(mjsElement * element) {
  return mjs_asLight(element);
}

MJWF_API_IMPL mjsMaterial * mjwf_mjs_asMaterial(mjsElement * element) {
  return mjs_asMaterial(element);
}

MJWF_API_IMPL mjsMesh * mjwf_mjs_asMesh(mjsElement * element) {
  return mjs_asMesh(element);
}

MJWF_API_IMPL mjsNumeric * mjwf_mjs_asNumeric(mjsElement * element) {
  return mjs_asNumeric(element);
}

MJWF_API_IMPL mjsPair * mjwf_mjs_asPair(mjsElement * element) {
  return mjs_asPair(element);
}

MJWF_API_IMPL mjsPlugin * mjwf_mjs_asPlugin(mjsElement * element) {
  return mjs_asPlugin(element);
}

MJWF_API_IMPL mjsSensor * mjwf_mjs_asSensor(mjsElement * element) {
  return mjs_asSensor(element);
}

MJWF_API_IMPL mjsSite * mjwf_mjs_asSite(mjsElement * element) {
  return mjs_asSite(element);
}

MJWF_API_IMPL mjsSkin * mjwf_mjs_asSkin(mjsElement * element) {
  return mjs_asSkin(element);
}

MJWF_API_IMPL mjsTendon * mjwf_mjs_asTendon(mjsElement * element) {
  return mjs_asTendon(element);
}

MJWF_API_IMPL mjsText * mjwf_mjs_asText(mjsElement * element) {
  return mjs_asText(element);
}

MJWF_API_IMPL mjsTexture * mjwf_mjs_asTexture(mjsElement * element) {
  return mjs_asTexture(element);
}

MJWF_API_IMPL mjsTuple * mjwf_mjs_asTuple(mjsElement * element) {
  return mjs_asTuple(element);
}

MJWF_API_IMPL mjsElement * mjwf_mjs_attach(mjsElement * parent, const mjsElement * child, const char * prefix, const char * suffix) {
  return mjs_attach(parent, child, prefix, suffix);
}

MJWF_API_IMPL mjsFrame * mjwf_mjs_bodyToFrame(mjsBody * * body) {
  return mjs_bodyToFrame(body);
}

MJWF_API_IMPL void mjwf_mjs_defaultActuator(mjsActuator * actuator) {
  mjs_defaultActuator(actuator);
}

MJWF_API_IMPL void mjwf_mjs_defaultBody(mjsBody * body) {
  mjs_defaultBody(body);
}

MJWF_API_IMPL void mjwf_mjs_defaultCamera(mjsCamera * camera) {
  mjs_defaultCamera(camera);
}

MJWF_API_IMPL void mjwf_mjs_defaultEquality(mjsEquality * equality) {
  mjs_defaultEquality(equality);
}

MJWF_API_IMPL void mjwf_mjs_defaultFlex(mjsFlex * flex) {
  mjs_defaultFlex(flex);
}

MJWF_API_IMPL void mjwf_mjs_defaultFrame(mjsFrame * frame) {
  mjs_defaultFrame(frame);
}

MJWF_API_IMPL void mjwf_mjs_defaultGeom(mjsGeom * geom) {
  mjs_defaultGeom(geom);
}

MJWF_API_IMPL void mjwf_mjs_defaultHField(mjsHField * hfield) {
  mjs_defaultHField(hfield);
}

MJWF_API_IMPL void mjwf_mjs_defaultJoint(mjsJoint * joint) {
  mjs_defaultJoint(joint);
}

MJWF_API_IMPL void mjwf_mjs_defaultKey(mjsKey * key) {
  mjs_defaultKey(key);
}

MJWF_API_IMPL void mjwf_mjs_defaultLight(mjsLight * light) {
  mjs_defaultLight(light);
}

MJWF_API_IMPL void mjwf_mjs_defaultMaterial(mjsMaterial * material) {
  mjs_defaultMaterial(material);
}

MJWF_API_IMPL void mjwf_mjs_defaultMesh(mjsMesh * mesh) {
  mjs_defaultMesh(mesh);
}

MJWF_API_IMPL void mjwf_mjs_defaultNumeric(mjsNumeric * numeric) {
  mjs_defaultNumeric(numeric);
}

MJWF_API_IMPL void mjwf_mjs_defaultOrientation(mjsOrientation * orient) {
  mjs_defaultOrientation(orient);
}

MJWF_API_IMPL void mjwf_mjs_defaultPair(mjsPair * pair) {
  mjs_defaultPair(pair);
}

MJWF_API_IMPL void mjwf_mjs_defaultPlugin(mjsPlugin * plugin) {
  mjs_defaultPlugin(plugin);
}

MJWF_API_IMPL void mjwf_mjs_defaultSensor(mjsSensor * sensor) {
  mjs_defaultSensor(sensor);
}

MJWF_API_IMPL void mjwf_mjs_defaultSite(mjsSite * site) {
  mjs_defaultSite(site);
}

MJWF_API_IMPL void mjwf_mjs_defaultSkin(mjsSkin * skin) {
  mjs_defaultSkin(skin);
}

MJWF_API_IMPL void mjwf_mjs_defaultSpec(mjSpec * spec) {
  mjs_defaultSpec(spec);
}

MJWF_API_IMPL void mjwf_mjs_defaultTendon(mjsTendon * tendon) {
  mjs_defaultTendon(tendon);
}

MJWF_API_IMPL void mjwf_mjs_defaultText(mjsText * text) {
  mjs_defaultText(text);
}

MJWF_API_IMPL void mjwf_mjs_defaultTexture(mjsTexture * texture) {
  mjs_defaultTexture(texture);
}

MJWF_API_IMPL void mjwf_mjs_defaultTuple(mjsTuple * tuple) {
  mjs_defaultTuple(tuple);
}

MJWF_API_IMPL int mjwf_mjs_delete(mjSpec * spec, mjsElement * element) {
  return mjs_delete(spec, element);
}

MJWF_API_IMPL void mjwf_mjs_deleteUserValue(mjsElement * element, const char * key) {
  mjs_deleteUserValue(element, key);
}

MJWF_API_IMPL mjsBody * mjwf_mjs_findBody(mjSpec * s, const char * name) {
  return mjs_findBody(s, name);
}

MJWF_API_IMPL mjsBody * mjwf_mjs_findChild(mjsBody * body, const char * name) {
  return mjs_findChild(body, name);
}

MJWF_API_IMPL mjsDefault * mjwf_mjs_findDefault(mjSpec * s, const char * classname) {
  return mjs_findDefault(s, classname);
}

MJWF_API_IMPL mjsElement * mjwf_mjs_findElement(mjSpec * s, mjtObj type, const char * name) {
  return mjs_findElement(s, type, name);
}

MJWF_API_IMPL mjsFrame * mjwf_mjs_findFrame(mjSpec * s, const char * name) {
  return mjs_findFrame(s, name);
}

MJWF_API_IMPL mjSpec * mjwf_mjs_findSpec(mjSpec * spec, const char * name) {
  return mjs_findSpec(spec, name);
}

MJWF_API_IMPL mjsElement * mjwf_mjs_firstChild(mjsBody * body, mjtObj type, int recurse) {
  return mjs_firstChild(body, type, recurse);
}

MJWF_API_IMPL mjsElement * mjwf_mjs_firstElement(mjSpec * s, mjtObj type) {
  return mjs_firstElement(s, type);
}

MJWF_API_IMPL mjsDefault * mjwf_mjs_getDefault(mjsElement * element) {
  return mjs_getDefault(element);
}

MJWF_API_IMPL const double * mjwf_mjs_getDouble(const mjDoubleVec * source, int * size) {
  return mjs_getDouble(source, size);
}

MJWF_API_IMPL const char * mjwf_mjs_getError(mjSpec * s) {
  return mjs_getError(s);
}

MJWF_API_IMPL mjsFrame * mjwf_mjs_getFrame(mjsElement * element) {
  return mjs_getFrame(element);
}

MJWF_API_IMPL int mjwf_mjs_getId(mjsElement * element) {
  return mjs_getId(element);
}

MJWF_API_IMPL mjString * mjwf_mjs_getName(mjsElement * element) {
  return mjs_getName(element);
}

MJWF_API_IMPL mjsBody * mjwf_mjs_getParent(mjsElement * element) {
  return mjs_getParent(element);
}

MJWF_API_IMPL const void * mjwf_mjs_getPluginAttributes(const mjsPlugin * plugin) {
  return mjs_getPluginAttributes(plugin);
}

MJWF_API_IMPL mjSpec * mjwf_mjs_getSpec(mjsElement * element) {
  return mjs_getSpec(element);
}

MJWF_API_IMPL mjsDefault * mjwf_mjs_getSpecDefault(mjSpec * s) {
  return mjs_getSpecDefault(s);
}

MJWF_API_IMPL const char * mjwf_mjs_getString(const mjString * source) {
  return mjs_getString(source);
}

MJWF_API_IMPL const void * mjwf_mjs_getUserValue(mjsElement * element, const char * key) {
  return mjs_getUserValue(element, key);
}

MJWF_API_IMPL int mjwf_mjs_isWarning(mjSpec * s) {
  return mjs_isWarning(s);
}

MJWF_API_IMPL int mjwf_mjs_makeMesh(mjsMesh * mesh, mjtMeshBuiltin builtin, double * params, int nparams) {
  return mjs_makeMesh(mesh, builtin, params, nparams);
}

MJWF_API_IMPL mjsElement * mjwf_mjs_nextChild(mjsBody * body, mjsElement * child, int recurse) {
  return mjs_nextChild(body, child, recurse);
}

MJWF_API_IMPL mjsElement * mjwf_mjs_nextElement(mjSpec * s, mjsElement * element) {
  return mjs_nextElement(s, element);
}

MJWF_API_IMPL const char * mjwf_mjs_resolveOrientation(double quat[4], mjtByte degree, const char * sequence, const mjsOrientation * orientation) {
  return mjs_resolveOrientation(quat, degree, sequence, orientation);
}

MJWF_API_IMPL int mjwf_mjs_sensorDim(const mjsSensor * sensor) {
  return mjs_sensorDim(sensor);
}

MJWF_API_IMPL void mjwf_mjs_setBuffer(mjByteVec * dest, const void * array, int size) {
  mjs_setBuffer(dest, array, size);
}

MJWF_API_IMPL int mjwf_mjs_setDeepCopy(mjSpec * s, int deepcopy) {
  return mjs_setDeepCopy(s, deepcopy);
}

MJWF_API_IMPL void mjwf_mjs_setDefault(mjsElement * element, const mjsDefault * def) {
  mjs_setDefault(element, def);
}

MJWF_API_IMPL void mjwf_mjs_setDouble(mjDoubleVec * dest, const double * array, int size) {
  mjs_setDouble(dest, array, size);
}

MJWF_API_IMPL void mjwf_mjs_setFloat(mjFloatVec * dest, const float * array, int size) {
  mjs_setFloat(dest, array, size);
}

MJWF_API_IMPL int mjwf_mjs_setFrame(mjsElement * dest, mjsFrame * frame) {
  return mjs_setFrame(dest, frame);
}

MJWF_API_IMPL mjtByte mjwf_mjs_setInStringVec(mjStringVec * dest, int i, const char * text) {
  return mjs_setInStringVec(dest, i, text);
}

MJWF_API_IMPL void mjwf_mjs_setInt(mjIntVec * dest, const int * array, int size) {
  mjs_setInt(dest, array, size);
}

MJWF_API_IMPL int mjwf_mjs_setName(mjsElement * element, const char * name) {
  return mjs_setName(element, name);
}

MJWF_API_IMPL void mjwf_mjs_setPluginAttributes(mjsPlugin * plugin, void * attributes) {
  mjs_setPluginAttributes(plugin, attributes);
}

MJWF_API_IMPL void mjwf_mjs_setString(mjString * dest, const char * text) {
  mjs_setString(dest, text);
}

MJWF_API_IMPL void mjwf_mjs_setStringVec(mjStringVec * dest, const char * text) {
  mjs_setStringVec(dest, text);
}

MJWF_API_IMPL const char * mjwf_mjs_setToAdhesion(mjsActuator * actuator, double gain) {
  return mjs_setToAdhesion(actuator, gain);
}

MJWF_API_IMPL const char * mjwf_mjs_setToCylinder(mjsActuator * actuator, double timeconst, double bias, double area, double diameter) {
  return mjs_setToCylinder(actuator, timeconst, bias, area, diameter);
}

MJWF_API_IMPL const char * mjwf_mjs_setToDamper(mjsActuator * actuator, double kv) {
  return mjs_setToDamper(actuator, kv);
}

MJWF_API_IMPL const char * mjwf_mjs_setToIntVelocity(mjsActuator * actuator, double kp, double kv[1], double dampratio[1], double timeconst[1], double inheritrange) {
  return mjs_setToIntVelocity(actuator, kp, kv, dampratio, timeconst, inheritrange);
}

MJWF_API_IMPL const char * mjwf_mjs_setToMotor(mjsActuator * actuator) {
  return mjs_setToMotor(actuator);
}

MJWF_API_IMPL const char * mjwf_mjs_setToMuscle(mjsActuator * actuator, double timeconst[2], double tausmooth, double range[2], double force, double scale, double lmin, double lmax, double vmax, double fpmax, double fvmax) {
  return mjs_setToMuscle(actuator, timeconst, tausmooth, range, force, scale, lmin, lmax, vmax, fpmax, fvmax);
}

MJWF_API_IMPL const char * mjwf_mjs_setToPosition(mjsActuator * actuator, double kp, double kv[1], double dampratio[1], double timeconst[1], double inheritrange) {
  return mjs_setToPosition(actuator, kp, kv, dampratio, timeconst, inheritrange);
}

MJWF_API_IMPL const char * mjwf_mjs_setToVelocity(mjsActuator * actuator, double kv) {
  return mjs_setToVelocity(actuator, kv);
}

MJWF_API_IMPL void mjwf_mjs_setUserValue(mjsElement * element, const char * key, const void * data) {
  mjs_setUserValue(element, key, data);
}

MJWF_API_IMPL mjsWrap * mjwf_mjs_wrapGeom(mjsTendon * tendon, const char * name, const char * sidesite) {
  return mjs_wrapGeom(tendon, name, sidesite);
}

MJWF_API_IMPL mjsWrap * mjwf_mjs_wrapJoint(mjsTendon * tendon, const char * name, double coef) {
  return mjs_wrapJoint(tendon, name, coef);
}

MJWF_API_IMPL mjsWrap * mjwf_mjs_wrapPulley(mjsTendon * tendon, double divisor) {
  return mjs_wrapPulley(tendon, divisor);
}

MJWF_API_IMPL mjsWrap * mjwf_mjs_wrapSite(mjsTendon * tendon, const char * name) {
  return mjs_wrapSite(tendon, name);
}

MJWF_API_IMPL mjtNum mjwf_mju_Halton(int index, int base) {
  return mju_Halton(index, base);
}

MJWF_API_IMPL mjtNum mjwf_mju_L1(const mjtNum * vec, int n) {
  return mju_L1(vec, n);
}

MJWF_API_IMPL void mjwf_mju_add(mjtNum * res, const mjtNum * vec1, const mjtNum * vec2, int n) {
  mju_add(res, vec1, vec2, n);
}

MJWF_API_IMPL void mjwf_mju_add3(mjtNum res[3], const mjtNum vec1[3], const mjtNum vec2[3]) {
  mju_add3(res, vec1, vec2);
}

MJWF_API_IMPL void mjwf_mju_addScl(mjtNum * res, const mjtNum * vec1, const mjtNum * vec2, mjtNum scl, int n) {
  mju_addScl(res, vec1, vec2, scl, n);
}

MJWF_API_IMPL void mjwf_mju_addScl3(mjtNum res[3], const mjtNum vec1[3], const mjtNum vec2[3], mjtNum scl) {
  mju_addScl3(res, vec1, vec2, scl);
}

MJWF_API_IMPL void mjwf_mju_addTo(mjtNum * res, const mjtNum * vec, int n) {
  mju_addTo(res, vec, n);
}

MJWF_API_IMPL void mjwf_mju_addTo3(mjtNum res[3], const mjtNum vec[3]) {
  mju_addTo3(res, vec);
}

MJWF_API_IMPL void mjwf_mju_addToScl(mjtNum * res, const mjtNum * vec, mjtNum scl, int n) {
  mju_addToScl(res, vec, scl, n);
}

MJWF_API_IMPL void mjwf_mju_addToScl3(mjtNum res[3], const mjtNum vec[3], mjtNum scl) {
  mju_addToScl3(res, vec, scl);
}

MJWF_API_IMPL void mjwf_mju_axisAngle2Quat(mjtNum res[4], const mjtNum axis[3], mjtNum angle) {
  mju_axisAngle2Quat(res, axis, angle);
}

MJWF_API_IMPL void mjwf_mju_band2Dense(mjtNum * res, const mjtNum * mat, int ntotal, int nband, int ndense, mjtByte flg_sym) {
  mju_band2Dense(res, mat, ntotal, nband, ndense, flg_sym);
}

MJWF_API_IMPL int mjwf_mju_bandDiag(int i, int ntotal, int nband, int ndense) {
  return mju_bandDiag(i, ntotal, nband, ndense);
}

MJWF_API_IMPL void mjwf_mju_bandMulMatVec(mjtNum * res, const mjtNum * mat, const mjtNum * vec, int ntotal, int nband, int ndense, int nvec, mjtByte flg_sym) {
  mju_bandMulMatVec(res, mat, vec, ntotal, nband, ndense, nvec, flg_sym);
}

MJWF_API_IMPL void mjwf_mju_bindThreadPool(mjData * d, void * thread_pool) {
  mju_bindThreadPool(d, thread_pool);
}

MJWF_API_IMPL int mjwf_mju_boxQP(mjtNum * res, mjtNum * R, int * index, const mjtNum * H, const mjtNum * g, int n, const mjtNum * lower, const mjtNum * upper) {
  return mju_boxQP(res, R, index, H, g, n, lower, upper);
}

MJWF_API_IMPL void mjwf_mju_boxQPmalloc(mjtNum * * res, mjtNum * * R, int * * index, mjtNum * * H, mjtNum * * g, int n, mjtNum * * lower, mjtNum * * upper) {
  mju_boxQPmalloc(res, R, index, H, g, n, lower, upper);
}

MJWF_API_IMPL int mjwf_mju_cholFactor(mjtNum * mat, int n, mjtNum mindiag) {
  return mju_cholFactor(mat, n, mindiag);
}

MJWF_API_IMPL mjtNum mjwf_mju_cholFactorBand(mjtNum * mat, int ntotal, int nband, int ndense, mjtNum diagadd, mjtNum diagmul) {
  return mju_cholFactorBand(mat, ntotal, nband, ndense, diagadd, diagmul);
}

MJWF_API_IMPL void mjwf_mju_cholSolve(mjtNum * res, const mjtNum * mat, const mjtNum * vec, int n) {
  mju_cholSolve(res, mat, vec, n);
}

MJWF_API_IMPL void mjwf_mju_cholSolveBand(mjtNum * res, const mjtNum * mat, const mjtNum * vec, int ntotal, int nband, int ndense) {
  mju_cholSolveBand(res, mat, vec, ntotal, nband, ndense);
}

MJWF_API_IMPL int mjwf_mju_cholUpdate(mjtNum * mat, mjtNum * x, int n, int flg_plus) {
  return mju_cholUpdate(mat, x, n, flg_plus);
}

MJWF_API_IMPL void mjwf_mju_clearHandlers(void) {
  mju_clearHandlers();
}

MJWF_API_IMPL mjtNum mjwf_mju_clip(mjtNum x, mjtNum min, mjtNum max) {
  return mju_clip(x, min, max);
}

MJWF_API_IMPL void mjwf_mju_copy(mjtNum * res, const mjtNum * vec, int n) {
  mju_copy(res, vec, n);
}

MJWF_API_IMPL void mjwf_mju_copy3(mjtNum res[3], const mjtNum data[3]) {
  mju_copy3(res, data);
}

MJWF_API_IMPL void mjwf_mju_copy4(mjtNum res[4], const mjtNum data[4]) {
  mju_copy4(res, data);
}

MJWF_API_IMPL void mjwf_mju_cross(mjtNum res[3], const mjtNum a[3], const mjtNum b[3]) {
  mju_cross(res, a, b);
}

MJWF_API_IMPL void mjwf_mju_d2n(mjtNum * res, const double * vec, int n) {
  mju_d2n(res, vec, n);
}

MJWF_API_IMPL void mjwf_mju_decodePyramid(mjtNum * force, const mjtNum * pyramid, const mjtNum * mu, int dim) {
  mju_decodePyramid(force, pyramid, mu, dim);
}

MJWF_API_IMPL void mjwf_mju_defaultTask(mjTask * task) {
  mju_defaultTask(task);
}

MJWF_API_IMPL void mjwf_mju_dense2Band(mjtNum * res, const mjtNum * mat, int ntotal, int nband, int ndense) {
  mju_dense2Band(res, mat, ntotal, nband, ndense);
}

MJWF_API_IMPL int mjwf_mju_dense2sparse(mjtNum * res, const mjtNum * mat, int nr, int nc, int * rownnz, int * rowadr, int * colind, int nnz) {
  return mju_dense2sparse(res, mat, nr, nc, rownnz, rowadr, colind, nnz);
}

MJWF_API_IMPL void mjwf_mju_derivQuat(mjtNum res[4], const mjtNum quat[4], const mjtNum vel[3]) {
  mju_derivQuat(res, quat, vel);
}

MJWF_API_IMPL mjtNum mjwf_mju_dist3(const mjtNum pos1[3], const mjtNum pos2[3]) {
  return mju_dist3(pos1, pos2);
}

MJWF_API_IMPL mjtNum mjwf_mju_dot(const mjtNum * vec1, const mjtNum * vec2, int n) {
  return mju_dot(vec1, vec2, n);
}

MJWF_API_IMPL mjtNum mjwf_mju_dot3(const mjtNum vec1[3], const mjtNum vec2[3]) {
  return mju_dot3(vec1, vec2);
}

MJWF_API_IMPL int mjwf_mju_eig3(mjtNum eigval[3], mjtNum eigvec[9], mjtNum quat[4], const mjtNum mat[9]) {
  return mju_eig3(eigval, eigvec, quat, mat);
}

MJWF_API_IMPL void mjwf_mju_encodePyramid(mjtNum * pyramid, const mjtNum * force, const mjtNum * mu, int dim) {
  mju_encodePyramid(pyramid, force, mu, dim);
}

MJWF_API_IMPL void mjwf_mju_error(const char * msg) {
  mju_error(msg);
}

MJWF_API_IMPL void mjwf_mju_error_i(const char * msg, int i) {
  mju_error_i(msg, i);
}

MJWF_API_IMPL void mjwf_mju_error_s(const char * msg, const char * text) {
  mju_error_s(msg, text);
}

MJWF_API_IMPL void mjwf_mju_euler2Quat(mjtNum quat[4], const mjtNum euler[3], const char * seq) {
  mju_euler2Quat(quat, euler, seq);
}

MJWF_API_IMPL void mjwf_mju_eye(mjtNum * mat, int n) {
  mju_eye(mat, n);
}

MJWF_API_IMPL void mjwf_mju_f2n(mjtNum * res, const float * vec, int n) {
  mju_f2n(res, vec, n);
}

MJWF_API_IMPL void mjwf_mju_fill(mjtNum * res, mjtNum val, int n) {
  mju_fill(res, val, n);
}

MJWF_API_IMPL void mjwf_mju_free(void * ptr) {
  mju_free(ptr);
}

MJWF_API_IMPL void mjwf_mju_getXMLDependencies(const char * filename, mjStringVec * dependencies) {
  mju_getXMLDependencies(filename, dependencies);
}

MJWF_API_IMPL void mjwf_mju_insertionSort(mjtNum * list, int n) {
  mju_insertionSort(list, n);
}

MJWF_API_IMPL void mjwf_mju_insertionSortInt(int * list, int n) {
  mju_insertionSortInt(list, n);
}

MJWF_API_IMPL int mjwf_mju_isBad(mjtNum x) {
  return mju_isBad(x);
}

MJWF_API_IMPL int mjwf_mju_isZero(const mjtNum * vec, int n) {
  return mju_isZero(vec, n);
}

MJWF_API_IMPL void * mjwf_mju_malloc(size_t size) {
  return mju_malloc(size);
}

MJWF_API_IMPL void mjwf_mju_mat2Quat(mjtNum quat[4], const mjtNum mat[9]) {
  mju_mat2Quat(quat, mat);
}

MJWF_API_IMPL int mjwf_mju_mat2Rot(mjtNum quat[4], const mjtNum mat[9]) {
  return mju_mat2Rot(quat, mat);
}

MJWF_API_IMPL mjtNum mjwf_mju_max(mjtNum a, mjtNum b) {
  return mju_max(a, b);
}

MJWF_API_IMPL mjtNum mjwf_mju_min(mjtNum a, mjtNum b) {
  return mju_min(a, b);
}

MJWF_API_IMPL void mjwf_mju_mulMatMat(mjtNum * res, const mjtNum * mat1, const mjtNum * mat2, int r1, int c1, int c2) {
  mju_mulMatMat(res, mat1, mat2, r1, c1, c2);
}

MJWF_API_IMPL void mjwf_mju_mulMatMatT(mjtNum * res, const mjtNum * mat1, const mjtNum * mat2, int r1, int c1, int r2) {
  mju_mulMatMatT(res, mat1, mat2, r1, c1, r2);
}

MJWF_API_IMPL void mjwf_mju_mulMatTMat(mjtNum * res, const mjtNum * mat1, const mjtNum * mat2, int r1, int c1, int c2) {
  mju_mulMatTMat(res, mat1, mat2, r1, c1, c2);
}

MJWF_API_IMPL void mjwf_mju_mulMatTVec(mjtNum * res, const mjtNum * mat, const mjtNum * vec, int nr, int nc) {
  mju_mulMatTVec(res, mat, vec, nr, nc);
}

MJWF_API_IMPL void mjwf_mju_mulMatTVec3(mjtNum res[3], const mjtNum mat[9], const mjtNum vec[3]) {
  mju_mulMatTVec3(res, mat, vec);
}

MJWF_API_IMPL void mjwf_mju_mulMatVec(mjtNum * res, const mjtNum * mat, const mjtNum * vec, int nr, int nc) {
  mju_mulMatVec(res, mat, vec, nr, nc);
}

MJWF_API_IMPL void mjwf_mju_mulMatVec3(mjtNum res[3], const mjtNum mat[9], const mjtNum vec[3]) {
  mju_mulMatVec3(res, mat, vec);
}

MJWF_API_IMPL void mjwf_mju_mulPose(mjtNum posres[3], mjtNum quatres[4], const mjtNum pos1[3], const mjtNum quat1[4], const mjtNum pos2[3], const mjtNum quat2[4]) {
  mju_mulPose(posres, quatres, pos1, quat1, pos2, quat2);
}

MJWF_API_IMPL void mjwf_mju_mulQuat(mjtNum res[4], const mjtNum quat1[4], const mjtNum quat2[4]) {
  mju_mulQuat(res, quat1, quat2);
}

MJWF_API_IMPL void mjwf_mju_mulQuatAxis(mjtNum res[4], const mjtNum quat[4], const mjtNum axis[3]) {
  mju_mulQuatAxis(res, quat, axis);
}

MJWF_API_IMPL mjtNum mjwf_mju_mulVecMatVec(const mjtNum * vec1, const mjtNum * mat, const mjtNum * vec2, int n) {
  return mju_mulVecMatVec(vec1, mat, vec2, n);
}

MJWF_API_IMPL mjtNum mjwf_mju_muscleBias(mjtNum len, const mjtNum lengthrange[2], mjtNum acc0, const mjtNum prm[9]) {
  return mju_muscleBias(len, lengthrange, acc0, prm);
}

MJWF_API_IMPL mjtNum mjwf_mju_muscleDynamics(mjtNum ctrl, mjtNum act, const mjtNum prm[3]) {
  return mju_muscleDynamics(ctrl, act, prm);
}

MJWF_API_IMPL mjtNum mjwf_mju_muscleGain(mjtNum len, mjtNum vel, const mjtNum lengthrange[2], mjtNum acc0, const mjtNum prm[9]) {
  return mju_muscleGain(len, vel, lengthrange, acc0, prm);
}

MJWF_API_IMPL void mjwf_mju_n2d(double * res, const mjtNum * vec, int n) {
  mju_n2d(res, vec, n);
}

MJWF_API_IMPL void mjwf_mju_n2f(float * res, const mjtNum * vec, int n) {
  mju_n2f(res, vec, n);
}

MJWF_API_IMPL void mjwf_mju_negPose(mjtNum posres[3], mjtNum quatres[4], const mjtNum pos[3], const mjtNum quat[4]) {
  mju_negPose(posres, quatres, pos, quat);
}

MJWF_API_IMPL void mjwf_mju_negQuat(mjtNum res[4], const mjtNum quat[4]) {
  mju_negQuat(res, quat);
}

MJWF_API_IMPL mjtNum mjwf_mju_norm(const mjtNum * res, int n) {
  return mju_norm(res, n);
}

MJWF_API_IMPL mjtNum mjwf_mju_norm3(const mjtNum vec[3]) {
  return mju_norm3(vec);
}

MJWF_API_IMPL mjtNum mjwf_mju_normalize(mjtNum * res, int n) {
  return mju_normalize(res, n);
}

MJWF_API_IMPL mjtNum mjwf_mju_normalize3(mjtNum vec[3]) {
  return mju_normalize3(vec);
}

MJWF_API_IMPL mjtNum mjwf_mju_normalize4(mjtNum vec[4]) {
  return mju_normalize4(vec);
}

MJWF_API_IMPL void mjwf_mju_printMat(const mjtNum * mat, int nr, int nc) {
  mju_printMat(mat, nr, nc);
}

MJWF_API_IMPL void mjwf_mju_printMatSparse(const mjtNum * mat, int nr, const int * rownnz, const int * rowadr, const int * colind) {
  mju_printMatSparse(mat, nr, rownnz, rowadr, colind);
}

MJWF_API_IMPL void mjwf_mju_quat2Mat(mjtNum res[9], const mjtNum quat[4]) {
  mju_quat2Mat(res, quat);
}

MJWF_API_IMPL void mjwf_mju_quat2Vel(mjtNum res[3], const mjtNum quat[4], mjtNum dt) {
  mju_quat2Vel(res, quat, dt);
}

MJWF_API_IMPL void mjwf_mju_quatIntegrate(mjtNum quat[4], const mjtNum vel[3], mjtNum scale) {
  mju_quatIntegrate(quat, vel, scale);
}

MJWF_API_IMPL void mjwf_mju_quatZ2Vec(mjtNum quat[4], const mjtNum vec[3]) {
  mju_quatZ2Vec(quat, vec);
}

MJWF_API_IMPL mjtNum mjwf_mju_rayFlex(const mjModel * m, const mjData * d, int flex_layer, mjtByte flg_vert, mjtByte flg_edge, mjtByte flg_face, mjtByte flg_skin, int flexid, const mjtNum * pnt, const mjtNum * vec, int vertid[1]) {
  return mju_rayFlex(m, d, flex_layer, flg_vert, flg_edge, flg_face, flg_skin, flexid, pnt, vec, vertid);
}

MJWF_API_IMPL mjtNum mjwf_mju_rayGeom(const mjtNum pos[3], const mjtNum mat[9], const mjtNum size[3], const mjtNum pnt[3], const mjtNum vec[3], int geomtype) {
  return mju_rayGeom(pos, mat, size, pnt, vec, geomtype);
}

MJWF_API_IMPL mjtNum mjwf_mju_raySkin(int nface, int nvert, const int * face, const float * vert, const mjtNum pnt[3], const mjtNum vec[3], int vertid[1]) {
  return mju_raySkin(nface, nvert, face, vert, pnt, vec, vertid);
}

MJWF_API_IMPL void mjwf_mju_rotVecQuat(mjtNum res[3], const mjtNum vec[3], const mjtNum quat[4]) {
  mju_rotVecQuat(res, vec, quat);
}

MJWF_API_IMPL int mjwf_mju_round(mjtNum x) {
  return mju_round(x);
}

MJWF_API_IMPL void mjwf_mju_scl(mjtNum * res, const mjtNum * vec, mjtNum scl, int n) {
  mju_scl(res, vec, scl, n);
}

MJWF_API_IMPL void mjwf_mju_scl3(mjtNum res[3], const mjtNum vec[3], mjtNum scl) {
  mju_scl3(res, vec, scl);
}

MJWF_API_IMPL mjtNum mjwf_mju_sigmoid(mjtNum x) {
  return mju_sigmoid(x);
}

MJWF_API_IMPL mjtNum mjwf_mju_sign(mjtNum x) {
  return mju_sign(x);
}

MJWF_API_IMPL void mjwf_mju_sparse2dense(mjtNum * res, const mjtNum * mat, int nr, int nc, const int * rownnz, const int * rowadr, const int * colind) {
  mju_sparse2dense(res, mat, nr, nc, rownnz, rowadr, colind);
}

MJWF_API_IMPL mjtNum mjwf_mju_springDamper(mjtNum pos0, mjtNum vel0, mjtNum Kp, mjtNum Kv, mjtNum dt) {
  return mju_springDamper(pos0, vel0, Kp, Kv, dt);
}

MJWF_API_IMPL void mjwf_mju_sqrMatTD(mjtNum * res, const mjtNum * mat, const mjtNum * diag, int nr, int nc) {
  mju_sqrMatTD(res, mat, diag, nr, nc);
}

MJWF_API_IMPL mjtNum mjwf_mju_standardNormal(mjtNum * num2) {
  return mju_standardNormal(num2);
}

MJWF_API_IMPL int mjwf_mju_str2Type(const char * str) {
  return mju_str2Type(str);
}

MJWF_API_IMPL char * mjwf_mju_strncpy(char * dst, const char * src, int n) {
  return mju_strncpy(dst, src, n);
}

MJWF_API_IMPL void mjwf_mju_sub(mjtNum * res, const mjtNum * vec1, const mjtNum * vec2, int n) {
  mju_sub(res, vec1, vec2, n);
}

MJWF_API_IMPL void mjwf_mju_sub3(mjtNum res[3], const mjtNum vec1[3], const mjtNum vec2[3]) {
  mju_sub3(res, vec1, vec2);
}

MJWF_API_IMPL void mjwf_mju_subFrom(mjtNum * res, const mjtNum * vec, int n) {
  mju_subFrom(res, vec, n);
}

MJWF_API_IMPL void mjwf_mju_subFrom3(mjtNum res[3], const mjtNum vec[3]) {
  mju_subFrom3(res, vec);
}

MJWF_API_IMPL void mjwf_mju_subQuat(mjtNum res[3], const mjtNum qa[4], const mjtNum qb[4]) {
  mju_subQuat(res, qa, qb);
}

MJWF_API_IMPL mjtNum mjwf_mju_sum(const mjtNum * vec, int n) {
  return mju_sum(vec, n);
}

MJWF_API_IMPL void mjwf_mju_symmetrize(mjtNum * res, const mjtNum * mat, int n) {
  mju_symmetrize(res, mat, n);
}

MJWF_API_IMPL void mjwf_mju_taskJoin(mjTask * task) {
  mju_taskJoin(task);
}

MJWF_API_IMPL mjThreadPool * mjwf_mju_threadPoolCreate(size_t number_of_threads) {
  return mju_threadPoolCreate(number_of_threads);
}

MJWF_API_IMPL void mjwf_mju_threadPoolDestroy(mjThreadPool * thread_pool) {
  mju_threadPoolDestroy(thread_pool);
}

MJWF_API_IMPL void mjwf_mju_threadPoolEnqueue(mjThreadPool * thread_pool, mjTask * task) {
  mju_threadPoolEnqueue(thread_pool, task);
}

MJWF_API_IMPL void mjwf_mju_transformSpatial(mjtNum res[6], const mjtNum vec[6], int flg_force, const mjtNum newpos[3], const mjtNum oldpos[3], const mjtNum rotnew2old[9]) {
  mju_transformSpatial(res, vec, flg_force, newpos, oldpos, rotnew2old);
}

MJWF_API_IMPL void mjwf_mju_transpose(mjtNum * res, const mjtNum * mat, int nr, int nc) {
  mju_transpose(res, mat, nr, nc);
}

MJWF_API_IMPL void mjwf_mju_trnVecPose(mjtNum res[3], const mjtNum pos[3], const mjtNum quat[4], const mjtNum vec[3]) {
  mju_trnVecPose(res, pos, quat, vec);
}

MJWF_API_IMPL const char * mjwf_mju_type2Str(int type) {
  return mju_type2Str(type);
}

MJWF_API_IMPL void mjwf_mju_unit4(mjtNum res[4]) {
  mju_unit4(res);
}

MJWF_API_IMPL void mjwf_mju_warning(const char * msg) {
  mju_warning(msg);
}

MJWF_API_IMPL const char * mjwf_mju_warningText(int warning, size_t info) {
  return mju_warningText(warning, info);
}

MJWF_API_IMPL void mjwf_mju_warning_i(const char * msg, int i) {
  mju_warning_i(msg, i);
}

MJWF_API_IMPL void mjwf_mju_warning_s(const char * msg, const char * text) {
  mju_warning_s(msg, text);
}

MJWF_API_IMPL void mjwf_mju_writeLog(const char * type, const char * msg) {
  mju_writeLog(type, msg);
}

MJWF_API_IMPL const char * mjwf_mju_writeNumBytes(size_t nbytes) {
  return mju_writeNumBytes(nbytes);
}

MJWF_API_IMPL void mjwf_mju_zero(mjtNum * res, int n) {
  mju_zero(res, n);
}

MJWF_API_IMPL void mjwf_mju_zero3(mjtNum res[3]) {
  mju_zero3(res);
}

MJWF_API_IMPL void mjwf_mju_zero4(mjtNum res[4]) {
  mju_zero4(res);
}

#undef MJWF_API_IMPL
