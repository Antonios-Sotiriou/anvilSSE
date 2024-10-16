#ifndef _SHADOWSHADER_H
#define _SHADOWSHADER_H 1

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

/* Global variables. Defined in main.c. used by all shaders with small name changes. */
extern int shadowShaderProgram;
extern GLint shadowMapFBO;
extern Mat4x4 lookAtMatrix, viewMatrix, lightMatrix, orthoMatrix;
extern Scene scene; 

const int initShadowShader(void);
const void shadowCast(void);

#endif /* _SHADOWSHADER_H */


