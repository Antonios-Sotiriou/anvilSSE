#include "../../headers/components/kinetics.h"

/* Rotates object according to World X axis. */
const void rotateGlobalX(Mesh *m, const float angle) {
    vec4f axis = { 1.f, 0.f, 0.f };

    Quat yrot = rotationQuat(angle, axis);
    Mat4x4 mt = MatfromQuat(yrot, (vec4f){ 0.f, 0.f, 0.f, 0.f });

    m->cd.v[P] = vecxm(m->cd.v[P], mt);
    m->Q = multiplyQuats(m->Q, yrot);

    m->r = yrot;

    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, mt);
}
/* Rotates object according to World Y axis. */
const void rotateGlobalY(Mesh *m, const float angle) {
    vec4f axis = { 0.f, 1.f, 0.f };

    Quat yrot = rotationQuat(angle, axis);
    Mat4x4 mt = MatfromQuat(yrot, (vec4f){ 0.f, 0.f, 0.f, 0.f });

    m->cd.v[P] = vecxm(m->cd.v[P], mt);
    m->Q = multiplyQuats(m->Q, yrot);

    m->r = yrot;

    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, mt);
}
/* Rotates object according to World Z axis. */
const void rotateGlobalZ(Mesh *m, const float angle) {
    vec4f axis = { 0.f, 0.f, 1.f };

    Quat yrot = rotationQuat(angle, axis);
    Mat4x4 mt = MatfromQuat(yrot, (vec4f){ 0.f, 0.f, 0.f, 0.f });

    m->cd.v[P] = vecxm(m->cd.v[P], mt);
    m->Q = multiplyQuats(m->Q, yrot);

    m->r = yrot;

    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, mt);
}
/* Rotates Mesh according to own axis in relation with its cd.v[P] point. */
const void rotateMesh(Mesh *m, const float angle, float x, float y, float z) {
    vec4f axis = { x, y, z };

    m->r = rotationQuat(angle, axis);
    m->rot_angle = angle;
}


