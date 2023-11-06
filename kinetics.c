#include "headers/kinetics.h"

/* Rotates object according to World X axis. */
const void rotate_x(Mesh *c, const float angle) {
    Mat4x4 m = rotateXMatrix(radians(angle));
    c->v = setvecsarrayxm(c->v, c->v_indexes, m);
    c->n = setvecsarrayxm(c->n, c->n_indexes, m);
}
/* Rotates object according to World Y axis. */
const void rotate_y(Mesh *c, const float angle) {
    Mat4x4 m = rotateYMatrix(radians(angle));
    c->v = setvecsarrayxm(c->v, c->v_indexes, m);
    c->n = setvecsarrayxm(c->n, c->n_indexes, m);
}
/* Rotates object according to World Z axis. */
const void rotate_z(Mesh *c, const float angle) {
    Mat4x4 m = rotateZMatrix(radians(angle));
    c->v = setvecsarrayxm(c->v, c->v_indexes, m);
    c->n = setvecsarrayxm(c->n, c->n_indexes, m);
}
/* Rotates object according to own axis. */
const void rotate_origin(Mesh *c, const float angle, float x, float y, float z) {
    vec4f axis = { x, y, z };
    Quat n = setQuat(0, c->pivot);

    Quat xrot = rotationQuat(angle, axis);
    Mat4x4 m = MatfromQuat(xrot, n.v);

    c->v = setvecsarrayxm(c->v, c->v_indexes, m);
    c->n = setvecsarrayxm(c->n, c->n_indexes, m);
}
/* Rotates light arround scene center. */
const void rotate_light(Light *l, const float angle, float x, float y, float z) {
    vec4f pos = { 0.0, 0.0, 498.0 };
    vec4f axis = { x, y, z };
    Quat n = setQuat(0, pos);

    Quat xrot = rotationQuat(1, axis);
    Mat4x4 m = MatfromQuat(xrot, n.v);

    sunlight.newPos = vecxm(l->pos, m);
    sunlight.u = vecxm(l->u, m);
    sunlight.v = vecxm(l->v, m);
    sunlight.n = vecxm(l->n, m);
}


