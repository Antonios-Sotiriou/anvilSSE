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
extern vec4i half_screen;
extern int PROJECTIONVIEW;
extern unsigned int SMA;
extern unsigned int SMB;
extern unsigned int SMC;
extern unsigned int SMD;
extern unsigned int STA;
extern unsigned int STB;
extern unsigned int STC;
extern unsigned int STD;
extern float FPlane, NPlane, ASPECTRATIO;
extern XWindowAttributes main_wa;
extern Mat4x4 ortholightMat[4], viewMat;
extern float *shadow_buffer[4], shadow_bias;

extern Mesh *eye;

/* For investigating shadow maps. Defined in main.c */
extern unsigned int SMA, SMB, SMC, STA, STB ,STC;

vec4f *worldSpaceFrustum(const float np, const float fp);
const Mat4x4 createOrthoMatrixFromLimits(const DimensionsLimits dl);
const void createCascadeShadowMatrices(void);
const void shadowPipeline(Scene *s, const int sm_index);
const float shadowTest(vec4f frag);

#endif /* _SHADOW_PIPELINE_H */


