#include "headers/matrices.h"

/* Scale Matrix. */
const Mat4x4 scaleMatrix(const float scale) {
    Mat4x4 m = { 0 };
    m.m[0][0] = scale;
    m.m[1][1] = scale;
    m.m[2][2] = scale;
    m.m[3][3] = 1.0;
    return m;
}
/* Translation Matrix. */
const Mat4x4 translationMatrix(const float x, const float y, const float z) {
    Mat4x4 m = { 0 };
    m.m[0][0] = 1.0;
    m.m[1][1] = 1.0;
    m.m[2][2] = 1.0;
    m.m[3][3] = 1.0;
    m.m[3][0] = x;
    m.m[3][1] = y;
    m.m[3][2] = z;
    return m;
}
/* Rotate Object on Y axis according the world origin. */
const Mat4x4 rotateYMatrix(const float angle) {
    Mat4x4 m = { 0 };
    m.m[0][0] = cosf(angle);
    m.m[1][1] = 1.00;
    m.m[0][2] = -sinf(angle);
    m.m[2][0] = sinf(angle);
    m.m[2][2] = cosf(angle);
    m.m[3][3] = 1.0;
    return m;
}
/* Perspective Projection Matrix. */
const Mat4x4 perspectiveMatrix(const float fov, const float aspectratio, const float zn, const float zf) {
    Mat4x4 m = { 0 };
    float fovRadius = 1 / tanf(fov * 0.5 / 180.0 * 3.14159);
    m.m[0][0] = fovRadius;
    m.m[1][1] = aspectratio * fovRadius;
    m.m[2][2] = (zf / (zf - zn));
    m.m[2][3] = 1.0;
    m.m[3][2] = ((zf * zn) / (zf - zn));
    return m;
}
/* Multiplies a Mesh c with the given Matrix and returns a new Mesh, leaving the original unmodified. */
vec4f *meshxm(vec4f vecs[], const int len, const Mat4x4 m) {
    vec4f *r = malloc(16 * len);
    for (int i = 0; i < len; i++) {
        r[i][0] = vecs[i][0] * m.m[0][0] + vecs[i][1] * m.m[1][0] + vecs[i][2] * m.m[2][0] + vecs[i][3] * m.m[3][0];
        r[i][1] = vecs[i][0] * m.m[0][1] + vecs[i][1] * m.m[1][1] + vecs[i][2] * m.m[2][1] + vecs[i][3] * m.m[3][1];
        r[i][2] = vecs[i][0] * m.m[0][2] + vecs[i][1] * m.m[1][2] + vecs[i][2] * m.m[2][2] + vecs[i][3] * m.m[3][2];
        r[i][3] = vecs[i][0] * m.m[0][3] + vecs[i][1] * m.m[1][3] + vecs[i][2] * m.m[2][3] + vecs[i][3] * m.m[3][3];
    }
    return r;
}
/* The Camera Matrix or as used to called the View Matrix.Returns a new 4x4 Matrix. */
const Mat4x4 lookat(const vec4f P, const vec4f U, const vec4f V, const vec4f N) {
    Mat4x4 m = { 0 };
    m.m[0][0] = U[0];    m.m[0][1] = U[1];    m.m[0][2] = U[2];    m.m[0][3] = 0.0;
    m.m[1][0] = V[0];    m.m[1][1] = V[1];    m.m[1][2] = V[2];    m.m[1][3] = 0.0;
    m.m[2][0] = N[0];    m.m[2][1] = N[1];    m.m[2][2] = N[2];    m.m[2][3] = 0.0;
    m.m[3][0] = P[0];    m.m[3][1] = P[1];    m.m[3][2] = P[2];    m.m[3][3] = 1.0;
    return m;
}
/* Inverts the given Matrix m returning a new 4x4 Matrix. */
const Mat4x4 inverse_mat(const Mat4x4 m) {
    Mat4x4 rm = { 0 };
    rm.m[0][0] =   m.m[0][0];    rm.m[0][1] = m.m[1][0];    rm.m[0][2] = m.m[2][0];    rm.m[0][3] = 0.0;
    rm.m[1][0] =   m.m[0][1];    rm.m[1][1] = m.m[1][1];    rm.m[1][2] = m.m[2][1];    rm.m[1][3] = 0.0;
    rm.m[2][0] =   m.m[0][2];    rm.m[2][1] = m.m[1][2];    rm.m[2][2] = m.m[2][2];    rm.m[2][3] = 0.0;
    rm.m[3][0] = -(m.m[3][0] * rm.m[0][0] + m.m[3][1] * rm.m[1][0] + m.m[3][2] * rm.m[2][0]);
    rm.m[3][1] = -(m.m[3][0] * rm.m[0][1] + m.m[3][1] * rm.m[1][1] + m.m[3][2] * rm.m[2][1]);
    rm.m[3][2] = -(m.m[3][0] * rm.m[0][2] + m.m[3][1] * rm.m[1][2] + m.m[3][2] * rm.m[2][2]);
    rm.m[3][3] = 1.0;
    return rm;
}
/* Multiplies two given Matrices m1, m2.Returns a new 4x4 Matrix. */
const Mat4x4 mxm(const Mat4x4 m1, const Mat4x4 m2) {
    Mat4x4 r = { 0 };
    for (int i = 0; i < 4; i++) 
        for (int j = 0; j < 4; j++) {
            r.m[j][i] = m1.m[j][0] * m2.m[0][i] + m1.m[j][1] * m2.m[1][i] + m1.m[j][2] * m2.m[2][i] + m1.m[j][3] * m2.m[3][i];
        }
    return r;
}

