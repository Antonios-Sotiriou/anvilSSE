#include "headers/camera.h"

extern float DeltaTime;
enum { Pos, U, V, N, C };

/* Rotates the camera to look left. */
const void look_left(vec4f *g, const float angle) {
    Quat u = setQuat(0, g[U]);
    Quat v = setQuat(0, g[V]);
    Quat n = setQuat(0, g[N]);

    Quat xrot = rotationQuat(2, g[V]);
    Quat rerot = conjugateQuat(xrot);

    Quat resu = multiplyQuats(multiplyQuats(xrot, u), rerot);
    Quat resv = multiplyQuats(multiplyQuats(xrot, v), rerot);
    Quat resn = multiplyQuats(multiplyQuats(xrot, n), rerot);

    g[U] = resu.v;
    g[V] = resv.v;
    g[N] = resn.v;
}
/* Rotates the camera to look right. */
const void look_right(vec4f *g, const float angle) {
    Quat u = setQuat(0, g[U]);
    Quat v = setQuat(0, g[V]);
    Quat n = setQuat(0, g[N]);

    Quat xrot = rotationQuat(-2, g[V]);
    Quat rerot = conjugateQuat(xrot);

    Quat resu = multiplyQuats(multiplyQuats(xrot, u), rerot);
    Quat resv = multiplyQuats(multiplyQuats(xrot, v), rerot);
    Quat resn = multiplyQuats(multiplyQuats(xrot, n), rerot);

    g[U] = resu.v;
    g[V] = resv.v;
    g[N] = resn.v;
}
/* Moves camera position forward. */
const void move_forward(vec4f *g, const float vel) {
    g[Pos] = g[Pos] + (g[N] * vel);
}
/* Moves camera position backwards. */
const void move_backward(vec4f *g, const float vel) {
    g[Pos] = g[Pos] - (g[N] * vel);
}
/* Moves camera position left. */
const void move_left(vec4f *g, const float vel) {
    g[Pos] = g[Pos] - (g[U] * vel);
}
/* Moves camera position right. */
const void move_right(vec4f *g, const float vel) {
    g[Pos] = g[Pos] + (g[U] * vel);
}
/* Moves camera position Up. */
const void move_up(vec4f *g, const float vel) {
    // g[Pos] = sub_vecs(g[Pos], multiply_vec(g->V, 0.1));
    g[Pos][1] += vel;
}
/* Moves camera position Down. */
const void move_down(vec4f *g, const float vel) {
    // g[Pos] = add_vecs(g[Pos], multiply_vec(g->V, 0.1));
    g[Pos][1] -= vel;
}