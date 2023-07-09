#include "headers/logging.h"

const void logVec4f(const vec4f v) {
    fprintf(stdout, "x: %f    y: %f    z: %f    w: %f\n", v[0], v[1], v[2], v[3]);
}

const void logVec4i(const vec4i v) {
    fprintf(stdout, "x: %d    y: %d    z: %d    w: %f\n", v[0], v[1], v[2], v[3]);
}

// const void logPixel(const Pixel p) {
//     fprintf(stdout, "Red: %u    Green: %u    Blue: %u\n", p.Red, p.Green, p.Blue);
// }

/* Logging Triangle values.If vec = 1 loggs vec4 values, if tex Texture, if norm Normal. */
const void logFace(const face f, const int vec, const int tex, const int norm) {
    // if (vec) {
        fprintf(stdout, "{ %d, %d, %d }, { %d, %d, %d }, { %d, %d, %d }\n", f.a[0], f.a[1], f.a[2], f.b[0], f.b[1], f.b[2], f.c[0], f.c[1], f.c[2]);
    // } else if (tex) {
    //     fprintf(stdout, "t.vt[0].u: %f    t.vt[0].v: %f    t.v[0].w: %f\n", t.vt[0].u, t.vt[0].v, t.vt[0].w);
    //     fprintf(stdout, "t.vt[1].u: %f    t.vt[1].v: %f    t.v[1].w: %f\n", t.vt[1].u, t.vt[1].v, t.vt[1].w);
    //     fprintf(stdout, "t.vt[2].u: %f    t.vt[2].v: %f    t.v[2].w: %f\n\n", t.vt[2].u, t.vt[2].v, t.vt[2].w);
    // } else if (norm) {
    //     logVec4f(t.fn);
    // }
}

const void logMatrix(const Mat4x4 m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            fprintf(stdout, "m.m[%d][%d]: %.6f    ", i, j, m.m[i][j]);
        }
        fprintf(stdout, "\n");
    }
}

// const void logQuat(const Quat q) {
//     fprintf(stdout, "w: %f  v[x: %f, y: %f, z: %f]\n", q.w, q.v.x, q.v.y, q.v.z);
// }

const void logEvent(const XEvent e) {
    fprintf(stdout, "event.type: %d\n", e.type);
}

