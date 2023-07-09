#include "headers/vectors_math.h"

typedef float vec4f __attribute__((vector_size(16)));

/* Multiplies a vec4f by the given num.Returns a new vec4f. */
const vec4f mul_vec(const vec4f v1, const float num) {
    return v1 * num;
}
/* Divides a vec4f by the given num.Returns a new vec4f. */
const vec4f div_vec(const vec4f v1, const float num) {
    return v1 / num;;
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
const float winding(const vec4f a, const vec4f b, const vec4f c) {
    float step1 = (a[0] * b[1]) - (a[1] * b[0]);
    float step2 = (b[0] * c[1]) - (b[1] * c[0]);
    float step3 = (c[0] * a[1]) - (c[1] * a[0]);
    return step1 + step2 + step3;
}


