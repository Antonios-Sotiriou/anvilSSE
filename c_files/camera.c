#include "../headers/camera.h"

/* Rotates the camera to look left. */
const void look_left(Mesh *m, const float angle) {
    Quat xrot = rotationQuat(-2, m->cd.v[V]);
    Mat4x4 tm = MatfromQuat(xrot, m->cd.v[P]);

    setvecsarrayxm(m->cd.v, 4, tm);
    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, tm);
    m->Q = multiplyQuats(m->Q, xrot);
    m->r = xrot;

}
/* Rotates the camera to look right. */
const void look_right(Mesh *m, const float angle) {
    Quat xrot = rotationQuat(2, m->cd.v[V]);
    Mat4x4 tm = MatfromQuat(xrot, m->cd.v[P]);

    setvecsarrayxm(m->cd.v, 4, tm);
    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, tm);
    m->Q = multiplyQuats(m->Q, xrot);
    m->r = xrot;
}
/* Rotates the camera to look Up. */
const void look_up(Mesh *m, const float angle) {
    Quat xrot = rotationQuat(2, m->cd.v[U]);
    Mat4x4 tm = MatfromQuat(xrot, m->cd.v[P]);

    setvecsarrayxm(m->cd.v, 4, tm);
    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, tm);
    m->Q = multiplyQuats(m->Q, xrot);
    m->r = xrot;
}
/* Rotates the camera to look Down. */
const void look_down(Mesh *m, const float angle) {
    Quat xrot = rotationQuat(-2, m->cd.v[U]);
    Mat4x4 tm = MatfromQuat(xrot, m->cd.v[P]);

    setvecsarrayxm(m->cd.v, 4, tm);
    setvecsarrayxm(m->bbox.v, m->bbox.v_indexes, tm);
    m->Q = multiplyQuats(m->Q, xrot);
    m->r = xrot;
}
/* Moves camera position forward. */
const void move_forward(Mesh *m, const float vel) {
    m->mvdir = m->cd.v[N];
    m->momentum = vel;
}
/* Moves camera position backwards. */
const void move_backward(Mesh *m, const float vel) {
    m->mvdir = -m->cd.v[N];
    m->momentum = vel;
}
/* Moves camera position left. */
const void move_left(Mesh *m, const float vel) {
    m->mvdir = -m->cd.v[U];
    m->momentum = vel;
}
/* Moves camera position right. */
const void move_right(Mesh *m, const float vel) {
    m->mvdir = m->cd.v[U];
    m->momentum = vel;
}
/* Moves camera position Up. */
const void move_up(Mesh *m, const float vel) {
    m->mvdir = -m->cd.v[V];
    m->momentum = vel;
}
/* Moves camera position Down. */
const void move_down(Mesh *m, const float vel) {
    m->mvdir = m->cd.v[V];
    m->momentum = vel;
}


