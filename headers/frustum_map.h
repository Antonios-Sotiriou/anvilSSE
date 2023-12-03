#ifndef _FRUSTUM_MAP_H
#define _FRUSTUM_MAP_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#include "world_objects.h"
#include "draw_functions.h"
#include "clipping.h"

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

extern u_int8_t *map_buffer, *point_buffer;
extern float *point_depth_buffer, *map_depth_buffer;
extern int HALFW, HALFH, DEBUG, PROJECTIONVIEW;
extern Mat4x4 lookAt, reperspMat, perspMat, orthoMat, *point_mat;
extern float FPlane, NPlane;
extern XWindowAttributes main_wa, map_wa, *point_attrib;
extern vec4f *eye;

const void mapPipeline(const Scene *s);

#endif /* _FRUSTUM_MAP_H */


