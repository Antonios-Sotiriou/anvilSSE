#ifndef _SHADOW_PIPELINE_H
#define _SHADOW_PIPELINE_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _CLIPPING_H
    #include "clipping.h"
#endif

#ifndef _WORLD_OBJECTS_H
    #include "world_objects.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

/* Defined in main.c. */
extern int HALFH, HALFW, PROJECTIONVIEW;
extern float FPlane, NPlane;
extern XWindowAttributes *point_attrib, main_wa;
extern Mat4x4 ortholightMat[3], persplightMat, lookAt, reperspMat, lm;
extern float *shadow_buffer[3], shadow_bias;
extern Light sunlight;

/* Defined in general_functions.c. */
extern const void swap(void *a, void *b, unsigned long size);

const void shadowPipeline(Scene s, const unsigned int sm_index);
const float shadowTest(vec4f frag);

#endif /* _SHADOW_PIPELINE_H */


