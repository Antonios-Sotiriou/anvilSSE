#ifndef _MATRICES_H
#define _MATRICES_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATH_H
    #include <math.h>
#endif

/* 3D graphics Matrices and computations. */
const Mat4x4 scaleMatrix(const float scale);
const Mat4x4 translationMatrix(const float x, const float y, const float z);
const Mat4x4 rotateYMatrix(const float angle);
const Mat4x4 perspectiveMatrix(const float fov, const float aspectratio, const float zn, const float zf);
vec4f *meshxm(vec4f vecs[], const int len, const Mat4x4 m);
const Mat4x4 lookat(const vec4f P, const vec4f U, const vec4f V, const vec4f N);
const Mat4x4 inverse_mat(const Mat4x4 m);
const Mat4x4 mxm(const Mat4x4 m1, const Mat4x4 m2);

#endif /* _MATRICES_H */

