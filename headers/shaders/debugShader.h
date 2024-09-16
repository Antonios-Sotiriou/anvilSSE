#ifndef _DEBUGSHADER_H
#define _DEBUGSHADER_H 1

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

/* Global variables. Defined in main.c. used by all shaders with small name changes. */
extern int debugShaderProgram;
extern GLint transformLocI;
extern Display *displ;
extern Window mainwin;
extern XWindowAttributes main_wa;

const int initDebugShader(void);
const void displayTexture(const int textureIndex);

#endif /* _DEBUGSHADER_H */


