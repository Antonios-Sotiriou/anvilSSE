#include "../../headers/components/matrices.h"

/* Some usefull masks to shuffle vectors with builtins SSE gcc. */
const static vec4i xmask = { 0, 0, 0, 0 };
const static vec4i ymask = { 1, 1, 1, 1 };
const static vec4i zmask = { 2, 2, 2, 2 };
const static vec4i wmask = { 3, 3, 3, 3 };

/* Scale Matrix. */
const Mat4x4 identityMatrix(void) {
    Mat4x4 m = { 0 };
    m.m[0][0] = 1.0;
    m.m[1][1] = 1.0;
    m.m[2][2] = 1.0;
    m.m[3][3] = 1.0;
    return m;
}
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
const Mat4x4 orthographicMatrix(const float l, const float r, const float t, const float b, const float n, const float f) {
    Mat4x4 m = { 0 };
    m.m[0][0] = 2.f / (r - l);
    m.m[1][1] = 2.f / (b - t);
    m.m[2][2] = (1.00 / (n - f));
    m.m[3][0] = ( (r + l) / (r - l) );
    m.m[3][1] = ( (b + t) / (b - t) );
    m.m[3][2] = ( (f + n) / (n - f) );
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
/* Matrix which transforms from Clipp Space to Screen Space. */
const Mat4x4 screenSpaceMatrix(const int screen_height, const int screen_width) {
    Mat4x4 m = { 0 };
    m.m[0][0] = screen_width * 0.5;
    m.m[1][1] = screen_height * 0.5;
    m.m[2][2] = 1.0f;
    m.m[2][3] = 0.f;
    m.m[3][2] = 0.f;
    m.m[3][3] = 1.f;
    return m;
}
/* Multiplies a vec4f array with the given Matrix and returns a new array, which includes the original array information, leaving the original unmodified. */
vec4f *vecsarrayxm(vec4f vecs[], const int len, const Mat4x4 m) {
    vec4f *r = malloc(16 * len);
    for (int i = 0; i < len; i++) {
        r[i] = __builtin_shuffle(vecs[i], xmask) * m.m[0] + __builtin_shuffle(vecs[i], ymask) * m.m[1] + __builtin_shuffle(vecs[i], zmask) * m.m[2] + __builtin_shuffle(vecs[i], wmask) * m.m[3];
    }
    return r;
}
/* Multiplies a vec4f array with the given Matrix and returns a new array. Frees the original array resources. */
const void setvecsarrayxm(vec4f vecs[], const int len, const Mat4x4 m) {
    vec4f r;
    for (int i = 0; i < len; i++) {
        r = vecs[i];
        vecs[i] = __builtin_shuffle(r, xmask) * m.m[0] + __builtin_shuffle(r, ymask) * m.m[1] + __builtin_shuffle(r, zmask) * m.m[2] + __builtin_shuffle(r, wmask) * m.m[3];
    }
}
/* Multiplies a face array with the given Matrix and returns a new array, which includes the original array information, leaving the original unmodified. */
face *facesarrayxm(face fs[], const int len, const Mat4x4 m) {
    size_t arr_size = sizeof(face) * len;
    face *r = malloc(arr_size);
    for (int i = 0; i < len; i++) {
        r[i].v[0] = __builtin_shuffle(fs[i].v[0], xmask) * m.m[0] + __builtin_shuffle(fs[i].v[0], ymask) * m.m[1] + __builtin_shuffle(fs[i].v[0], zmask) * m.m[2] + __builtin_shuffle(fs[i].v[0], wmask) * m.m[3];
        r[i].v[1] = __builtin_shuffle(fs[i].v[1], xmask) * m.m[0] + __builtin_shuffle(fs[i].v[1], ymask) * m.m[1] + __builtin_shuffle(fs[i].v[1], zmask) * m.m[2] + __builtin_shuffle(fs[i].v[1], wmask) * m.m[3];
        r[i].v[2] = __builtin_shuffle(fs[i].v[2], xmask) * m.m[0] + __builtin_shuffle(fs[i].v[2], ymask) * m.m[1] + __builtin_shuffle(fs[i].v[2], zmask) * m.m[2] + __builtin_shuffle(fs[i].v[2], wmask) * m.m[3];
    }
    return r;
}
/* Multiplies a face array with the given Matrix and returns a new array. Frees the original array resources. */
const void setfacesarrayxm(face fs[], const int len, const Mat4x4 m) {
    face r;
    for (int i = 0; i < len; i++) {
        r = fs[i];
        fs[i].v[0] = __builtin_shuffle(r.v[0], xmask) * m.m[0] + __builtin_shuffle(r.v[0], ymask) * m.m[1] + __builtin_shuffle(r.v[0], zmask) * m.m[2] + __builtin_shuffle(r.v[0], wmask) * m.m[3];
        fs[i].v[1] = __builtin_shuffle(r.v[1], xmask) * m.m[0] + __builtin_shuffle(r.v[1], ymask) * m.m[1] + __builtin_shuffle(r.v[1], zmask) * m.m[2] + __builtin_shuffle(r.v[1], wmask) * m.m[3];
        fs[i].v[2] = __builtin_shuffle(r.v[2], xmask) * m.m[0] + __builtin_shuffle(r.v[2], ymask) * m.m[1] + __builtin_shuffle(r.v[2], zmask) * m.m[2] + __builtin_shuffle(r.v[2], wmask) * m.m[3];
    }
}
/* Multiplies a faces array vectors with vm Matrix and normals with nm Matrix and returns a new faces array, leaving the original unmodified. */
face *compinedFaceNormals(face fs[], const int len, const Mat4x4 vm, const Mat4x4 nm) {
    size_t arr_size = sizeof(face) * len;
    face *r = malloc(arr_size);
    memcpy(r, fs, arr_size);
    for (int i = 0; i < len; i++) {
        r[i].v[0] = __builtin_shuffle(fs[i].v[0], xmask) * vm.m[0] + __builtin_shuffle(fs[i].v[0], ymask) * vm.m[1] + __builtin_shuffle(fs[i].v[0], zmask) * vm.m[2] + __builtin_shuffle(fs[i].v[0], wmask) * vm.m[3];
        r[i].v[1] = __builtin_shuffle(fs[i].v[1], xmask) * vm.m[0] + __builtin_shuffle(fs[i].v[1], ymask) * vm.m[1] + __builtin_shuffle(fs[i].v[1], zmask) * vm.m[2] + __builtin_shuffle(fs[i].v[1], wmask) * vm.m[3];
        r[i].v[2] = __builtin_shuffle(fs[i].v[2], xmask) * vm.m[0] + __builtin_shuffle(fs[i].v[2], ymask) * vm.m[1] + __builtin_shuffle(fs[i].v[2], zmask) * vm.m[2] + __builtin_shuffle(fs[i].v[2], wmask) * vm.m[3];

        r[i].vn[0] = __builtin_shuffle(fs[i].vn[0], xmask) * nm.m[0] + __builtin_shuffle(fs[i].vn[0], ymask) * nm.m[1] + __builtin_shuffle(fs[i].vn[0], zmask) * nm.m[2] + __builtin_shuffle(fs[i].vn[0], wmask) * nm.m[3];
        r[i].vn[1] = __builtin_shuffle(fs[i].vn[1], xmask) * nm.m[0] + __builtin_shuffle(fs[i].vn[1], ymask) * nm.m[1] + __builtin_shuffle(fs[i].vn[1], zmask) * nm.m[2] + __builtin_shuffle(fs[i].vn[1], wmask) * nm.m[3];
        r[i].vn[2] = __builtin_shuffle(fs[i].vn[2], xmask) * nm.m[0] + __builtin_shuffle(fs[i].vn[2], ymask) * nm.m[1] + __builtin_shuffle(fs[i].vn[2], zmask) * nm.m[2] + __builtin_shuffle(fs[i].vn[2], wmask) * nm.m[3];
    }
    free(fs);
    return r;
}
/* Multiplies a vec4 with the given Matrix and returns a new vec4, leaving the original unmodified. */
const vec4f vecxm(const vec4f v, const Mat4x4 m) {
    return  __builtin_shuffle(v, xmask) * m.m[0] + __builtin_shuffle(v, ymask) * m.m[1] + __builtin_shuffle(v, zmask) * m.m[2] + __builtin_shuffle(v, wmask) * m.m[3];
}
/* Multiplies a vec4 with the given Matrix. */
const void setvecxm(vec4f *v, const Mat4x4 m) {
    const vec4f r = *v;
    *v = __builtin_shuffle(r, xmask) * m.m[0] + __builtin_shuffle(r, ymask) * m.m[1] + __builtin_shuffle(r, zmask) * m.m[2] + __builtin_shuffle(r, wmask) * m.m[3];
}
/* Multiplies a faces vectors with the given Matrix and returns a new face with only the vectors translated and modified, leaving the original unmodified. */
const face facexm(const face f, const Mat4x4 m) {
    face r;
    r.v[0] = __builtin_shuffle(f.v[0], xmask) * m.m[0] + __builtin_shuffle(f.v[0], ymask) * m.m[1] + __builtin_shuffle(f.v[0], zmask) * m.m[2] + __builtin_shuffle(f.v[0], wmask) * m.m[3];
    r.v[1] = __builtin_shuffle(f.v[1], xmask) * m.m[0] + __builtin_shuffle(f.v[1], ymask) * m.m[1] + __builtin_shuffle(f.v[1], zmask) * m.m[2] + __builtin_shuffle(f.v[1], wmask) * m.m[3];
    r.v[2] = __builtin_shuffle(f.v[2], xmask) * m.m[0] + __builtin_shuffle(f.v[2], ymask) * m.m[1] + __builtin_shuffle(f.v[2], zmask) * m.m[2] + __builtin_shuffle(f.v[2], wmask) * m.m[3];
    return r;
}
/* The Camera Matrix or as used to called the View Matrix.Returns a new 4x4 Matrix. */
const Mat4x4 lookat(const vec4f P, const vec4f U, const vec4f V, const vec4f N) {
    Mat4x4 m;
    m.m[0][0] = U[0];    m.m[0][1] = U[1];    m.m[0][2] = U[2];    m.m[0][3] = 0.0;
    m.m[1][0] = V[0];    m.m[1][1] = V[1];    m.m[1][2] = V[2];    m.m[1][3] = 0.0;
    m.m[2][0] = N[0];    m.m[2][1] = N[1];    m.m[2][2] = N[2];    m.m[2][3] = 0.0;
    m.m[3][0] = P[0];    m.m[3][1] = P[1];    m.m[3][2] = P[2];    m.m[3][3] = 1.0;
    return m;
}
/* The Camera Matrix or as used to called the View Matrix.Returns a new 4x4 Matrix. */
const Mat4x4 pointat(const vec4f P, const vec4f T, const vec4f Up) {
    Mat4x4 m;
    vec4f N = norm_vec(T - P);
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
    Mat4x4 rm;
    rm.m[0][0] =   m.m[0][0];    rm.m[0][1] = m.m[1][0];    rm.m[0][2] = m.m[2][0];    rm.m[0][3] = 0.0;
    rm.m[1][0] =   m.m[0][1];    rm.m[1][1] = m.m[1][1];    rm.m[1][2] = m.m[2][1];    rm.m[1][3] = 0.0;
    rm.m[2][0] =   m.m[0][2];    rm.m[2][1] = m.m[1][2];    rm.m[2][2] = m.m[2][2];    rm.m[2][3] = 0.0;
    rm.m[3] = -(__builtin_shuffle(m.m[3], xmask) * rm.m[0] + __builtin_shuffle(m.m[3], ymask) * rm.m[1] + __builtin_shuffle(m.m[3], zmask) * rm.m[2]);
    rm.m[3][3] = 1.0;
    return rm;
}
/* Inverts the given Matrix m returning a new 4x4 Matrix. */
const Mat4x4 transpose_mat(const Mat4x4 m) {
    Mat4x4 rm;
    rm.m[0][0] =   m.m[0][0];    rm.m[0][1] = m.m[1][0];    rm.m[0][2] = m.m[2][0];    rm.m[0][3] = m.m[3][0];
    rm.m[1][0] =   m.m[0][1];    rm.m[1][1] = m.m[1][1];    rm.m[1][2] = m.m[2][1];    rm.m[1][3] = m.m[3][1];
    rm.m[2][0] =   m.m[0][2];    rm.m[2][1] = m.m[1][2];    rm.m[2][2] = m.m[2][2];    rm.m[2][3] = m.m[3][2];
    rm.m[3][0] =   m.m[0][3];    rm.m[3][1] = m.m[1][3];    rm.m[3][2] = m.m[2][3];    rm.m[3][3] = m.m[3][3];
    return rm;
}
/* Multiplies two given Matrices m1, m2.Returns a new 4x4 Matrix. */
const Mat4x4 mxm(const Mat4x4 m1, const Mat4x4 m2) {
    Mat4x4 r;
    for (int i = 0; i < 4; i++)
        r.m[i] = __builtin_shuffle(m1.m[i], xmask) * m2.m[0] + __builtin_shuffle(m1.m[i], ymask) * m2.m[1] + __builtin_shuffle(m1.m[i], zmask) * m2.m[2] + __builtin_shuffle(m1.m[i], wmask) * m2.m[3];
    return r;
}


