#ifndef _GRAFIK_PIPELINE_H
#define _GRAFIK_PIPELINE_H 1

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

#ifndef _DRAW_FUNCTIONS_H
    #include "draw_functions.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

extern int HALFH, HALFW, PROJECTIONVIEW, DEBUG;
extern float FPlane, NPlane;
extern XWindowAttributes wa;
extern Mat4x4 viewMat, worldMat, lookAt;

const void grafikPipeline(Scene s);

#endif /* _GRAFIK_PIPELINE_H */


