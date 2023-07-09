#include "headers/kinetics.h"

enum { Pos, U, V, N, C};
extern vec4f light[];
extern const void initMesh(Mesh *a, const Mesh b);
extern const void releaseMesh(Mesh *c);
extern const float radians(const float value);

// /* Rotates object according to World X axis. */
// const void rotate_x(Mesh *c, const float angle) {
//     Mat4x4 m = rotate_xmat(radians(angle));
//     Mesh cache = *c;
//     *c = meshxm(cache, m);
//     free(cache.t);
//     free(cache.v);
// }
/* Rotates object according to World Y axis. */
const void rotate_y(Mesh *c, const float angle) {
    Mesh cache;
    initMesh(&cache, *c);
    Mat4x4 m = rotateYMatrix(radians(angle));
    c->v = meshxm(cache.v, cache.v_indexes, m);
    releaseMesh(&cache);
}
// /* Rotates object according to World Z axis. */
// const void rotate_z(Mesh *c, const float angle) {
//     Mat4x4 m = rotate_zmat(radians(angle));
//     Mesh cache = *c;
//     *c = meshxm(cache, m);
//     free(cache.t);
//     free(cache.v);
// }
// /* Rotates object according to own axis. */
// const void rotate_origin(Mesh *c, const float angle, float x, float y, float z) {
//     vec4f pos = { 1.0, -1.0, 510.0 };
//     vec4f axis = { x, y, z };
//     Quat n = setQuat(0, pos);

//     Quat xrot = rotationQuat(angle, axis);
//     Mat4x4 rm = MatfromQuat(xrot, n.v);

//     Mesh cache = *c;
//     normalsxm(&cache, rm);
//     *c = meshxm(cache, rm);
    
//     free(cache.t);
//     free(cache.v);
// }
// /* Rotates light arround scene center. */
// const void rotate_light(vec4f g[], const float angle, float x, float y, float z) {
//     vec4f pos = { 0.0, 0.0, 498.0 };
//     vec4f axis = { x, y, z };
//     Quat n = setQuat(0, pos);

//     Quat xrot = rotationQuat(1, axis);
//     Mat4x4 rm = MatfromQuat(xrot, n.v);

//     light[Pos] = vecxm(g[Pos], rm);
//     light[U] = vecxm(g[U], rm);
//     light[V] = vecxm(g[V], rm);
//     light[N] = vecxm(g[N], rm);
// }


