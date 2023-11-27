#ifndef _LIGHTING_H
#define _LIGHTING_H 1

#ifndef _STRING_H
    #include <string.h>
#endif

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _VECTORS_MATH_H
    #include "vectors_math.h"
#endif

/* Defined in main.c. */
extern XWindowAttributes *point_attrib;
extern int HALFW;
extern int HALFH;
extern Mat4x4 reperspMat;
extern u_int8_t *point_buffer;
extern Light sunlight;
extern float AmbientStrength, SpecularStrength;

/* Defined in shadow_pipeline.c. */
extern const float shadowTest(vec4f frag);

/* Defined in draw_functions.c. */
extern const vec4i rgbmask;

const float phong(vec4f nm, const Material mtr, const float pixX, const float pixY, const float pixZ, const float pixW);

#endif /* _LIGHTING_H */


