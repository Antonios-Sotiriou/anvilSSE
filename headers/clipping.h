#ifndef _CLIPPING_H
#define _CLIPPING_H 1

#ifndef _OBJECTS_H
    #include "anvil_structs.h"
#endif

#ifndef _VECTORS_MATH_H
    #include "vectors_math.h"
#endif

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

const Mesh clipp(const Mesh c, vec4f plane_n, vec4f plane_p);
const vec4f plane_intersect(vec4f plane_n, vec4f plane_p, vec4f line_start, vec4f line_end, float *t);
float dist(vec4f plane_p, vec4f plane_n, vec4f v);
int clipp_triangle(vec4f plane_p, vec4f plane_n, face in_t, face *out_t1, face *out_t2);

#endif /* _CLIPPING_H */


