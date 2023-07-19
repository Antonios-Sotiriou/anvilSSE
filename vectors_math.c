#include "headers/vectors_math.h"

/* Computes the length of a vector with the use of Pythagorean Theorem. */
const float len_vec(const vec4f v) {
    return sqrtf(dot_product(v, v));
}
/* Normalizes a vec4 so he ranges from 0 to 1.Returns a new vec4. */
const vec4f norm_vec(const vec4f v) {
    float len = len_vec(v);
    return v / len;
}
/* Multiplies a vec4f by the given num.Returns a new vec4f. */
const vec4f mul_vec(const vec4f v1, const float num) {
    return v1 * num;
}
/* Divides a vec4f by the given num.Returns a new vec4f. */
const vec4f div_vec(const vec4f v1, const float num) {
    return v1 / num;
}
/* Adits two vec4fs.Returns a new vec4f. */
const vec4f add_vecs(const vec4f v1, const vec4f v2) {
    return v1 + v2;
}
/* Subtracts two vec4fs.Returns a new vec4f. */
const vec4f sub_vecs(const vec4f v1, const vec4f v2) {
    return v1 - v2;
}
/* Identifies if the Vectors are in clockwise order < CW > or not < CCW >. */
const float winding(const face f) {
    vec4i mask = { 1, 2, 0, 3 };
    vec4f xs = { f.v[0][0], f.v[1][0], f.v[2][0], 0.0f };
    vec4f ys = { f.v[0][1], f.v[1][1], f.v[2][1], 0.0f };

    vec4f r = xs * __builtin_shuffle(ys, mask) - ys * __builtin_shuffle(xs, mask);
    return r[0] + r[1] + r[2];

    // float step1 = (f.v[0][0] * f.v[1][1]) - (f.v[0][1] * f.v[1][0]);
    // float step2 = (f.v[1][0] * f.v[2][1]) - (f.v[1][1] * f.v[2][0]);
    // float step3 = (f.v[2][0] * f.v[0][1]) - (f.v[2][1] * f.v[0][0]);
    // return step1 + step2 + step3;
}
/* Computes the Dot Product of two given Vectors. */
float dot_product(const vec4f v1, const vec4f v2) {
    vec4f r = v1 * v2;
    return r[0] + r[1] + r[2];
}
/* Computes the Cross Product of two given Vectors.Returns a new vec4. */ 
const vec4f cross_product(const vec4f v1, const vec4f v2) {
    vec4i mask1 = { 1, 2, 0, 3 }, mask2 = { 2, 0, 1, 3 };
    return __builtin_shuffle(v1, mask1) * __builtin_shuffle(v2, mask2) - __builtin_shuffle(v1, mask2) * __builtin_shuffle(v2, mask1);
    
    // vec4f cp;
    // cp[0] = v1[1] * v2[2] - v1[2] * v2[1];
    // cp[1] = v1[2] * v2[0] - v1[0] * v2[2];
    // cp[2] = v1[0] * v2[1] - v1[1] * v2[0];
    // return cp;
}
/* Computes the Cross Product of a given Triangle.Returns a vec4 which represents the Cross Product. */
const vec4f triangle_cp(const face f) {
    return cross_product(f.v[1] - f.v[0], f.v[2] - f.v[0]);
}


