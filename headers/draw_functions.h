#ifndef _DRAW_FUNCTIONS_H
#define _DRAW_FUNCTIONS_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _LIGHTING_H
    #include "lighting.h"
#endif

#ifndef _MATH_H
    #include "math.h"
#endif

#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STRING_H
    #include <string.h>
#endif

/* Defined in main.c file. */
extern int EDGEFUNC, SCANLINE;
extern u_int8_t *frame_buffer;
extern float *point_depth_buffer;
extern XWindowAttributes *point_attrib;
extern Fragment *frags_buffer;

/* Defined in general_functions.c. */
extern const void swap(void *a, void *b, unsigned long size);


const void drawLine(float x1, float y1, float x2, float y2, vec4f color);

const void edgeMesh(Mesh *m, const vec4f color);

const void fillMesh(Mesh *m, Material *mtr);
const void edgefillface(const face f, Material *mtr);
const void scanlinefillface(const face f, Material *mtr);

const void texMesh(Mesh *m, Material *mtr);
const void edgetexface(face *f, Material *mtr);
const void scanlinetexface(face *f, Material *mtr);

#endif /* _DRAW_FUNCTIONS_H */

