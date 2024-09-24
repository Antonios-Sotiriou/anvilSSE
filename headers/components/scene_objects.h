#ifndef _SCENE_OBJECTS_H
#define _SCENE_OBJECTS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _QUATERNIONS_H
    #include "quaternions.h"
#endif

#ifndef _GENERAL_FUNCTIONS_H
    #include "general_functions.h"
#endif

#ifndef _TEST_SHAPES_H
    #include "test_shapes.h"
#endif

#ifndef _TERRAIN_FUNCTIONS_H
    #include "terrain_functions.h"
#endif

#ifndef _BOUNDINGBOX_LOADER_H
    #include "bbox_loader.h"
#endif

/* OpenGL headers. */
#ifndef __glew_h__
    #include <GL/glew.h>     /* libglew-dev */
#endif

#ifndef GLX_H
    #include <GL/glx.h>     /* libglx-dev */
#endif

#ifndef __glu_h__
    #include <GL/glu.h>     /* libglu1-mesa-dev */
#endif

/* Defined in main.c */
extern int mainShaderProgram;
extern GLint testTexture[4];
extern TerrainInfo Gitana;

const void initWorldObjects(Scene *s);
const void createScene(Scene *s);
const void releaseScene(Scene *s);

#endif /* _SCENE_OBJECTS_H */