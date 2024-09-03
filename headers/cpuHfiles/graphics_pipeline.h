#ifndef _GRAPHICS_PIPELINE_H
#define _GRAPHICS_PIPELINE_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
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

#ifndef _DRAW_FUNCTIONS_H
    #include "draw_functions.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

/* Defined in main.c. */
extern int PROJECTIONVIEW, DEBUG;
extern vec4i half_screen;
extern float FPlane, NPlane;
extern XWindowAttributes main_wa;
extern Scene scene;
extern Tile *tiles, *point_tiles;
extern Mat4x4 viewMat, worldMat, lookAt, *point_mat;
extern vec4c *frame_buffer, *point_frame_buffer;
extern float *main_depth_buffer, *point_depth_buffer;
extern pthread_t threads[THREADS];

void *graphicsPipeline(void *args);

#endif /* _GRAPHICS_PIPELINE_H */


