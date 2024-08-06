#include "../headers/kinetics.h"

/* Rotates object according to World X axis. */
const void rotate_x(Mesh *c, const float angle) {
    Mat4x4 m = rotateXMatrix(radians(angle));
    // c->v = setvecsarrayxm(c->v, c->v_indexes, m);
    // c->n = setvecsarrayxm(c->n, c->n_indexes, m);

    setvecsarrayxm(c->bbox.v, c->bbox.v_indexes, m);
    setvecsarrayxm(c->cd.v, 4, m);
}
/* Rotates object according to World Y axis. */
const void rotate_y(Mesh *c, const float angle) {
    vec4f axis = { 0.f, 1.f, 0.f };

    Quat yrot = rotationQuat(angle, axis);
    Mat4x4 m = MatfromQuat(yrot, (vec4f){ 0.f, 0.f, 0.f, 0.f });

    c->cd.v[P] = vecxm(c->cd.v[P], m);
    c->Q = multiplyQuats(c->Q, yrot);

    c->r = yrot;

    setvecsarrayxm(c->bbox.v, c->bbox.v_indexes, m);
}
/* Rotates object according to World Y axis. */
// const void rotate_y(Mesh *c, const float angle) {
//     Mat4x4 m = rotateYMatrix(radians(angle));
//     c->v = setvecsarrayxm(c->v, c->v_indexes, m);
//     c->n = setvecsarrayxm(c->n, c->n_indexes, m);

//     c->bbox.v = setvecsarrayxm(c->bbox.v, c->bbox.v_indexes, m);
// }
/* Rotates object according to World Z axis. */
const void rotate_z(Mesh *c, const float angle) {
    Mat4x4 m = rotateZMatrix(radians(angle));
    // c->v = setvecsarrayxm(c->v, c->v_indexes, m);
    // c->n = setvecsarrayxm(c->n, c->n_indexes, m);

    setvecsarrayxm(c->bbox.v, c->bbox.v_indexes, m);
}
/* Rotates Mesh according to own axis in relation with its cd.v[P] point. */
const void rotate_origin(Mesh *m, const float angle, float x, float y, float z) {
    vec4f axis = { x, y, z };

    m->r = rotationQuat(angle, axis);
    m->rot_angle = angle;
}


