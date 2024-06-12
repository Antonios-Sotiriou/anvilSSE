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

    c->bbox.v = setvecsarrayxm(c->bbox.v, c->bbox.v_indexes, m);
}
/* Rotates object according to World Z axis. */
const void rotate_z(Mesh *c, const float angle) {
    Mat4x4 m = rotateZMatrix(radians(angle));
    c->v = setvecsarrayxm(c->v, c->v_indexes, m);
    c->n = setvecsarrayxm(c->n, c->n_indexes, m);
}
/* Rotates Mesh according to own axis in relation with its pivot point. */
const void rotate_origin(Mesh *c, const float angle, float x, float y, float z) {
    vec4f axis = { x, y, z };
    Quat n = setQuat(0, c->pivot);
    // #include "headers/logging.h"
    // logQuat(n);
    Quat xrot = rotationQuat(angle, axis);
    Mat4x4 m = MatfromQuat(xrot, n.v);
    // logQuat(xrot);
    c->Q = multiplyQuats(c->Q, xrot);
    // logQuat(c->Q);
    c->v = setvecsarrayxm(c->v, c->v_indexes, m);
    c->n = setvecsarrayxm(c->n, c->n_indexes, m);

    c->bbox.v = setvecsarrayxm(c->bbox.v, c->bbox.v_indexes, m);
}
/* Rotates light arround a given pivot point. */
const void rotate_light(Light *l, const vec4f pivot, const float angle, float x, float y, float z) {
    vec4f axis = { x, y, z };
    Quat n = setQuat(0, pivot);

    Quat xrot = rotationQuat(angle, axis);
    Mat4x4 m = MatfromQuat(xrot, n.v);

    sunlight.pos = vecxm(l->pos, m);
    sunlight.u = vecxm(l->u, m);
    sunlight.v = vecxm(l->v, m);
    sunlight.n = vecxm(l->n, m);
}
/* Rotates light according to camera position. */
const void rotate_light_cam(Mesh *l, const vec4f pivot, const float angle, float x, float y, float z) {
    vec4f axis = { x, y, z };
    Quat n = setQuat(0, pivot);

    Quat xrot = rotationQuat(angle, axis);
    Mat4x4 m = MatfromQuat(xrot, n.v);

    l->v = setvecsarrayxm(l->v, l->v_indexes, m);
    l->n = setvecsarrayxm(l->n, l->n_indexes, m);
    // sunlight.pos = vecxm(l->pos, m);
    // sunlight.u = vecxm(l->u, m);
    // sunlight.v = vecxm(l->v, m);
    // sunlight.n = vecxm(l->n, m);
}


