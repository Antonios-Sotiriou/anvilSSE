#include "../headers/camera.h"

/* Rotates the camera to look left. */
const void look_left(Mesh *m, const float angle) {
    m->rot_angle = -angle;
    Quat xrot = rotationQuat(m->rot_angle, m->cd.v[V]);
    m->r = xrot;
}
/* Rotates the camera to look right. */
const void look_right(Mesh *m, const float angle) {
    m->rot_angle = angle;
    Quat xrot = rotationQuat(m->rot_angle, m->cd.v[V]);
    m->r = xrot;
}
/* Rotates the camera to look Up. */
const void look_up(Mesh *m, const float angle) {
    m->rot_angle = -angle;
    Quat xrot = rotationQuat(m->rot_angle, m->cd.v[U]);
    m->r = xrot;
}
/* Rotates the camera to look Down. */
const void look_down(Mesh *m, const float angle) {
    m->rot_angle = angle;
    Quat xrot = rotationQuat(m->rot_angle, m->cd.v[U]);
    m->r = xrot;
}
/* Moves camera position forward. */
const void move_forward(Mesh *m, const float force) {
    m->mvdir = m->cd.v[N];
    m->momentum = force * m->mass;
}
/* Moves camera position backwards. */
const void move_backward(Mesh *m, const float force) {
    m->mvdir = -m->cd.v[N];
    m->momentum = force * m->mass;
}
/* Moves camera position left. */
const void move_left(Mesh *m, const float force) {
    m->mvdir = -m->cd.v[U];
    m->momentum = force * m->mass;
}
/* Moves camera position right. */
const void move_right(Mesh *m, const float force) {
    m->mvdir = m->cd.v[U];
    m->momentum = force * m->mass;
}
/* Moves camera position Up. */
const void move_up(Mesh *m, const float force) {
    m->mvdir = -m->cd.v[V];
    m->momentum = force * m->mass;
}
/* Moves camera position Down. */
const void move_down(Mesh *m, const float force) {
    m->mvdir = m->cd.v[V];
    m->momentum = force * m->mass;
}


