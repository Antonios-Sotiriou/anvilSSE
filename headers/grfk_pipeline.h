#ifndef _GRFK_PIPELINE_H
#define _GRFK_PIPELINE_H 1

#ifndef _ANVIL_STRUCTS_H
    #include "anvil_structs.h"
#endif

#ifndef _MATRICES_H
    #include "matrices.h"
#endif

#ifndef _CLIPPING_H
    #include "clipping.h"
#endif

#ifndef _VECTORS_MATH_H
    #include "vectors_math.h"
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

const void shadowPipeline(Scene s);
const static void initfaceVerticesShadow(Mesh *m, const int len);
const void grfkPipeline(Scene s);
const static void initfaceVertices(Mesh *m, const int len);
const static void ppdiv(Mesh *m, const int len);
const static Mesh bfculling(const Mesh c, const int len);
const static void viewtoscreen(Mesh *m, const int len);
const static void rasterize(const Mesh m);

#endif /* _GRFK_PIPELINE_H */