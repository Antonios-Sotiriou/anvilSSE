#ifndef _CLICKSELECTSHADER_H
#define _CLICKSELECTSHADER_H 1

#ifndef STDIO_H
    #include <stdio.h>
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

#ifndef _ANVIL_STRUCTS_H
    #include "../components/anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "../components/matrices.h"
#endif

#ifndef _TERRAIN_FUNCTIONS_H
    #include "../components/terrain_functions.h"
#endif

/* Global variables. Defined in main.c. used by all shaders with small name changes. */
extern int clickSelectShaderProgram;
extern GLint transformLocI, clickSelectMapFBO;
extern Display *displ;
extern Window mainwin;
extern XWindowAttributes main_wa;
extern Mat4x4 worldMatrix, reperspMatrix, lookAtMatrix;
extern Scene scene;
extern vec4f click;

extern const int frustumCulling(vec4f v[], const int v_indexes, const Mat4x4 wm);

const int initClickSelectShader(void);
// const int clickSelect();
const int pointScreenToWorld(const vec4f cs);
const int pointVsBbox(TerrainInfo *ti, Scene *s, vec4f wsc);

#endif /* _CLICKSELECTSHADER_H */


