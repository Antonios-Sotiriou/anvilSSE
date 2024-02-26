#ifndef _SHADOW_PIPELINE_H
#define _SHADOW_PIPELINE_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _GENERAL_FUNCTIONS_H
    #include "general_functions.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _CLIPPING_H
    #include "clipping.h"
#endif

#ifndef _SCENE_OBJECTS_H
    #include "scene_objects.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

/* Defined in main.c. */
extern int HALFH, HALFW, PROJECTIONVIEW;
extern unsigned int SMA;
extern unsigned int SMB;
extern unsigned int SMC;
extern unsigned int STA;
extern unsigned int STB;
extern unsigned int STC;
extern float FPlane, NPlane, ASPECTRATIO;
extern XWindowAttributes main_wa;
extern Mat4x4 ortholightMat[3], viewMat,  *point_mat;
extern float *shadow_buffer[3], shadow_bias;
extern Light sunlight;
extern vec4f *eye, camera[4];

/* For investigating shadow maps. Defined in main.c */
extern unsigned int SMA, SMB, SMC, STA, STB ,STC;

vec4f *worldSpaceFrustum(const float np, const float fp);
const Mat4x4 createOrthoMatrixFromLimits(const DimensionsLimits dl);
const void createCascadeShadowMatrices(const unsigned int num_of_cascades);
const void shadowPipeline(Scene *s, const unsigned int sm_index);
const float shadowTest(vec4f frag, vec4f nml);

#endif /* _SHADOW_PIPELINE_H */


