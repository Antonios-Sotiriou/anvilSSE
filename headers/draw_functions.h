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

const void drawLine(float x1, float y1, float x2, float y2, vec4f color);

const void edgeMesh(const Mesh m, const vec4f color);

const void fillMesh(const Mesh m);
const void edgefillface(const face f, const Material mtr);
const void scanlinefillface(const face f, const Material mtr);

const void texMesh(const Mesh m);
const void edgetexface(const face f, const Material mtr);
const void scanlinetexface(const face f, const Material mtr);

#endif /* _DRAW_FUNCTIONS_H */

