#ifndef _MAINSHADER_H
#define _MAINSHADER_H 1

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

#ifndef _GENERAL_FUNCTIONS_H
    #include "../components/general_functions.h"
#endif

/* Global variables. Defined in main.c. used by all shaders with small name changes. */
extern int mainShaderProgram, DEBUG, DISPLAYBBOX, EYEPOINT, mesh_id, primitive_id, COLLISION, COLLIDINGFACE, COLLIDINGMESH;
extern GLint mainFBO;
extern Mat4x4 lookAtMatrix, viewMatrix, lightMatrix, perspMatrix, worldMatrix;
extern Scene scene;
extern GLenum drawBuffers[2];

const int initMainShader(void);
const void project(void);

#endif /* _MAINSHADER_H */


