#ifndef _VECTORS_MATH_H
#define _VECTORS_MATH_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _IMMINTRIN_H
    #include <immintrin.h>
#endif

/* 3D graphics vec4f computation functions */
const vec4f mul_vec(const vec4f v1, const float num);
const vec4f div_vec(const vec4f v1, const float num);
const vec4f add_vecs(const vec4f v1, const vec4f v2);
const vec4f sub_vecs(const vec4f v1, const vec4f v2);
const float winding(const vec4f a, const vec4f b, const vec4f c);

#endif /* _VECTORS_MATH_H */

