#include "headers/matrices.h"

/* Some usefull masks to shuffle vectors with builtins SSE gcc. */
const static vec4i xmask = { 0, 0, 0, 0 };
const static vec4i ymask = { 1, 1, 1, 1 };
const static vec4i zmask = { 2, 2, 2, 2 };
const static vec4i wmask = { 3, 3, 3, 3 };

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
/* Rotate Object on X axis according the world origin */
const Mat4x4 rotateXMatrix(const float angle) {
    Mat4x4 m = { 0 };
    m.m[0][0] = 1.0;
    m.m[1][1] = cosf(angle);
    m.m[1][2] = -sinf(angle);
    m.m[2][1] = sinf(angle);
    m.m[2][2] = cosf(angle);
    m.m[3][3] = 1.0;
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
/* Rotate Object on Z axis according the world origin */
const Mat4x4 rotateZMatrix(const float angle) {
    Mat4x4 m = { 0 };
    m.m[0][0] = cosf(angle);
    m.m[0][1] = sinf(angle);
    m.m[1][0] = -sinf(angle);
    m.m[1][1] = cosf(angle);
    m.m[2][2] = 1.0;
    m.m[3][3] = 1.0;
    return m;
}
/* Orthographic Projection Matrix. */
const Mat4x4 orthographicMatrix(const float scaleX, const float scaleY, const float transX, const float transY, const float zn, const float zf) {
    Mat4x4 m = { 0 };
    m.m[0][0] = scaleX;
    m.m[1][1] = scaleY;
    m.m[2][2] = 1.00 / (zf - zn);
    m.m[3][0] = transX;
    m.m[3][1] = transY;
    m.m[3][2] = ( (zf + zn) / (zf - zn) );
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
/* Reverse Perspective Projection Matrix. */
const Mat4x4 reperspectiveMatrix(const float fov, const float aspectratio) {
    Mat4x4 m = { 0 };
    const float fovRadius = 1 / tanf(fov * 0.5 / 180.0 * 3.14159);
    m.m[0][0] = aspectratio / fovRadius;
    m.m[1][1] = aspectratio / fovRadius;
    m.m[3][2] = 1.0;
    m.m[3][3] = 1.0;
    return m;
}
/* Multiplies a Mesh c with the given Matrix and returns a new Mesh, leaving the original unmodified. */
vec4f *meshxm(vec4f vecs[], const int len, const Mat4x4 m) {
    vec4f *r = malloc(16 * len);
    for (int i = 0; i < len; i++) {
        r[i] = __builtin_shuffle(vecs[i], xmask) * m.m[0] + __builtin_shuffle(vecs[i], ymask) * m.m[1] + __builtin_shuffle(vecs[i], zmask) * m.m[2] + __builtin_shuffle(vecs[i], wmask) * m.m[3];
    }
    return r;
}
/* Multiplies a Mesh normals with the given Matrix. */
vec4f *normalsxm(vec4f vecs[], const int len, const Mat4x4 m) {
    vec4f *r = malloc(16 * len);
    for (int i = 0; i < len; i++) {
        r[i] = __builtin_shuffle(vecs[i], xmask) * m.m[0] + __builtin_shuffle(vecs[i], ymask) * m.m[1] + __builtin_shuffle(vecs[i], zmask) * m.m[2] + __builtin_shuffle(vecs[i], wmask) * m.m[3];
    }
    return r;
}
/* Multiplies a vec4 with the given Matrix and returns a new vec4, leaving the original unmodified. */
const vec4f vecxm(const vec4f v, const Mat4x4 m) {
    return  __builtin_shuffle(v, xmask) * m.m[0] + __builtin_shuffle(v, ymask) * m.m[1] + __builtin_shuffle(v, zmask) * m.m[2] + __builtin_shuffle(v, wmask) * m.m[3];
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
/* The Camera Matrix or as used to called the View Matrix.Returns a new 4x4 Matrix. */
const Mat4x4 pointat(const vec4f P, const vec4f T, const vec4f Up) {
    Mat4x4 m = { 0 };
    vec4f N = norm_vec(sub_vecs(P, T));
    vec4f U = norm_vec(cross_product(Up, N));
    vec4f V = cross_product(N, U);

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
    rm.m[3] = -(__builtin_shuffle(m.m[3], xmask) * rm.m[0] + __builtin_shuffle(m.m[3], ymask) * rm.m[1] + __builtin_shuffle(m.m[3], zmask) * rm.m[2] + __builtin_shuffle(m.m[3], wmask) * rm.m[3]);
    rm.m[3][3] = 1.0;
    return rm;
}
/* Inverts the given Matrix m returning a new 4x4 Matrix. */
const Mat4x4 transpose_mat(const Mat4x4 m) {
    Mat4x4 rm = { 0 };
    rm.m[0][0] =   m.m[0][0];    rm.m[0][1] = m.m[1][0];    rm.m[0][2] = m.m[2][0];    rm.m[0][3] = m.m[3][0];
    rm.m[1][0] =   m.m[0][1];    rm.m[1][1] = m.m[1][1];    rm.m[1][2] = m.m[2][1];    rm.m[1][3] = m.m[3][1];
    rm.m[2][0] =   m.m[0][2];    rm.m[2][1] = m.m[1][2];    rm.m[2][2] = m.m[2][2];    rm.m[2][3] = m.m[3][2];
    rm.m[3][0] =   m.m[0][3];    rm.m[3][1] = m.m[1][3];    rm.m[3][2] = m.m[2][3];    rm.m[3][3] = m.m[3][3];
    return rm;
}
/* Multiplies two given Matrices m1, m2.Returns a new 4x4 Matrix. */
const Mat4x4 mxm(const Mat4x4 m1, const Mat4x4 m2) {
    Mat4x4 r = { 0 };
    for (int i = 0; i < 4; i++)
        r.m[i] = __builtin_shuffle(m1.m[i], xmask) * m2.m[0] + __builtin_shuffle(m1.m[i], ymask) * m2.m[1] + __builtin_shuffle(m1.m[i], zmask) * m2.m[2] + __builtin_shuffle(m1.m[i], wmask) * m2.m[3];
    return r;
}

