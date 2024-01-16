#ifndef _HEIGHT_MAP_H
#define _HEIGHT_MAP_H 1

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

#ifndef _STDIO_H
    #include <stdio.h>
#endif

/* Defined in main.c. */
extern float *point_depth_buffer, *height_map, NPlane, FPlane;
extern vec4f birdview[4], camera[4];
extern Mat4x4 perspMat;
extern XWindowAttributes main_wa;
extern int HALFW, HALFH;

const void heightPipeline(Scene *s);

#endif /* _HEIGHT_MAP_H */



