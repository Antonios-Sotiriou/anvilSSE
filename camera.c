#include "headers/camera.h"

extern float DeltaTime;
enum { Pos, U, V, N, C };

/* Moves camera position forward. */
const void move_forward(vec4f *g) {
    g[Pos] = add_vecs(g[Pos], mul_vec(g[N], 0.2));
}
/* Moves camera position backwards. */
const void move_backward(vec4f *g) {
    g[Pos] = sub_vecs(g[Pos], mul_vec(g[N], 0.2));
}
/* Moves camera position left. */
const void move_left(vec4f *g) {
    g[Pos] = sub_vecs(g[Pos], mul_vec(g[U], 0.1));
}
/* Moves camera position right. */
const void move_right(vec4f *g) {
    g[Pos] = add_vecs(g[Pos], mul_vec(g[U], 0.1));
}
/* Moves camera position Up. */
const void move_up(vec4f *g) {
    // g[Pos] = sub_vecs(g[Pos], multiply_vec(g->V, 0.1));
    g[Pos][1] -= 0.1;
}
/* Moves camera position Down. */
const void move_down(vec4f *g) {
    // g[Pos] = add_vecs(g[Pos], multiply_vec(g->V, 0.1));
    g[Pos][1] += 0.1;
}