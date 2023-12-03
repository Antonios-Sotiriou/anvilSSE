#include "headers/logging.h"

const void logVec4f(const vec4f v) {
    fprintf(stdout, "x: %f    y: %f    z: %f    w: %f\n", v[0], v[1], v[2], v[3]);
}

const void logVec4i(const vec4i v) {
    fprintf(stdout, "x: %d    y: %d    z: %d    w: %d\n", v[0], v[1], v[2], v[3]);
}

const void logVec4c(const vec4c v) {
    fprintf(stdout, "x: %d    y: %d    z: %d    w: %d\n", v[0], v[1], v[2], v[3]);
}

/* Logging Triangle values.If vec = 1 loggs vec4 values, if tex Texture, if norm Normal. */
const void logFace(const face f, const int vec, const int tex, const int norm, const int indexes) {
    if (vec) {
        fprintf(stdout, "v0 - { %f, %f, %f, %f }\n", f.v[0][0], f.v[0][1], f.v[0][2], f.v[0][3]);
        fprintf(stdout, "v1 - { %f, %f, %f, %f }\n", f.v[1][0], f.v[1][1], f.v[1][2], f.v[1][3]);
        fprintf(stdout, "v2 - { %f, %f, %f, %f }\n", f.v[2][0], f.v[2][1], f.v[2][2], f.v[2][3]);
    } else if (indexes) {
        fprintf(stdout, "{ %d, %d, %d }, { %d, %d, %d }, { %d, %d, %d }\n", f.a[0], f.a[1], f.a[2], f.b[0], f.b[1], f.b[2], f.c[0], f.c[1], f.c[2]);
    } else if (tex) {
        fprintf(stdout, "f.vt[0].u: %f    f.vt[0].v: %f\n", f.vt[0][0], f.vt[0][1]);
        fprintf(stdout, "f.vt[1].u: %f    f.vt[1].v: %f\n", f.vt[1][0], f.vt[1][1]);
        fprintf(stdout, "f.vt[2].u: %f    f.vt[2].v: %f\n\n", f.vt[2][0], f.vt[2][1]);
    } else if (norm) {
        fprintf(stdout, "v0 - { %f, %f, %f, %f }\n", f.vn[0][0], f.vn[0][1], f.vn[0][2], f.vn[0][3]);
        fprintf(stdout, "v1 - { %f, %f, %f, %f }\n", f.vn[1][0], f.vn[1][1], f.vn[1][2], f.vn[1][3]);
        fprintf(stdout, "v2 - { %f, %f, %f, %f }\n", f.vn[2][0], f.vn[2][1], f.vn[2][2], f.vn[2][3]);
    }
}

const void logMatrix(const Mat4x4 m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            fprintf(stdout, "m.m[%d][%d]: %.6f    ", i, j, m.m[i][j]);
        }
        fprintf(stdout, "\n");
    }
}

const void logQuat(const Quat q) {
    fprintf(stdout, "w: %f  v[x: %f, y: %f, z: %f]\n", q.w, q.v[0], q.v[1], q.v[2]);
}

const void logEvent(const XEvent e) {
    fprintf(stdout, "event.type: %d\n", e.type);
}

