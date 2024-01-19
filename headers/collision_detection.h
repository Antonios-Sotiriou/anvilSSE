#ifndef _COLLISION_DETECTION_H
#define _COLLISION_DETECTION_H 1

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
extern float *height_map;
extern XWindowAttributes main_wa;
extern int HALFW, HALFH;

/* Defined in height_pipeline.c. */
// extern Mat4x4 viewHeight, heightWorld;

const int checkCollision(Scene *s);

#endif /* _COLLISION_DETECTION_H */



