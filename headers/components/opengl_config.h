#ifndef _OPENGL_CONFIG_H
#define _OPENGL_CONFIG_H 1

#ifndef _STDIO_H
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

/* Defined in main.c. */
extern Display *displ;
extern Window mainwin;
extern GLXContext glc;
extern XVisualInfo *vinfo;
extern XWindowAttributes main_wa;
extern GLint VBO, testTexture[4], shadowDepthMap, shadowMapFBO, mainColorMap, mainDepthMap, mainInfoMap, mainFBO;

const void setupGL(void);
const void createBuffers(void);
const void createTextures(void);

#endif /* _OPENGL_CONFIG_H */


